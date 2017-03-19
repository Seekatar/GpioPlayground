#include <EEPROM.h>

// RTC
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

RTC_DS1307 rtc;
// end RTC

#ifdef __AVR__
#include <avr/power.h>
#endif

// neopixel
#include "NeoPixelWheel.h"

#define NEOPIXEL_LEDS 12

#define NEOPIXEL_DATA_PIN 6
#define ANALOG_COLOR_PIN A0
#define ANALOG_BRIGHTNESS_PIN A1

NeoPixelWheel wheel = NeoPixelWheel(NEOPIXEL_LEDS, NEOPIXEL_DATA_PIN, ANALOG_COLOR_PIN, ANALOG_BRIGHTNESS_PIN);

// end neopixel

// current time passed into all processors
DateTime current;

// include the processors and create array of them
#include "TimeEntry.h"
#include "NeoClock.h"
#include "Backlight.h"
#include "FadingBacklight.h"
#include "Nightlight.h"

TimeEntry timeEntry(current,wheel);
Processor *processors[] = { new Backlight(current,wheel), new NeoClock(current, wheel), new FadingBacklight(current,wheel), &timeEntry, new Nightlight(current, wheel, timeEntry) };
#define PROCESSOR_COUNT sizeof(processors)/sizeof(Processor*)

#define MODE_SWITCH_PIN 10
int mode = 0; 

DateTime compileTime;

const int MODE_INDEX = 0;  // EEPROM index

/*******************************************************************************
*******************************************************************************/
void setup() {
  compileTime = DateTime(F(__DATE__), F(__TIME__));

#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  Serial.begin(57600);

  pinMode(MODE_SWITCH_PIN, INPUT_PULLUP);

  EEPROM.get(MODE_INDEX, mode);
  if ( mode < 0 || mode >= PROCESSOR_COUNT)
    mode = 0;
  Serial.print(F("Got mode from eeprom ") );
  Serial.println(processors[mode]->name());

  rtcSetup();

  wheel.initialize();

  int newIndex = MODE_INDEX+sizeof(mode);
  for ( int i = 0; i < PROCESSOR_COUNT; i++ )
  {
    Serial.print("Initalizing ");
    Serial.println(processors[i]->name());
    newIndex = processors[i]->initialize(newIndex);
    Serial.println("ok");
  }

}
bool haveRtc = false;
bool pressed = false;
bool changingModes = false;

// helper to get time for testing if no UTC available
DateTime getCurrent()
{
  if ( haveRtc )
    return rtc.now();
  else
    return compileTime.unixtime() + millis() / 50; // speed up time! use 1000 for close to real time
}

/*******************************************************************************
*******************************************************************************/
void loop() {

  current = getCurrent();
  bool next = false;

  if ( mode < 0 || mode >= PROCESSOR_COUNT )
    mode = 0;
    
  next = processors[mode]->process(changingModes);
    
  changingModes = false;

  if ( digitalRead(MODE_SWITCH_PIN) == LOW  || next )
  {
    if ( !pressed || next )
    {
      next = false;
      pressed = true;
      mode++;
      if ( mode >= PROCESSOR_COUNT )
        mode = 0;
        
      EEPROM.put(MODE_INDEX, mode);

      changingModes = true;
      Processor::printTime(current);
      Serial.print(F(" Switching mode to "));
      Serial.println(processors[mode]->name());

      wheel.colorWipe();
    }
  }
  else
    pressed = false;
}

/*******************************************************************************
 * Setup the RealTimeClock breakout
*******************************************************************************/
void rtcSetup() {

  if (!rtc.begin()) {
    while (1)
    {
      Serial.println(F("Couldn't find RTC"));
      delay(1000);
    }
  }

  if ( rtc.isrunning() ) {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(compileTime);
  
    DateTime now = rtc.now();
    Serial.print( F("Rtc now is "));
    Serial.print(now.unixtime(),HEX);
    Serial.print(F(" which is "));
    Processor::printTime(now);
    Serial.println("");
    
    haveRtc = now.hour() <= 24 || now.minute() <= 60 || now.month() <= 12; // all above succeeds
  }
  else {
    Serial.println(F("RTC is NOT running!"));
  }
}


//#define DEBUG
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

#define NEOPIXEL_DATA_PIN 6
#define NEOPIXEL_LEDS 12
#define ANALOG_COLOR_PIN A0
#define ANALOG_BRIGHTNESS_PIN A1

NeoPixelWheel wheel = NeoPixelWheel(NEOPIXEL_LEDS, NEOPIXEL_DATA_PIN, ANALOG_COLOR_PIN, ANALOG_BRIGHTNESS_PIN);

// end neopixel

// clock settings
DateTime current;

uint32_t secColor = 0;
uint32_t minColor = 0;
uint32_t hrColor = 0;

#ifdef DEBUG
#define DEBUG_PRINT(n) Serial.print(n);
#define DEBUG_PRINTLN(n) {Serial.println(n); delay(100); }
#define DEBUG_PRINTTIME(n) printTime(n);
#else
#define DEBUG_PRINT(n) {}
#define DEBUG_PRINTLN(n) {}
#define DEBUG_PRINTTIME
#endif

void printTime( DateTime &dt )
{
  Serial.print( dt.hour() );
  int minutes = dt.minute();
  int seconds = dt.second();
  if ( minutes < 10 )
    Serial.print( ":0" );
  else
    Serial.print( ":" );
  Serial.print(minutes);
  if ( seconds < 10 )
    Serial.print( ":0" );
  else
    Serial.print( ":" );
  Serial.print(seconds);

}

// mode settings
#include "TimeEntry.h"
#include "NeoClock.h"

#define BACKLIGHT_MODE 0
#define CLOCK_MODE  1
#define FADING_BACKLIGHT_MODE 2
#define ALARM_ENTER 3
#define NIGHTLIGHT_MODE 4
char *modes[] = {"Backlight", "Clock", "Fader", "AlarmEnter", "Nightlight"};

TimeEntry timeEntry(current,wheel);
IProcessor *processors[] = { &timeEntry, new NeoClock(current, wheel) };

#define MODE_SWITCH_PIN 10
int mode = CLOCK_MODE; // BACKLIGHT_MODE;
char s[200];

DateTime compileTime;

// EEPROM indexes
const int MODE_INDEX = 0;
const int WAKE_INDEX = (MODE_INDEX+sizeof(mode));
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
  if ( mode < 0 || mode >= sizeof(modes) / sizeof(char*))
    mode = 0;
  Serial.print(F("Got mode from eeprom ") );
  Serial.println(modes[mode]);

  rtcSetup();

  wheel.Initialize();

  int newIndex = MODE_INDEX+sizeof(mode);
  for ( int i = 0; i < sizeof(processors)/sizeof(IProcessor*); i++ )
  {
    Serial.print("initalizing ");
    Serial.println(i);
    newIndex = processors[i]->Initialize(newIndex);
  }

}
bool rtcRunning = false;
bool pressed = false;
bool forceChange = false;

DateTime getCurrent()
{
  if ( rtcRunning )
    return rtc.now();
  else
    return compileTime.unixtime() + millis() / 50; // 1000 for close to real time
}

/*******************************************************************************
*******************************************************************************/
void loop() {

  current = getCurrent();
  bool next = false;

  switch ( mode )
  {
    case BACKLIGHT_MODE:
      backlight(forceChange);
      break;
    case CLOCK_MODE:
      processors[1]->Process(forceChange); 
      break;
    case FADING_BACKLIGHT_MODE:
      fadingBacklight(current, forceChange);
      break;
    case ALARM_ENTER:
      if ( processors[0]->Process(forceChange)) 
        next = true;
      break;
    case NIGHTLIGHT_MODE:
      nightlight(current, forceChange);
      break;
  }
  forceChange = false;

  if ( digitalRead(MODE_SWITCH_PIN) == LOW  || next )
  {
    if ( !pressed || next )
    {
      next = false;
      pressed = true;
      mode++;
      if ( mode >= sizeof(modes) / sizeof(char*) )
        mode = BACKLIGHT_MODE;

      forceChange = true;
      printTime(current);
      Serial.print(F(" Switching mode to "));
      Serial.println(modes[mode]);
      EEPROM.put(MODE_INDEX, mode);

      wheel.setAllPixels();
      wheel.show();
    }
  }
  else
    pressed = false;
}

int prevHours = -1;
int nextWake = 0;
unsigned long lastWakeChange = 0;

/*******************************************************************************
*******************************************************************************/
void nightlight(DateTime current, bool forceChange)
{
  // how many minutes until wakey?
  DateTime wakeTime = timeEntry.WakeTime();
  double currentMin = 60.0 * current.hour() + current.minute();
  double wakeyMin = 60.0 * wakeTime.hour() + wakeTime.minute();

  bool show = forceChange || wheel.checkColorChange();

  double diffMin = wakeyMin - currentMin;
  if ( diffMin <= 0 )
  {
    // for 15 minutes, animate
    if ( diffMin > -15 )
    {
      if ( lastWakeChange == 0 || millis() - lastWakeChange > 100 )
      {
        lastWakeChange = millis();
        for ( int i = 0; i < NEOPIXEL_LEDS; i++ )
          wheel.setPixelColor(i, wheel.Wheel(wheel.ColorValue));
        wheel.setPixelColor(nextWake++ % NEOPIXEL_LEDS, 0);
        wheel.show();
        prevHours = -1;
      }
      DEBUG_PRINT("Wake UP! since diff min is ");
      DEBUG_PRINTLN(diffMin);
      return;
    }

    diffMin = wakeyMin + (24.0 * 60) - currentMin; // remains of today + until wakey
  }
  lastWakeChange = 0;

  int newHours = 1.5 + (diffMin / 60); // last light 30 minutes


  if ( newHours > 12 )
  {
    wheel.setAllPixels();
    for ( int i = 0; i < NEOPIXEL_LEDS; i += 4 )
      wheel.setPixelColor(i, wheel.Wheel(wheel.ColorValue));

    prevHours = newHours;
    show = true;
  }
  else if ( prevHours != newHours || show )
  {
    wheel.setAllPixels();
    show = true;
    for ( int i = 0; i < abs(newHours); i++ )
      wheel.setPixelColor(i, wheel.Wheel(wheel.ColorValue));
    DEBUG_PRINT( F("PrevHours "));
    DEBUG_PRINT( prevHours );
    DEBUG_PRINT( F(" newHours "));
    DEBUG_PRINTLN( newHours );
    prevHours = newHours;
  }

#ifdef DEBUG
  if ( millis() % 1000 == 0 || show )
  {
    DEBUG_PRINTTIME printTime(current);
    DEBUG_PRINT( F(" (WakeyMin - currentMin) =  ("));
    DEBUG_PRINT( (int)wakeyMin );
    DEBUG_PRINT( F(" - "));
    DEBUG_PRINT( (int)currentMin );
    DEBUG_PRINT( F(") and diff is "));
    DEBUG_PRINT( (int)diffMin );
    DEBUG_PRINT( F("min and show is "));
    DEBUG_PRINT( show );
    DEBUG_PRINT( F(" and forceChange is "));
    DEBUG_PRINTLN( forceChange );
  }
#endif

  if ( show )
    wheel.show();
}

DateTime lastChange(2000, 1, 1);
byte fadeColorValue = 0;

/*******************************************************************************
   color changing backlight
*******************************************************************************/
void fadingBacklight(DateTime &current, bool forceChange )
{
  TimeSpan ts = current - lastChange;

  if ( forceChange || ts.totalseconds() > 1 )
  {
    wheel.checkColorChange(); // for brightness only
    fadeColorValue++;
    if ( fadeColorValue > 255 )
      fadeColorValue = 0;
    wheel.colorWipe(wheel.Wheel(fadeColorValue));
    lastChange = current;
  }
}

/*******************************************************************************
*******************************************************************************/
void backlight(bool forceChange )
{
  if ( forceChange || wheel.checkColorChange() )
  {
    wheel.colorWipe(wheel.Wheel(wheel.ColorValue));
  }
}

/*******************************************************************************
*******************************************************************************/
void rtcSetup() {

  if (!rtc.begin()) {
    while (1)
    {
      Serial.println(F("Couldn't find RTC"));
      delay(1000);
    }
  }
  rtcRunning = true;

  if (!rtc.isrunning()) {
    Serial.println(F("RTC is NOT running!"));
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(compileTime);
  }
}


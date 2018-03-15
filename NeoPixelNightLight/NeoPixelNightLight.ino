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
int inDst = 0;

DateTime compileTime;

const int MODE_INDEX = 0;           // EEPROM index of mode (int)

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

// helper to get time for testing if no RTC available
DateTime getCurrent()
{
  DateTime ret;
  if ( haveRtc )
    ret = rtc.now();
  else
    ret = compileTime.unixtime() + millis() / 50; // speed up time! use 1000 for close to real time

  // need to spring forward? 2am, second Sunday in March
  if ( ret.month() == 3 && ret.dayOfTheWeek() == 0 && ret.hour() == 2 && (ret.day() / 7) == 1 && inDst == 0)
  {
    inDst = 1;
    ret = ret + TimeSpan(0,1,0,0);
    rtc.adjust(ret);
  }
  else if ( ret.month() == 11 && ret.dayOfTheWeek() == 0 && ret.hour() == 2 && (ret.day() / 7) == 0 && inDst == 1 )
  {
    // need to fall back first Sunday in November 2am back to 1am
    inDst = 0;
    ret = ret - TimeSpan(0,1,0,0);
    rtc.adjust(ret);
  }
  
  return ret;
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

// from https://hackaday.com/2012/07/16/automatic-daylight-savings-time-compensation-for-your-clock-projects/
/*--------------------------------------------------------------------------
  FUNC: 6/11/11 - Returns day of week for any given date
  PARAMS: year, month, date
  RETURNS: day of week (0-7 is Sun-Sat)
  NOTES: Sakamoto's Algorithm
    http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week#Sakamoto.27s_algorithm
    Altered to use char when possible to save microcontroller ram
--------------------------------------------------------------------------*/
char dow(int y, char m, char d)
   {
       static char t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
       y -= m < 3;
       return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
   }
 
/*--------------------------------------------------------------------------
  FUNC: 6/11/11 - Returns the date for Nth day of month. For instance,
    it will return the numeric date for the 2nd Sunday of April
  PARAMS: year, month, day of week, Nth occurence of that day in that month
  RETURNS: date
  NOTES: There is no error checking for invalid inputs.
--------------------------------------------------------------------------*/
DateTime NthDayInMonth(int year, char month, char DOW, char NthWeek, int hour, int min ){
  char targetDate = 1;
  char firstDOW = dow(year,month,targetDate);
  while (firstDOW != DOW){
    firstDOW = (firstDOW+1)%7;
    targetDate++;
  }
  //Adjust for weeks
  targetDate += (NthWeek-1)*7;
  return DateTime ( year, month, targetDate, hour, min );
}
 
void setDst( DateTime current )
{
  // a bit imperfect if in the Fall back hour, but this only called when initialized, so don't flash it at those times.
  
  DateTime startDst = NthDayInMonth( current.year(), 3, 0, 2, 2, 0 ); // 2am second sunday in March
  DateTime endDst = NthDayInMonth( current.year(), 11, 0, 1, 2, 0 ); // 2am first sunday in November
  inDst = 1;
  if ( current.secondstime() < startDst.secondstime() || 
       current.secondstime() > endDst.secondstime() )
       inDst = 0;
  Serial.print("Setting DST to ");
  Serial.println(inDst);
  char buffer[100];
  sprintf(buffer, "Current  is %d/%d/%d %d:%d", (int)current.month(), (int)current.day(), (int)current.year(), (int)current.hour(), (int)current.minute() );
  Serial.println(buffer);
  sprintf(buffer, "StartDst is %d/%d/%d %d:%d", (int)startDst.month(), (int)startDst.day(), (int)startDst.year(), (int)startDst.hour(), (int)startDst.minute() );
  Serial.println(buffer);
  sprintf(buffer, "EndDst   is %d/%d/%d %d:%d", (int)endDst.month(), (int)endDst.day(), (int)endDst.year(), (int)endDst.hour(), (int)endDst.minute() );
  Serial.println(buffer);

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
    setDst(compileTime);
  
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


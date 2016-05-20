#include "TimeEntry.h"
#include <EEPROM.h>
        
int TimeEntry::Initialize( int eepromOffset )
{
    _wakeEepromIndex = eepromOffset;
    
    int32_t wakeUnixTime;
    EEPROM.get(_wakeEepromIndex,wakeUnixTime);
    
    Serial.print(F("Got wakeTime from eeprom ") );
    Serial.println(wakeUnixTime);
    
    if ( wakeUnixTime >= 0 )
        _wakeTime = DateTime(wakeUnixTime);
    
    return eepromOffset + sizeof(int32_t);
}

    
bool  TimeEntry::Process( bool changingModes )
{
  if ( !_prompted || changingModes)
  {
    clearPixels();
    _strip.show();
    
    Serial.print(F("Current wake time is "));
    printTime(_wakeTime);
    Serial.println("");
    Serial.println(F("Enter time to wake in 24 hour clock HH,MM"));
    
    _prompted = true;
    
  }
  
  if ( millis() % 100 == 0 )
  {
      _strip.setPixelColor(_currentPixel,0);
      _strip.setPixelColor(_strip.numPixels()-_currentPixel++,0);
      if ( _currentPixel >= _strip.numPixels() )
        _currentPixel = 0;
      _strip.setPixelColor(_currentPixel,_strip.ColorValue);
      _strip.setPixelColor(_strip.numPixels()-_currentPixel,_strip.ColorValue);
      _strip.show();
      delay(5);
  }
  
  // if there's any serial available, read it:
  if (Serial.available() > 0) {
    
    // look for the next valid integer in the incoming serial stream:
    int hours = Serial.parseInt();
    // do it again:
    int minutes = Serial.parseInt();
    while (Serial.available() > 0) {
      Serial.read(); // pull off rest
    }
    if ( hours >= 0 && hours < 24 && minutes >=0 && minutes < 60 )
    {
      _wakeTime = DateTime( 2016,1,1, // ignore y,m,d
                              hours,minutes); 
      printTime(_wakeTime);
      Serial.println(F(" is new waketime"));     
      EEPROM.put(_wakeEepromIndex,_wakeTime.unixtime());
    }
    else
      Serial.println(F("Invalid time.  Must be 0-23 for hour and 0-59 for minute"));
      
    _prompted = false;
      
    return true;
  }
  return false;
}    
#include "TimeEntry.h"
#include <EEPROM.h>
        
int TimeEntry::initialize( int eepromOffset )
{
    _wakeEepromIndex = eepromOffset;
    
    int32_t wakeUnixTime;
    EEPROM.get(_wakeEepromIndex,wakeUnixTime);
    
    Serial.print(F("Got wakeTime from eeprom of ") );
    Serial.print(wakeUnixTime);
    
    if ( wakeUnixTime >= 0 )
        _wakeTime = DateTime(wakeUnixTime);
        
    Serial.print(F(" which is ") );
    printTime(_wakeTime);
    Serial.println("");
    
    return eepromOffset + sizeof(int32_t);
}

    
bool  TimeEntry::process( bool changingModes )
{
  if ( !_prompted || changingModes)
  {
    _wheel.colorWipe();
    
    Serial.print(F("Current wake time is "));
    printTime(_wakeTime);
    Serial.println("");
    Serial.println(F("Enter time to wake in 24 hour clock HH,MM"));
    
    _prompted = true;
    
  }
  
  if ( millis() - _lastAnimation > 100 )
  {
      _lastAnimation = millis();
      
      _wheel.setPixelColor(_currentPixel,0);
      _wheel.setPixelColor(_wheel.numPixels()-_currentPixel,0);
      _currentPixel += 1;
      if ( _currentPixel >= _wheel.numPixels() )
        _currentPixel = 0;
        
      _wheel.setPixelColor(_currentPixel,_wheel.colorWheel(_wheel.colorIndexValue));
      _wheel.setPixelColor(_wheel.numPixels()-_currentPixel,_wheel.colorWheel(_wheel.colorIndexValue));
      
      _wheel.show();
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
      Serial.print(F(" is new waketime ("));     
      Serial.print(_wakeTime.unixtime());     
      Serial.println(F(")"));     
      EEPROM.put(_wakeEepromIndex,_wakeTime.unixtime());
    }
    else
      Serial.println(F("Invalid time.  Must be 0-23 for hour and 0-59 for minute"));
      
    _prompted = false;
      
    return true;
  }
  return false;
}    

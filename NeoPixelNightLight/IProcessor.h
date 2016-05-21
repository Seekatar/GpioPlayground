#ifndef __IPROCESSOR_H__
#define __IPROCESSOR_H__
#include "jsDebug.h"

#include <RTCLib.h>
#include "NeoPixelWheel.h"

class IProcessor
{
protected:
    DateTime &_currentTime;
    NeoPixelWheel &_wheel;
       
    IProcessor(DateTime &currentTime,  NeoPixelWheel &wheel) : _currentTime(currentTime),
                                                                  _wheel(wheel)
                                                                  {}
                                                                  
    inline void printTime( DateTime &dt )
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

                                                                              
public:
    // return next eepromOffset (add your size to eepromOffset)
    inline virtual int Initialize( int eepromOffset )
    {
        return 0;
    }
    
    // return true to move to next mode
    inline virtual bool Process( bool changingModes )
    {
        return false;
    }
};


#endif

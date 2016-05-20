#ifndef __IPROCESSOR_H__
#define __IPROCESSOR_H__

#include <RTCLib.h>
#include "NeoPixelWheel.h"

class IProcessor
{
protected:
    DateTime &_currentTime;
    NeoPixelWheel &_strip;
       
    IProcessor(DateTime &currentTime,  NeoPixelWheel &strip) : _currentTime(currentTime),
                                                                  _strip(strip)
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
    inline virtual int Initialize( int eepromOffset )
    {
        return 0;
    }
    
    inline virtual bool Process( bool changingModes )
    {
        return true;
    }
};


#endif

#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__
#include "jsDebug.h"

#include <RTCLib.h>
#include "NeoPixelWheel.h"

/// <summary>
/// Base class for all Processors
/// </summary>
class Processor
{
protected:
    DateTime &_currentTime;
    NeoPixelWheel &_wheel;
    const char * _name;
       
    Processor(const char *name, DateTime &currentTime,  NeoPixelWheel &wheel) : _currentTime(currentTime),
                                                                  _wheel(wheel), _name(name)
                                                                  {}
                                                                  
public:
    inline static void printDateTime(DateTime &dt, bool dow = false)
    {
        printDate(dt,dow);
        Serial.print(" ");
        printTime(dt);
    }

    /// <summary>
    /// Prints the time to serial
    /// </summary>
    /// <param name="dt">The dt.</param>
    inline static void printDate(DateTime &dt, bool dow = false)
    {
        Serial.print( dt.month() );
        Serial.print( "/" );
        Serial.print( dt.day() );
        Serial.print( "/" );
        Serial.print(dt.year());
        if ( dow )
        {
            Serial.print(" dow: ");
            Serial.print(dt.dayOfTheWeek());
        }
    }

    /// <summary>
    /// Prints the time to serial
    /// </summary>
    /// <param name="dt">The dt.</param>
    inline static void printTime(DateTime &dt)
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

                                                                              
    /// <summary>
    /// Gets the name of this instance.
    /// </summary>
    /// <returns></returns>
    inline const char *name() const { return _name; }
    
    /// <summary>
    /// Initializes this processor, called once 
    /// </summary>
    /// <param name="eepromOffset">The eeprom offset.</param>
    /// <returns>return next eepromOffset (add your size to eepromOffset)</returns>
    inline virtual int initialize(int eepromOffset)
    {
        return eepromOffset;
    }
    
    /// <summary>
    /// Called each time in processing loop, when this Processor selected
    /// </summary>
    /// <param name="changingModes">true if changing from a different processor, false if just looping on this one</param>
    /// <returns>true to move to next Processor</returns>
    inline virtual bool process(bool changingModes)
    {
        return false;
    }
};


#endif

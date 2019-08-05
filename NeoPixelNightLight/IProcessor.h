#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__
#include "jsDebug.h"

#include "NeoPixelWheel.h"
#include <ezTime.h>

/// <summary>
/// Base class for all Processors
/// </summary>
class Processor
{
protected:
    time_t &_currentTime;
    NeoPixelWheel &_wheel;
    const char * _name;

    Processor(const char *name, time_t &currentTime,  NeoPixelWheel &wheel) : _currentTime(currentTime),
                                                                  _wheel(wheel), _name(name)
                                                                  {}

public:
    inline static void printDateTime(time_t &dt, bool dow = false)
    {
        printDate(dt,dow);
        Serial.print(" ");
        printTime(dt);
    }

    /// <summary>
    /// Prints the time to serial
    /// </summary>
    /// <param name="dt">The dt.</param>
    inline static void printDate(time_t &dt, bool dow = false)
    {
        Serial.print( month(dt) );
        Serial.print( "/" );
        Serial.print( day(dt) );
        Serial.print( "/" );
        Serial.print(year(dt));
        if ( dow )
        {
            Serial.print(" dow: ");
            Serial.print(weekday(dt));
        }
    }

    /// <summary>
    /// Prints the time to serial
    /// </summary>
    /// <param name="dt">The dt.</param>
    inline static void printTime(time_t &dt)
    {
        Serial.print( hour(dt) );
        int minutes = minute(dt);
        int seconds = second(dt);
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

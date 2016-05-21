#ifndef __TIMEENTRY_H__
#define __TIMEENTRY_H__

#include "IProcessor.h"

/// <summary>
/// Processor for getting new alarm time from Serial input
/// </summary>
/// <seealso cref="Processor" />
class TimeEntry : public Processor
{
private:
    bool _prompted = false;
    int _currentPixel = 0;
    int _wakeEepromIndex = 0;
    unsigned long _lastAnimation = 0;
    DateTime _wakeTime;
    
public:
    TimeEntry(DateTime &currentTime,  NeoPixelWheel &wheel ) : 
        Processor("TimeEntry", currentTime, wheel )
        {  }
        
    /// <summary>
    /// Get the wakeup time
    /// </summary>
    /// <returns>time from EEPROM</returns>
    const DateTime &WakeTime() const { return _wakeTime; }
     
     int virtual initialize( int eepromOffset );
    
     bool virtual process( bool changingModes );        
};

#endif
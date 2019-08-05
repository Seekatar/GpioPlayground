#ifndef __NIGHTLIGHT_H__
#define __NIGHTLIGHT_H__

#include "TimeEntry.h"

/// <summary>
/// Nightlight processor for showing lights until wakeup
/// </summary>
/// <seealso cref="Processor" />
class Nightlight : public Processor
{
private:
    TimeEntry &_timeEntry;
    int _prevHours = -1;
    int _nextWake = 0;
    unsigned long _lastWakeChange = 0;
    unsigned long _lastIdleChange = 0;
    int _idlePixel = 0;


public:
    Nightlight(time_t &currentTime,  NeoPixelWheel &wheel, TimeEntry &timeEntry ) :
        Processor("Nightlight", currentTime, wheel ), _timeEntry(timeEntry)
        {  }

     bool virtual process( bool changingModes );
};

#endif
#ifndef __TIMEENTRY_H__
#define __TIMEENTRY_H__

#include "IProcessor.h"

class TimeEntry : public IProcessor
{
private:
    bool _prompted = false;
    int _currentPixel = 0;
    int _wakeEepromIndex = 0;
    unsigned long _lastAnimation = 0;
    DateTime _wakeTime;
    
public:
    TimeEntry(DateTime &currentTime,  NeoPixelWheel &wheel ) : 
        IProcessor(currentTime, wheel )
        {  }
        
     int virtual Initialize( int eepromOffset );
    
     bool virtual Process( bool changingModes );        
};

#endif
#ifndef __NEOCLOCK_H__
#define __NEOCLOCK_H__

#include "IProcessor.h"

/// <summary>
/// Processor showing a clock on the wheel
/// </summary>
/// <seealso cref="Processor" />
class NeoClock : public Processor
{
private:
    int hr, min, sec;
    
    uint32_t _secColor = 0;
    uint32_t _minColor = 0;
    uint32_t _hrColor = 0;

    
public:
    NeoClock(DateTime &currentTime,  NeoPixelWheel &wheel ) : 
        Processor("NeoClock", currentTime, wheel )
        {  }
        
     int virtual initialize( int eepromOffset );
    
     bool virtual process( bool changingModes );        
};

#endif
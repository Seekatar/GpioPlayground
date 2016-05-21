#ifndef __NEOCLOCK_H__
#define __NEOCLOCK_H__

#include "IProcessor.h"

class NeoClock : public IProcessor
{
private:
    int hr, min, sec;
    
    uint32_t _secColor = 0;
    uint32_t _minColor = 0;
    uint32_t _hrColor = 0;

    
public:
    NeoClock(DateTime &currentTime,  NeoPixelWheel &wheel ) : 
        IProcessor(currentTime, wheel )
        {  }
        
     int virtual Initialize( int eepromOffset );
    
     bool virtual Process( bool changingModes );        
};

#endif
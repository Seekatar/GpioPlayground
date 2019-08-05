#ifndef __BACKLIGHT_H__
#define __BACKLIGHT_H__

#include "IProcessor.h"

/// <summary>
/// Processor just setting the wheel to colors on the analog inputs
/// </summary>
/// <seealso cref="Processor" />
class Backlight : public Processor
{
public:
    Backlight(time_t &currentTime,  NeoPixelWheel &wheel ) :
        Processor("Backlight", currentTime, wheel )
        {  }


     inline bool virtual process( bool changingModes )
     {
        if ( changingModes || _wheel.checkColorChange() )
        {
            _wheel.colorWipe(_wheel.colorWheel(_wheel.colorIndexValue));
        }
        return false;
     }
};

#endif

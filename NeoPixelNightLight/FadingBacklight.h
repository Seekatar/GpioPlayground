#ifndef __FADINGBACKLIGHT_H__
#define __FADINGBACKLIGHT_H__

#include "IProcessor.h"

/// <summary>
/// Fading color backlight processor
/// </summary>
/// <seealso cref="Processor" />
class FadingBacklight : public Processor
{
private:
    time_t _lastChange = 0;
    byte _fadeColorValue = 0;

public:
    FadingBacklight(time_t &currentTime,  NeoPixelWheel &_wheel ) :
        Processor("FadingBacklight", currentTime, _wheel )
        { 
        }


     inline bool virtual process( bool changingModes )
     {
        int ts = _currentTime - _lastChange;

        if ( changingModes || ts > 1 )
        {
            _wheel.checkColorChange(); // for brightness only
            _fadeColorValue++;
            if ( _fadeColorValue > 255 )
                _fadeColorValue = 0;
            _wheel.colorWipe(_wheel.colorWheel(_fadeColorValue));
            _lastChange = _currentTime;
        }
        return false;
     }
};

#endif

#include "NeoClock.h"

int NeoClock::initialize( int eepromOffset )
{
    _secColor = _wheel.Color(0, 0, 255);
    _minColor = _wheel.Color(0, 255, 0);
    _hrColor = _wheel.Color(255, 0, 0);

    return eepromOffset;
}

bool NeoClock::process( bool changingModes )
{
  _wheel.setPixelColor(sec, 0);
  _wheel.setPixelColor(min, 0);
  _wheel.setPixelColor(hr, 0);

  sec = second(_currentTime) / 5;
  min = minute(_currentTime) / 5;
  hr = hour(_currentTime) % 12;

  _wheel.setPixelColor(sec, _secColor);
  _wheel.setPixelColor(min, _minColor);
  _wheel.setPixelColor(hr, _hrColor);

  if ( sec == min && sec == hr )
    _wheel.setPixelColor(sec, _secColor | _minColor | _hrColor );
  else if ( sec == min )
    _wheel.setPixelColor(sec, _secColor | _minColor);
  else if ( sec == hr )
    _wheel.setPixelColor(sec, _secColor | _hrColor);
  else if ( min == hr )
    _wheel.setPixelColor(min, _minColor | _hrColor);

  _wheel.checkColorChange();
  _wheel.setBrightness(_wheel.brightnessIndexValue);

  _wheel.show();

#ifdef DEBUG
  DEBUG_PRINTTIME( _currentTime );
  DEBUG_PRINTLN("");
#endif

    return false;
}

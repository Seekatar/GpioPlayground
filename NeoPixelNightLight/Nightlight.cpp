#include "Nightlight.h"
#include "jsDebug.h"

bool  Nightlight::process( bool changingModes )
{
 // how many minutes until wakey?
  time_t wakeTime = _timeEntry.WakeTime();
  double currentMin = 60.0 * hour(_currentTime) + minute(_currentTime);
  double wakeyMin = 60.0 * hour(wakeTime) + minute(wakeTime);

  bool show = changingModes || _wheel.checkColorChange();

  double diffMin = wakeyMin - currentMin;
  if ( diffMin <= 0 )
  {
    // for 15 minutes, animate
    if ( diffMin > -15 )
    {
      if ( _lastWakeChange == 0 || millis() - _lastWakeChange > 100 )
      {
        _lastWakeChange = millis();
        for ( int i = 0; i < _wheel.numPixels(); i++ )
          _wheel.setPixelColor(i, _wheel.colorWheel(_wheel.colorIndexValue));
        _wheel.setPixelColor(_nextWake++ % _wheel.numPixels(), 0);
        _wheel.show();
        _prevHours = -1;
      }
      // DEBUG_PRINT("Wake UP! since diff min is ");
      // DEBUG_PRINTLN(diffMin);
      return false;
    }

    diffMin = wakeyMin + (24.0 * 60) - currentMin; // remains of today + until wakey
  }
  _lastWakeChange = 0;

  int newHours = 1.5 + (diffMin / 60); // last light 30 minutes


  // if over 11 hours before alarm, just show 3 lights
  if ( newHours > 11  )
  {
    if ( millis() - _lastIdleChange > 1000 )
    {
      _lastIdleChange = millis();

      _wheel.setAllPixels();
      for ( int i = _idlePixel; i < _wheel.numPixels(); i += 4 )
        _wheel.setPixelColor(i, _wheel.colorWheel(_wheel.colorIndexValue));
      _idlePixel += 1;
      if ( _idlePixel > 3)
        _idlePixel = 0;

      _prevHours = newHours;
      show = true;

      // Odd, if do this, _wheel.ColorValue varies a bunch
      //DEBUG_PRINT( F(" colorValue "));
      //DEBUG_PRINTLN( _wheel.ColorValue );
    }
  }
  else if ( _prevHours != newHours || show )
  {
    _lastIdleChange = 0;

    _wheel.setAllPixels();
    show = true;
    for ( int i = 0; i < abs(newHours); i++ )
      _wheel.setPixelColor(i, _wheel.colorWheel(_wheel.colorIndexValue));

    _prevHours = newHours;

    // Odd, if do this, _wheel.ColorValue varies a bunch
    //DEBUG_PRINT( F("PrevHours "));
    //DEBUG_PRINT( _prevHours );
    //DEBUG_PRINT( F(" newHours "));
    //DEBUG_PRINT( newHours );
    //DEBUG_PRINT( F(" colorValue "));
    //DEBUG_PRINTLN( _wheel.ColorValue );
  }

#ifdef DEBUGX
  if ( millis() % 1000 == 0 || show )
  {
    DEBUG_PRINTTIME( _currentTime );
    DEBUG_PRINT( F(" (WakeyMin - currentMin) =  ("));
    DEBUG_PRINT( (int)wakeyMin );
    DEBUG_PRINT( F(" - "));
    DEBUG_PRINT( (int)currentMin );
    DEBUG_PRINT( F(") and diff is "));
    DEBUG_PRINT( (int)diffMin );
    DEBUG_PRINT( F("min and show is "));
    DEBUG_PRINT( show );
    DEBUG_PRINT( F(" and forceChange is "));
    DEBUG_PRINTLN( changingModes );
  }
#endif

  if ( show )
    _wheel.show();

  return false;
}


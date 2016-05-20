#ifndef __COLORSETTINGS_H_
#define __COLORSETTINGS_H_

#include <Adafruit_NeoPixel.h>
  
class NeoPixelWheel : public Adafruit_NeoPixel 
{
  int _colorAnalogPin;
  int _brightnessAnalogPin;
  
public:

    NeoPixelWheel(int numLeds, int dataPin, int colorAnalogPin, int brightnessAnalogPin ) : 
            Adafruit_NeoPixel(numLeds, dataPin, NEO_GRB + NEO_KHZ800), _colorAnalogPin(colorAnalogPin),
            _brightnessAnalogPin(brightnessAnalogPin)
   {
     
   } 
   
   void Initialize()
   {
      // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
      #if defined (__AVR_ATtiny85__)
        if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
      #endif
      // End of trinket special code

      begin();
      colorWipe(0);
      show(); // Initialize all pixels to 'off'
       
   }
    
    byte ColorValue = 0;          // value read from the pot
    float BrightnessValue = 1.0;   // value read from the pot

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c = 0, uint8_t wait = 0) {
  for(uint16_t i=0; i<numPixels(); i++) {
    setPixelColor(i, c);
    if ( wait > 0 )
      show();
    delay(wait);
  }
  if ( wait <= 0 )
    show();
}

bool checkColorChange()
{
  // read the analog in value:
  byte newSensorValue = map(analogRead(_colorAnalogPin), 0, 1023, 0, 255);
  bool updateNeo = false;
  if ( newSensorValue != ColorValue )
  {
      ColorValue = newSensorValue;
      updateNeo = true;
  }

  float newSensorBrightness = map(analogRead(_brightnessAnalogPin), 0, 1023, 0, 255);
  if (  BrightnessValue != newSensorBrightness )
  {
      updateNeo = true;
      BrightnessValue = newSensorBrightness;
      setBrightness(BrightnessValue);
  }
  
  return updateNeo;  
}
};

#endif;
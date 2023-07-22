#ifndef __COLORSETTINGS_H_
#define __COLORSETTINGS_H_

#include <Adafruit_NeoPixel.h>

/// <summary>
/// An Adafruit_NeoPixel-derived class for handling a wheel
/// </summary>
/// <seealso cref="Adafruit_NeoPixel" />
class NeoPixelWheel : public Adafruit_NeoPixel
{
  int _colorAnalogPin;
  int _brightnessAnalogPin;

public:
  NeoPixelWheel(int numLeds, int dataPin, int colorAnalogPin, int brightnessAnalogPin) : Adafruit_NeoPixel(numLeds, dataPin, NEO_GRB + NEO_KHZ800), _colorAnalogPin(colorAnalogPin),
                                                                                         _brightnessAnalogPin(brightnessAnalogPin)
  {
  }

  /// <summary>
  /// Initializes this instance.
  /// </summary>
  void initialize()
  {
    // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined(__AVR_ATtiny85__)
    if (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
#endif
    // End of trinket special code

    begin();
    colorWipe(0);
    checkColorChange();
    show(); // Initialize all pixels to 'off'
  }

  byte colorIndexValue = 0;         // value read from the pot
  float brightnessIndexValue = 1.0; // value read from the pot

  /// <summary>
  /// Gets a color wheel value (Adafruit function)
  /// </summary>
  /// Input a value 0 to 255 to get a color value.
  /// The colours are a transition r - g - b - back to r.
  /// <param name="WheelPos">The wheel position.</param>
  /// <returns></returns>
  uint32_t colorWheel(byte WheelPos)
  {
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
      return Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
      WheelPos -= 85;
      return Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }

  /// <summary>
  /// Sets all pixels to one color, without calling show()
  /// </summary>
  /// <param name="c">The c.</param>
  void setAllPixels(uint32_t c = 0)
  {
    for (uint16_t i = 0; i < numPixels(); i++)
    {
      setPixelColor(i, c);
    }
  }

  /// <summary>
  /// Fill the dots one after the other with a color (Adafruit function)
  /// </summary>
  /// <param name="c">The color.</param>
  /// <param name="wait">The wait delay after each pixel is set.</param>
  void colorWipe(uint32_t c = 0, uint8_t wait = 0)
  {
    for (uint16_t i = 0; i < numPixels(); i++)
    {
      setPixelColor(i, c);
      if (wait > 0)
        show();
      delay(wait);
    }
    if (wait <= 0)
      show();
  }

#define MAX_ANALOG_VALUE 960
#define MIN_ANALOG_VALUE 25

  /// <summary>
  /// Checks the color and brightness change by reading the analog input
  /// </summary>
  /// <returns>true if there was a change</returns>
  bool checkColorChange()
  {
    Serial.print(">>>> Checking color change:");
    bool updateNeo = false;

    // read the analog in value:
    // ESP8266 only has one analog pin
#ifdef have_color_analog_pin
    byte newSensorValue = constrain(map(analogRead(_colorAnalogPin), MIN_ANALOG_VALUE, MAX_ANALOG_VALUE, 0, 255), 0, 255);
    if (newSensorValue != colorIndexValue)
    {
      colorIndexValue = newSensorValue;
      updateNeo = true;
    }
#else
    colorIndexValue = 155;
#endif

    float newSensorBrightness = constrain(map(analogRead(_brightnessAnalogPin), MIN_ANALOG_VALUE, MAX_ANALOG_VALUE, 0, 255), 0, 255);
    if (brightnessIndexValue != newSensorBrightness)
    {
      updateNeo = true;
      brightnessIndexValue = newSensorBrightness;
      setBrightness(brightnessIndexValue);
    }

    if (updateNeo)
    {
      Serial.print("Updating neo with color/brightness: ");
      Serial.print(newSensorValue);
      Serial.print("/");
      Serial.println(brightnessIndexValue);
    }
    return updateNeo;
  }
};

#endif;

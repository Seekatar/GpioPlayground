// RTC
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

RTC_DS1307 rtc;
// end RTC

// neopixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_DATA_PIN 6
#define NEOPIXEL_LEDS 12

#define ANALOG_COLOR_PIN A0  
#define ANALOG_BRIGHTNESS_PIN A1  
#define SWIPE_WAIT 50

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_LEDS, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

byte sensorValue = 0;        // value read from the pot
float brightnessValue = 1.0;   // value read from the pot
// end neopixel

// clock settings
int hr = 0;
int min = 0;
int sec = 0;

uint32_t secColor = 0;
uint32_t minColor = 0;
uint32_t hrColor = 0;

// mode settings
#define BACKLIGHT_MODE 0
#define CLOCK_MODE  1
#define NIGHTLIGHT_MODE 2

int mode = BACKLIGHT_MODE;

/*******************************************************************************
*******************************************************************************/
void setup() {
  
  rtcSetup();
  neoPixelSetup();
  
}

/*******************************************************************************
*******************************************************************************/
void loop() {

  Serial.print("in loop with mode of");
  Serial.println(mode);
  switch ( mode )
  {
    case BACKLIGHT_MODE:
      backlight();
      break;
    case CLOCK_MODE:
      clocklight();
      break;
    case NIGHTLIGHT_MODE:
      nightlight();
      break;
  }

}

/*******************************************************************************
*******************************************************************************/
void clocklight() 
{
}

/*******************************************************************************
*******************************************************************************/
void nightlight() 
{

}

/*******************************************************************************
*******************************************************************************/
void backlight() 
{

  // read the analog in value:
  byte newSensorValue = map(analogRead(ANALOG_COLOR_PIN), 0, 1023, 0, 255);
  bool updateNeo = false;
  if ( newSensorValue != sensorValue )
  {
      sensorValue = newSensorValue;
      updateNeo = true;
  }
  float newSensorBrightness = map(analogRead(ANALOG_BRIGHTNESS_PIN), 0, 1023, 0, 255);
  
  if (  brightnessValue != newSensorBrightness )
  {
      updateNeo = true;
      brightnessValue = newSensorBrightness;
      strip.setBrightness(brightnessValue);
  }
  
  if ( updateNeo )
  {
    colorWipe(Wheel(sensorValue),SWIPE_WAIT);
  }

}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
/*******************************************************************************
*******************************************************************************/
void neoPixelSetup()
{
 // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  secColor = strip.Color(0,0,10);  
  minColor = strip.Color(0,10,0);  
  hrColor = strip.Color(10,0,0);  
  
}

/*******************************************************************************
*******************************************************************************/
void rtcSetup() {
#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  Serial.begin(57600);
  if (! rtc.begin()) {
    while (1)
    {
      Serial.println("Couldn't find RTC");
      delay(1000);
    }
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }  
}


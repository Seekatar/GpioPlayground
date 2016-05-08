//#define DEBUG
#include <EEPROM.h>

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
// how fast the wipe is when changing colors
#define SWIPE_WAIT 0

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

byte colorValue = 0;          // value read from the pot
float brightnessValue = 1.0;   // value read from the pot
// end neopixel

// clock settings
DateTime current;

uint32_t secColor = 0;
uint32_t minColor = 0;
uint32_t hrColor = 0;

#ifdef DEBUG
  #define DEBUG_PRINT(n) Serial.print(n);
  #define DEBUG_PRINTLN(n) {Serial.println(n); delay(100); }
#else
  #define DEBUG_PRINT(n) {}
  #define DEBUG_PRINTLN(n) {}
#endif

// mode settings
#define BACKLIGHT_MODE 0
#define CLOCK_MODE  1
#define NIGHTLIGHT_MODE 2

#define MODE_SWITCH_PIN 10
int mode = CLOCK_MODE; // BACKLIGHT_MODE;
char s[200];

DateTime wakeTime = DateTime( 2016,1,1, // ignore y,m,d
                              5,45); // 5:45am

#define MODE_INDEX 0

/*******************************************************************************
*******************************************************************************/
void setup() {

  pinMode(MODE_SWITCH_PIN,INPUT_PULLUP);

  EEPROM.get(MODE_INDEX,mode);

  Serial.print(F("Got mode from eeprom ") );
  Serial.println(mode);
   
  rtcSetup();
  neoPixelSetup();
  
}

bool pressed = false;
bool forceChange = false;
/*******************************************************************************
*******************************************************************************/
void loop() {

  current = rtc.now();
  
  switch ( mode )
  {
    case BACKLIGHT_MODE:
      backlight(forceChange);
      break;
    case CLOCK_MODE:
      clocklight(current);
      break;
    case NIGHTLIGHT_MODE:
      nightlight(current, forceChange);
      break;
  }
  forceChange = false;
  
  if ( digitalRead(MODE_SWITCH_PIN) == LOW )
  {
    if ( !pressed )
    {
      pressed = true;
      mode++;
      if ( mode > NIGHTLIGHT_MODE )
        mode = BACKLIGHT_MODE;

      forceChange = true;
      Serial.print(F("Switching mode to "));
      Serial.println(mode);
      EEPROM.put(MODE_INDEX,mode);

      clearPixels();
    }
  }
  else
    pressed = false;
}

int sec = 0;
int min = 0;
int hr = 0;

void clearPixels()
{
  for ( int i = 0; i < NEOPIXEL_LEDS; i++ )
    strip.setPixelColor(i,0);
}
/*******************************************************************************
*******************************************************************************/
void clocklight(DateTime current) 
{
  strip.setPixelColor(sec,0);
  strip.setPixelColor(min,0);
  strip.setPixelColor(hr,0);
  
  sec = current.second() / 5;
  min = current.minute() / 5;
  hr = current.hour() % 12;
  
  strip.setPixelColor(sec,secColor);
  strip.setPixelColor(min,minColor);
  strip.setPixelColor(hr,hrColor);

  if ( sec == min && sec == hr )
    strip.setPixelColor(sec,secColor | minColor | hrColor );
  else if ( sec == min )
    strip.setPixelColor(sec,secColor | minColor);
  else if ( sec == hr )
    strip.setPixelColor(sec,secColor | hrColor);
  else if ( min == hr )
    strip.setPixelColor(min,minColor | hrColor);
  
  strip.show();

  #ifdef DEBUG
  sprintf( s, "%d:%d:%d - %d %d %d", current.hour(), current.minute(), current.second(), hr,min,sec);
  DEBUG_PRINTLN(s);  
  #endif
}

int prevHours = -1;

/*******************************************************************************
*******************************************************************************/
void nightlight(DateTime current, bool forceChange) 
{
  // how many hours until wakey?
  double currentMin = 60.0*current.hour() + current.minute();
  double wakeyMin = 60.0*wakeTime.hour() + wakeTime.minute();

  double diffMin = wakeyMin - currentMin;
  if ( diffMin < 0 ) 
    diffMin = wakeyMin + (24.0*60)-currentMin; // remains of today + until wakey
  int newHours = (diffMin / 60)+.5;
  if ( newHours > 12 )
    newHours = 11; // so can tell difference between this and backlight
    
  bool show = forceChange || checkColorChange();
  
  if ( prevHours != newHours || show )
  {
    show = true;
    if ( prevHours >= 0 )
      strip.setPixelColor(prevHours,0);
    for ( int i = 0; i < abs(newHours); i++ )
      strip.setPixelColor(i,Wheel(colorValue));
    DEBUG_PRINT( F("PreHours "));
    DEBUG_PRINT( prevHours );
    DEBUG_PRINT( F(" newHours "));
    DEBUG_PRINTLN( newHours );
    prevHours = newHours;
  }

  DEBUG_PRINT( F("Wakey current is "));
  DEBUG_PRINT( currentMin );
  DEBUG_PRINT( F(" and wakey is "));
  DEBUG_PRINT( wakeyMin );
  DEBUG_PRINT( F(" and diffMin is "));
  DEBUG_PRINT( diffMin );
  DEBUG_PRINT( F(" and show is "));
  DEBUG_PRINTLN( show );

  if ( show )
    strip.show();
}

bool checkColorChange()
{
  // read the analog in value:
  byte newSensorValue = map(analogRead(ANALOG_COLOR_PIN), 0, 1023, 0, 255);
  bool updateNeo = false;
  if ( newSensorValue != colorValue )
  {
      colorValue = newSensorValue;
      updateNeo = true;
  }

  float newSensorBrightness = map(analogRead(ANALOG_BRIGHTNESS_PIN), 0, 1023, 0, 255);
  if (  brightnessValue != newSensorBrightness )
  {
      updateNeo = true;
      brightnessValue = newSensorBrightness;
      strip.setBrightness(brightnessValue);
  }
  
  return updateNeo;  
}

/*******************************************************************************
*******************************************************************************/
void backlight(bool forceChange ) 
{
  if ( forceChange || checkColorChange() )
  {
    colorWipe(Wheel(colorValue),SWIPE_WAIT);
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
      Serial.println(F("Couldn't find RTC"));
      delay(1000);
    }
  }

  if (! rtc.isrunning()) {
    Serial.println(F("RTC is NOT running!"));
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }  
}


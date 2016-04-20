#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_DATA_PIN 6
#define NEOPIXEL_LEDS 12
#define ANALOG_COLOR_PIN A0  
#define ANALOG_BRIGHTNESS_PIN A1  
#define SWIPE_WAIT 10

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

int hr = 0;
int min = 0;
int sec = 0;

uint32_t secColor = 0;
uint32_t minColor = 0;
uint32_t hrColor = 0;

void setup() {
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
  
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

char s[40];

void loop() {

  strip.setPixelColor(sec,0);
  sec++;
  if ( sec > 11 )
  {
    strip.setPixelColor(min,0);
    sec = 0;
    min++;
  }
  if ( min > 11 )
  {
    strip.setPixelColor(hr,0);
    min = 0;
    hr++;
  }
  if ( hr > 11 )
  {
    hr = 0;
  }
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
  delay(100);
  
  sprintf( s, "%d %d %d - %x %x %x", hr,min,sec,hrColor,minColor,secColor );
  Serial.println(s);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}




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

int r = 0;         // incoming red serial byte
int g = 0;         // incoming green serial byte
int b = 0;         // incoming blue serial byte
int cmd = 0;
uint32_t prevColor = 0;

#define SHOW_COLOR 1
#define ALERT 2

char s[41];
char ss[40];

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 3) {

    // first byte is command 
    cmd = Serial.read();
    if ( cmd == SHOW_COLOR )
    {
      // get incoming r/g/b:
      r = Serial.read();
      g = Serial.read();
      b = Serial.read();
  
      prevColor = strip.Color(r, g, b);
      colorWipe( prevColor ,SWIPE_WAIT );
      
      // echo back that we got it, always 40 bytes
      sprintf( ss, "%03d %03d %03d",r,g,b );
      sprintf( s, "%-39s", ss );
      Serial.println(s);

    }
    else if ( cmd == ALERT )
    {
      for ( int i = 0; i < 10; i++ )
      {
        colorWipe( strip.Color(255,0,0),0);
        delay(800);
        colorWipe( strip.Color(50,0,0),0);
        delay(200);
      }
      colorWipe( prevColor, SWIPE_WAIT );
      
      // echo back that we got it, always 40 bytes
      sprintf( s, "%-39s", "Alerted!" );
      Serial.println(s);
    }
    else
    {
      sprintf( s, "%-39s", "Unknown Arduino command" );
    }
    
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}




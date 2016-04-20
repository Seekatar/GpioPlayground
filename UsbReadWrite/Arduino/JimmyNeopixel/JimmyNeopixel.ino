#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

int cmd,r,g,b = 0;

#define ALERT 2

char ss[41];
char s[41];
int lastAlert = 0;

uint16_t currentRainbowValue = 0;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // 0-255 for bright 0xff
  strip.setBrightness(100);
  
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

// how often is changed color
#define RAINBOW_DELAY_MS 5000

// how many times to blink
#define ALERT_COUNT 5

void loop() {
  
  rainbow(RAINBOW_DELAY_MS);
  
 
  // first byte is command 
  cmd = Serial.read();
  if ( cmd > 0 )
  {
    r = Serial.read();
    g = Serial.read();
    b = Serial.read();
 
    if ( cmd == ALERT )
    {
      int now = millis();
      if ( abs(now - lastAlert) > 5000 )
      {
        lastAlert = now;
        for ( int i = 0; i < ALERT_COUNT; i++ )
        {
          colorWipe( strip.Color(255,0,0),0); // red
          delay(400);
          colorWipe( strip.Color(10,0,0),0); // dark red
          delay(100);
        }
      }
      
      // echo back that we got it, always 40 bytes
      sprintf( ss, "Alerted with cmd of %d %d %d", cmd,now,lastAlert );
      sprintf( s, "%-40s", ss );
      Serial.print(s);
      Serial.flush();
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

void rainbow(uint32_t wait) {
  uint16_t i;

//      sprintf( s, "wait is %u start is %ul", wait, startWait );
//      Serial.println(s);
  
  for(; currentRainbowValue<256; currentRainbowValue++) 
  {
    for(i=0; i<strip.numPixels(); i++) 
    {
      strip.setPixelColor(i, Wheel((i+currentRainbowValue) & 255));
    }
    strip.show();
    
      unsigned long startWait = millis();
//    unsigned long now = millis();
//    unsigned long diff = 0;
    // while (  diff < (unsigned long)wait )
    while (  millis() - startWait < (unsigned long)wait )
    {
//      diff = now - startWait;
      
//      Serial.print( "---- now: "); Serial.print(  now );
//      Serial.print( " start: "); Serial.print( startWait );
//      Serial.print( " diff: " ); Serial.print( diff );
//      Serial.print( " wait: " ); Serial.print( wait);
//      Serial.print( " Is lt: "); Serial.println( diff < (unsigned long)wait );

      if ( Serial.available() > 3 )
        return;
      delay(500);
      
//      now = millis();
    }
//        sprintf( s, "Outer: %u - %u  (%u) < %u", millis(), startWait, (millis() - startWait), wait );
//        Serial.println(s);
  }
  currentRainbowValue = 0;
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


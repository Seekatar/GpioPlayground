/***************************************************************************
  BASED ON Adafruit's BME280 and SSD1306 Samples that were:
  
  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// start OLED 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
// end OLED 


#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

//Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK); // I2C wasn't working for me

// On 32u4 or M0 Feathers, buttons A, B & C connect to 9, 6, 5 respectively
const int  modeButtonPin = 9;    // cycle temp/pressure/humidy
const int  systemButtonPin = 6;  // cycle English/metric
const int  dimPin = 5;           // toggle dim

void setup() {
  // display init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  
  // initialize the button pin as a input:
  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(systemButtonPin, INPUT_PULLUP);
  pinMode(dimPin, INPUT_PULLUP);
  
  delay(2000);

  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  
}

char buffer[100];

#define TEMP 0
#define PRESSURE 1
#define HUMIDITY 2

int currentlyShowing = TEMP;
int prevState = 0;

bool dim = false;
int prevDimState = LOW;

bool english = true;
int prevSystemState = 0;

int pollInterval = 2000; // how often to check/send values
int prevTemp = 0x7fff;
int prevPressure = -1;
int prevHumidity = -1;

char downArrow[3] = " \x19";
char upArrow[3] = " \x18";
char sideArrow[3] = " \x1a";


void loop() {

    float temp = bme.readTemperature() - 3; // mine a few degrees off
    float pressure = bme.readPressure();
    float humidity = bme.readHumidity();
    
    Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(pressure / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("millis() = ");
    Serial.println(millis());
    Serial.println();
    
    unsigned long now = millis();

    while (abs(millis() - now) < pollInterval )
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.setTextSize(1);
      char *direction = sideArrow;
      
      switch ( currentlyShowing )
      {
        case TEMP:
          if ( prevTemp != 0x7fff && prevTemp < (int)temp)
            direction = upArrow;
          else if ( prevTemp != 0x7fff && prevTemp > (int)temp)
            direction = downArrow;

          display.println("Temperature");
          display.println("");
          display.setTextSize(2);
          if ( english )
            sprintf( buffer, " %dF", (int)(temp*9/5+32));
          else
            sprintf( buffer, " %dC", (int)(temp));
          break;
        case PRESSURE:
          if ( prevPressure > 0 && prevPressure < (int)pressure)
            direction = upArrow;
          else if ( prevPressure > 0 && prevPressure > (int)pressure)
            direction = downArrow;

          display.println("Pressure");
          display.println("");
          display.setTextSize(2);
          if ( english )
            sprintf( buffer, " %d.%d\"Hg", (int)(pressure*0.000295299830714F), (int)(pressure*0.00295299830714F) % 10 );
          else
            sprintf( buffer, " %dhPa ", (int)(pressure / 100.0F));
          break;
        case HUMIDITY:
          if ( prevHumidity > 0 && prevHumidity < (int)humidity)
            direction = upArrow;
          else if ( prevHumidity > 0 && prevHumidity > (int)humidity)
            direction = downArrow;

          display.println("Humidity");
          display.println("");
          display.setTextSize(2);
          sprintf( buffer, " %d%%", (int)humidity);
      }

      display.print(buffer);
      display.println(direction);
      display.dim(dim);
      display.display();
    
      // read the pushbutton input pin:
      int buttonState = digitalRead(modeButtonPin);
      if ( buttonState != prevState  )
      {
        if ( buttonState == LOW )
          currentlyShowing = ++currentlyShowing > 2 ? 0 : currentlyShowing;
        prevState = buttonState;
      }
      
      buttonState = digitalRead(systemButtonPin);
      if ( buttonState != prevSystemState  )
      {
        if ( buttonState == LOW )
          english = !english;
        prevSystemState = buttonState;
      }
      
      buttonState = digitalRead(dimPin);
      if ( buttonState != prevDimState  )
      {
        if ( buttonState == LOW )
        {
          dim = !dim;
          Serial.print("Set dim to ");
          Serial.println(dim);
        }   
        prevDimState = buttonState;
      }
    }

    prevTemp = (int)temp;
    prevPressure = (int)pressure/10;
    prevHumidity = (int)humidity;
    
}

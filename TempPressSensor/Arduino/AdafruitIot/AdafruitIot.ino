/***************************************************************************
  BASED ON Adafruit's BME280 and SSD1306 Samples that were:
  
  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

/**************************************************************************/
// start OLED 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// On 32u4 or M0 Feathers, buttons A, B & C connect to 9, 6, 5 respectively
const int  modeButtonPin = 9;    // cycle temp/pressure/humidy
const int  systemButtonPin = 6;  // cycle English/metric
const int  dimPin = 5;           // toggle dim
// end OLED 
/**************************************************************************/
// start BME
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

//Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK); // I2C wasn't working for me
// end BME
/**************************************************************************/
// start WINC1500 (WiFi)
#include <Adafruit_WINC1500.h>
#include <SPI.h>
#include "utility/debug.h"
#include "Adafruit_IO_WINC1500.h"


// Configure WINC1500 pins.
// guide you don't need to modify these:
#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2     // or, tie EN to VCC and comment this out
// The SPI pins of the WINC1500 (SCK, MOSI, MISO) should be
// connected to the hardware SPI port of the Arduino.
// On an Uno or compatible these are SCK = #13, MISO = #12, MOSI = #11.
// On an Arduino Zero use the 6-pin ICSP header, see:
//   https://www.arduino.cc/en/Reference/SPI


// Configure WiFi access point details.
#define WLAN_SSID       "yourSSID"  // can't be longer than 32 characters!
#define WLAN_PASS       "yourpasskey"
//#define WLAN_SECURITY   WLAN_SEC_WPA2  // Can be: WLAN_SEC_UNSEC, WLAN_SEC_WEP, 
                                         //         WLAN_SEC_WPA or WLAN_SEC_WPA2

// Configure Adafruit IO access.
#define AIO_KEY    "youriotkey"


// Setup the WINC1500 connection with the pins above and the default hardware SPI.
Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);

// Create an Adafruit IO Client instance.  Notice that this needs to take a
// WINC1500 object as the first parameter, and as the second parameter a
// default Adafruit IO key to use when accessing feeds (however each feed can
// override this default key value if required, see further below).
Adafruit_IO_WINC1500 aio = Adafruit_IO_WINC1500(WiFi, AIO_KEY ); 

// Finally create instances of Adafruit_IO_Feed objects, one per feed.  Do this
// by calling the getFeed function on the Adafruit_IO_WINC1500 object and passing
// it at least the name of the feed, and optionally a specific AIO key to use
// when accessing the feed (the default is to use the key set on the
// Adafruit_IO_Client class).Adafruit_IO_Feed temperatureFeed = aio.getFeed("M0Temperature");
Adafruit_IO_Feed temperatureFeed = aio.getFeed("M0Temperature");
Adafruit_IO_Feed humidityFeed = aio.getFeed("M0Humidity");
Adafruit_IO_Feed pressureFeed = aio.getFeed("M0Pressure");

#define halt(s) { while(true) { Serial.println(F( s )); delay(1000); }  }

// end WINC1500
/**************************************************************************/

void setup() {
  
  // Setup serial port access.
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  initWifi();
  
  // display init
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false);  // initialize with the I2C addr 0x3C (for the 128x32)
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

  if (!bme.begin()) {
    halt("Could not find a valid BME280 sensor, check wiring!");
  }

}

char buffer[100];

#define TEMP 0
#define PRESSURE 1
#define HUMIDITY 2
#define NETWORK 3
#define LAST_STATE NETWORK

int currentlyShowing = TEMP;
int prevState = 0;

bool dim = false;
int prevDimState = LOW;

// vs metric
bool english = true;
int prevSystemState = 0;

int pollInterval = 2000; // how often to check values
int sendInterval = 10000; // how often to send values
int prevTemp = 0x7fff;
int prevPressure = -1;
int prevHumidity = -1;

int packets = 0;
unsigned long lastPacketSend = 10;

// ASCII arrows to show trends
char downArrow[3] = " \x19";
char upArrow[3] = " \x18";
char sideArrow[3] = " \x1a";

char ipAddress[16] = "\0";

void loop1() {
  float temp = bme.readTemperature() - 3; // mine a few degrees off
  float pressure = bme.readPressure();
  float humidity = bme.readHumidity();
  
  sendToCloud(temp,humidity,pressure);
  delay(5000);
}

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
          break;
          
        case NETWORK:
          direction = "";
          display.print("SSID: ");
          display.println(WLAN_SSID);
          display.print("IP: ");
          display.println(ipAddress);
          display.setTextSize(2);
          sprintf( buffer, " %d sent", (int)packets);
          break;
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
          currentlyShowing = ++currentlyShowing > LAST_STATE ? 0 : currentlyShowing;
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

    sendToCloud(temp,humidity,pressure);
}

void initWifi()
{
#ifdef WINC_EN
  pinMode(WINC_EN, OUTPUT);
  digitalWrite(WINC_EN, HIGH);
  Serial.println(F("Enabled WINC1500!"));
#endif

  delay(2000);
    
  Serial.println(F("Initializing WINC1500!"));

  // Check for compatible firmware.
  if (checkFirmwareVersion() < 0x113){
    halt("Wrong firmware version!");
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    halt("WiFi shield not present");
  }

  // Attempt to connect to an access point.
  Serial.print(F("\nAttempting to connect to ")); 
  Serial.print(WLAN_SSID); Serial.print(F("..."));

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WLAN_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(WLAN_SSID, WLAN_PASS);

    // wait 10 seconds for connection:
    uint8_t timeout = 10;
    while (timeout && (WiFi.status() != WL_CONNECTED)) {
      timeout--;
      delay(1000);
    }
  }  
  Serial.println(F("Connected!"));

  IPAddress ip = WiFi.localIP();
  sprintf(ipAddress,"%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

  // Initialize the Adafruit IO client class (not strictly necessary with the
  // client class, but good practice).
  aio.begin();

  Serial.println(F("Ready!"));
}

void sendToCloud(float temp, float humid, float pressure)
{
  if ( abs(millis() - lastPacketSend) > sendInterval )
  {
    Serial.println( "Sending to cloud!");
    packets++;
    temperatureFeed.send(temp);
    humidityFeed.send(humid);
    pressureFeed.send(pressure);
    lastPacketSend = millis();
  }
}


/**************************************************************************/
/*!
    @brief  Tries to read the WINC1500's internal firmware patch ID
*/
/**************************************************************************/
uint16_t checkFirmwareVersion(void)
{
  int major, minor;
  uint16_t version;
  char *versionString = WiFi.firmwareVersion();
  if(versionString[0] == '-' ) // returns "-Err-" if bad
  {
    Serial.println(F("Unable to retrieve the firmware version!\r\n"));
    version = 0;
  }
  else
  {
    Serial.print(F("Firmware V. : "));
    Serial.println(versionString);
    sscanf(versionString,"%d.%d.", &major, &minor);
    version = (major << 8) | minor;
  }
  return version;
}

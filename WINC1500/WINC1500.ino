// Adafruit IO REST API access with Adafruit winc1500
//
// Designed specifically to work with the Adafruit winc1500:
//  ----> https://www.adafruit.com/products/1469
//
// Adafruit invests time and resources providing this open source code, 
// please support Adafruit and open-source hardware by purchasing 
// products from Adafruit!
//
// Written by Tony DiCola for Adafruit Industries.  
// MIT license, all text above must be included in any redistribution.
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

// put the following defines in your my_keys.h
/*
// Configure WiFi access point details.
#define WLAN_SSID       "yourSSID"  // can't be longer than 32 characters!
#define WLAN_PASS       "yourpasskey"

// Configure Adafruit IO access.
#define AIO_KEY    "yourkey"
*/
#include "my_keys.h"

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
// Adafruit_IO_Client class).
Adafruit_IO_Feed testFeed = aio.getFeed("Welcome%20Feed");

// Alternatively to access a feed with a specific key:
//Adafruit_IO_Feed testFeed = aio.getFeed("cc3ktestfeed", "...cc3ktestfeed key...");

// Global state to increment a number and send it to the feed.
unsigned int count = 0;

#define halt(s) { while(true) { Serial.println(F( s )); delay(1000); }  }

void setup() {

#ifdef WINC_EN
  pinMode(WINC_EN, OUTPUT);
  digitalWrite(WINC_EN, HIGH);
#endif

  // Setup serial port access.
  Serial.begin(9600);


  Serial.println(F("Adafruit IO winc1500 test!"));

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

  // Initialize the Adafruit IO client class (not strictly necessary with the
  // client class, but good practice).
  aio.begin();

  Serial.println(F("Ready!"));
}

void loop() {
  // Increment the count value and write it to the feed.
  count += 1;
  // To write a value just call the feed's send function and pass it the value.
  // Send will create the feed on Adafruit IO if it doesn't already exist and
  // then add the value to it.  Send returns a boolean that's true if it works
  // and false if it fails for some reason.
  if (testFeed.send(count)) {
    Serial.print(F("Wrote value to feed: ")); Serial.println(count, DEC);
  }
  else {
    Serial.println(F("Error writing value to feed!"));
  }

  // Now wait 10 seconds and read the current feed value.
  Serial.println(F("Waiting 10 seconds and then reading the feed value."));
  Serial.flush(); // if no console hangs
  delay(10000);

  // To read the latest feed value call the receive function on the feed.
  // The returned object will be a FeedData instance and you can check if it's
  // valid (i.e. was successfully read) by calling isValid(), and then get the
  // value either as a text value, or converted to an int, float, etc.
  FeedData latest = testFeed.receive();
  if (latest.isValid()) {
    Serial.print(F("Received value from feed: ")); Serial.println(latest);
    // By default the received feed data item has a string value, however you
    // can use the following functions to attempt to convert it to a numeric
    // value like an int or float.  Each function returns a boolean that indicates
    // if the conversion succeeded, and takes as a parameter by reference the
    // output value.
    int i;
    if (latest.intValue(&i)) {
      Serial.print(F("Value as an int: ")); Serial.println(i, DEC);
    }
    // Other functions that you can use include:
    //  latest.uintValue() (unsigned int)
    //  latest.longValue() (long)
    //  latest.ulongValue() (unsigned long)
    //  latest.floatValue() (float)
    //  latest.doubleValue() (double)
  }
  else {
    Serial.print(F("Failed to receive the latest feed value!"));
  }

  // Now wait 10 more seconds and repeat.
  Serial.println(F("Waiting 10 seconds and then writing a new feed value."));
  Serial.flush();
  delay(10000);
  
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


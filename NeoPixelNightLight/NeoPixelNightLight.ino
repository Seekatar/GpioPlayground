#include <EEPROM.h>
#include <ezTime.h>
#include <ESP8266WiFi.h>
#include "my_keys.h"

Timezone currentTz;
#define LOCATION "America/New_York"

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

// neopixel
#include "NeoPixelWheel.h"

#define NEOPIXEL_LEDS 12

#define NEOPIXEL_DATA_PIN 14
#define MODE_SWITCH_PIN 13
#define ANALOG_COLOR_PIN A0
#define ANALOG_BRIGHTNESS_PIN A0

NeoPixelWheel wheel = NeoPixelWheel(NEOPIXEL_LEDS, NEOPIXEL_DATA_PIN, ANALOG_COLOR_PIN, ANALOG_BRIGHTNESS_PIN);

// end neopixel

// current time passed into all processors
time_t current;

// include the processors and create array of them
#include "TimeEntry.h"
#include "NeoClock.h"
#include "Backlight.h"
#include "FadingBacklight.h"
#include "Nightlight.h"

TimeEntry timeEntry(current, wheel);
Processor *processors[] = {new Backlight(current, wheel), new NeoClock(current, wheel), new FadingBacklight(current, wheel), &timeEntry, new Nightlight(current, wheel, timeEntry)};
#define PROCESSOR_COUNT sizeof(processors) / sizeof(Processor *)

int mode = 0;

const int MODE_INDEX = 0; // EEPROM index of mode (int)

/*******************************************************************************
*******************************************************************************/
void setup()
{
#ifndef ESP8266
  while (!Serial)
    ; // for Leonardo/Micro/Zero
#endif

  Serial.begin(115200);

#ifdef ESP8266
  EEPROM.begin(512);
  Serial.println("EEPROM initialized");
#endif

  pinMode(MODE_SWITCH_PIN, INPUT_PULLUP);

  EEPROM.get(MODE_INDEX, mode);

  if (mode < 0 || mode >= PROCESSOR_COUNT)
    mode = 0;
  Serial.print(F("Got mode from eeprom "));
  Serial.println(processors[mode]->name());

  wheel.initialize();

  int newIndex = MODE_INDEX + sizeof(mode);
  for (int i = 0; i < PROCESSOR_COUNT; i++)
  {
    Serial.print("Initializing ");
    Serial.println(processors[i]->name());
    newIndex = processors[i]->initialize(newIndex);
    Serial.println("ok");
  }

  setDebug(INFO);

  Serial.println("Connecting to WiFi " SSID);
  WiFi.begin(SSID, PASSPHRASE);
  Serial.print("Mac address is ");
  Serial.println(WiFi.macAddress());

  waitForSync();
  if (!currentTz.setLocation(LOCATION))
  {
    Serial.println("DIDN'T SET LOCATION to " LOCATION);
  }
  Serial.println("Current time is "+currentTz.dateTime());
}

bool pressed = false;
bool changingModes = false;
bool gotCurrent = false;

/*******************************************************************************
*******************************************************************************/
void loop()
{
  current = currentTz.now();
  bool next = false;

  if (mode < 0 || mode >= PROCESSOR_COUNT)
    mode = 0;

  next = processors[mode]->process(changingModes);

  events(); // update time, if needed

  changingModes = false;

  if (digitalRead(MODE_SWITCH_PIN) == LOW || next)
  {
    if (!pressed || next)
    {
      next = false;
      pressed = true;
      mode++;
      if (mode >= PROCESSOR_COUNT)
        mode = 0;

      EEPROM.put(MODE_INDEX, mode);
#ifdef ESP8266
      EEPROM.commit();
#endif

      changingModes = true;
      Processor::printDateTime(current);
      Serial.print(F(" Switching mode to "));
      Serial.println(processors[mode]->name());

      wheel.colorWipe();
    }
  }
  else
    pressed = false;
}

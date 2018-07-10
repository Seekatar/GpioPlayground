#include "OledDisplay.h"

#define ledPin 9
int pins[3] = {MBED_GPIO_31, MBED_GPIO_27, MBED_GPIO_12};
int pinCount = 3;
void setup() {
  Screen.init();
  Screen.print(0,"PWM Test");
  Screen.print(1,"Output test");
  Screen.print(2,"Initializing...");

  for ( int i = 0; i < pinCount; i++)
    pinMode(i, OUTPUT);
  
  pinMode(USER_BUTTON_A, INPUT);

  Screen.print(2,"Ready...");
}

int pin = pinCount-1;
char buffer[100];
void loop() {

  if ( digitalRead(USER_BUTTON_A) == LOW )
  {
      digitalWrite(pins[pin], LOW);
      if ( ++pin >= pinCount )
        pin = 0;

      snprintf(buffer, 100, "Setting %d to %d%%", pins[pin], 10000/255);
      Screen.print(3,buffer);
      digitalWrite(pins[pin], HIGH);


      delay(500); 
  }
}

void fadeinout() 
{
  // from https://www.arduino.cc/en/Tutorial/Fading
  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(100);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(100);
  }
}

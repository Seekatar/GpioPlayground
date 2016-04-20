#define ledPin 9
void setup() {
  // put your setup code here, to run once:
  
}

void loop() {
  // analogWrite(ledPin,128);
  fadeinout();
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

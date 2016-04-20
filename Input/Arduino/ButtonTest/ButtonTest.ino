// variable holding button state
int buttonState;
// variable holding previous button state
int previousButtonState = LOW;
// variable holding the LED state
int ledState = HIGH;
// LED pin
int ledPin = 7;
// Button pin
int buttonPin = 8;
void setup() {
 // initialize pins
 pinMode(ledPin, OUTPUT);
 pinMode(buttonPin, INPUT);
}
void loop(){
 // read the state of the button
 int reading = digitalRead(buttonPin);

 // if the button is pressed, change the state
 if (reading != buttonState) {
 buttonState = reading;

 // toggle the LED state only when the button is pushed
 if (buttonState == HIGH) {
 ledState = !ledState;
 }
 }

 // turn the LED on or off depending on the state
 digitalWrite(ledPin, ledState);

 // save current reading so that we can compare in the next loop
 previousButtonState = reading;
}

#define buttonPin 10

void setup() {
  // put your setup code here, to run once:

   pinMode(buttonPin, INPUT);
   
  // initialize serial communication:
  Serial.begin(9600);
}

int buttonState = 0;         // current state of the button

void loop() {
  // put your main code here, to run repeatedly:

  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) 
    Serial.println("on");
  else
    Serial.println("off");
  
}

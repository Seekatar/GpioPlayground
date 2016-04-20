#define LED 13
#define SENSOR 12

static int prevValue = 0;

void setup() {
  pinMode(LED,OUTPUT);
  pinMode(SENSOR,INPUT);
}

void loop() {
  // check the sensor and light the LED it set

  int value = digitalRead(SENSOR);
  if ( value != prevValue )
  {
    digitalWrite(LED,value);
    prevValue = value;
  }
}

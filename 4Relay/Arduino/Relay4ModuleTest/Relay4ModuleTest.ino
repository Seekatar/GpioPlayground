// the setup function runs once when you press reset or power the board

/*
 * Turn on and off four relays on a relay board, such as Sunfounders or JBtek
 * 
 * Note that the coil and trigger voltage supply can be different.  There's a 
 * jumper that makes them the same by default.  For Ardunio, this is ok, but 
 * for RPi, it shouldn't get 5V as input, so we're doing that here, too.
 * 
 * Wiring:
 *  Pins 3,4,5,6 are set to IN1-4 on relay board
 *  Gnd -> Gnd on relay board
 *  3.3V -> VCC on relay (to work same as RaspberryPi)
 *  5V -> JD-VCC on relay board after removing jumper
 */
int pins[] = {2,3,4,5};
int nextMode = LOW;

void setup() {
  // initialize digital pins as output and set high (relay off).
  for ( int i = 0; i < sizeof(pins)/sizeof(int); i++ )
  {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i],HIGH);
  }
}

// the loop function runs over and over again forever
void loop() {

  // flip the state of all the pins
  for ( int i = 0; i < sizeof(pins)/sizeof(int); i++ )
  {
    digitalWrite(pins[i],nextMode);
    delay(100);
  }  
  
  if ( nextMode == LOW )
    nextMode = HIGH;
  else 
    nextMode = LOW;
    
  delay(1000);              // wait for a second
}


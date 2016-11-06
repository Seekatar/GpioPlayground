/*
 * Using ultrasonic board to detect if the hatch of the car is up
 * and may hit the garage door, if it opens or closes.
 * 
 * If within the limit, it will break the connection for the relay
 * to disable the garage door, and beep occassionally to indicate
 * that the door is disabled.
 */
// pins for Ultrasonic sensor
#define US_TRIGGER 7
#define US_ECHO 6

// relay
#define RELAY 8

#define BEEPER 3


#define DISTANCE_LIMIT_CM 30

void setup() {

  Serial.begin(57600);

  pinMode( US_TRIGGER, OUTPUT );
  pinMode( US_ECHO, INPUT );
  pinMode( RELAY, OUTPUT );
  pinMode( BEEPER, OUTPUT );

  digitalWrite( US_TRIGGER, LOW );
  delay(500); // let it settle
}

double getAverageDistance( int count, unsigned long sleepMs, bool quiet = true );
double getDistance(bool quiet = true);

bool wasTooClose = false;
unsigned long lastBeep = 0;
int beepFreq = 200;

void loop() {

  double distance = getAverageDistance(10,20,false);  
  if ( distance < DISTANCE_LIMIT_CM  ) 
  {
    if ( !wasTooClose )
    {
      wasTooClose = true;
      digitalWrite( RELAY, HIGH );
      lastBeep = 0;
      Serial.print( "Too close " );
      Serial.println( distance );
    }
    if ( millis() - lastBeep > 2000 )
    {
      lastBeep = millis();
      analogWrite(BEEPER, beepFreq );
      delay(100);
      analogWrite(BEEPER, 0 );
    }
  }
  else if ( distance >= DISTANCE_LIMIT_CM && wasTooClose ) 
  {
    wasTooClose = false;
    digitalWrite( RELAY, LOW );
    analogWrite(BEEPER, 0 );
    Serial.print( "OK " );
    Serial.println( distance );
  }


}

double getAverageDistance( int count, unsigned long sleepMs, bool quiet )
{
  double total = 0;

  for ( int i = 0; i < count; i++ )
  {
    double dist = getDistance();
    total += dist;
    if ( i < count-1 )
      delay(sleepMs);
  }
  return total / count;
}
  

double getDistance(bool quiet)
{
  double distance = 3000;
  while ( distance >= 3000 && distance > 0)  // get occasionally 3000+ reading, toss it
  {
    unsigned long stop = 0;
    unsigned long start = 0;
    unsigned long tempStop = 0;
    unsigned long tempStart = 0;
    
    // Send 10us pulse to trigger
    digitalWrite(US_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(US_TRIGGER,LOW);

    // now wait for echo pulse, checking for timeouts so it doesn't get stuck
    tempStart = start = micros();

    while (digitalRead(US_ECHO) == LOW)
    {
      tempStop = start = micros();
      if (tempStop - tempStart > 500000)
      {
        if (!quiet)
          Serial.println("Timed out in first wait");
        return -1;
      }
    }

    tempStart = micros();
    while (digitalRead(US_ECHO) == HIGH)
    {
      stop = tempStop = micros();
      if (tempStop - tempStart > 500000)
      {
        if (!quiet)
          Serial.println("Timed out in second wait");
        return -1;
      }
    }
    
    // Calculate pulse length
    int elapsed = stop - start;

    // Distance pulse travelled in that time is time
    // multiplied by the speed of sound (cm/us) cut 
    // in half since time is round trip
    distance = elapsed * (.034 / 2);

    if ( distance > 3000 && !quiet )
    {
      Serial.print( "Outlier distance of " );
      Serial.print( distance );
      Serial.print( " with elapsed of " );
      Serial.println( elapsed );
    }
  }
  return distance;
}

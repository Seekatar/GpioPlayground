
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

#define BUTTON_PIN 5
#define GREEN_LED 6
#define RED_LED 9

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

#define SEND_ALERT 0xffffffff
#define CLEAR_ALERT 0xfffffffe
byte addresses[][6] = {"CALLR","RECVR"};

// Used to control whether this node is sending or receiving
void setup() 
{
  Serial.begin(115200);
  Serial.println(F("LED Sender Started"));
  pinMode( BUTTON_PIN, INPUT_PULLUP);
  pinMode( GREEN_LED, OUTPUT );
  pinMode( RED_LED, OUTPUT );
  digitalWrite( RED_LED, HIGH );
  digitalWrite( GREEN_LED, HIGH );
  delay(1000);

  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  
  // Start the radio listening for data
  radio.startListening();

  digitalWrite( RED_LED, LOW );
  digitalWrite( GREEN_LED, LOW );
}

unsigned long pingValue = 0;

long lastSend = 0;
#define SEND_FREQ_MS 1000
bool callSent = false;

void loop()
{
  long now = millis();
  if ( now - lastSend > SEND_FREQ_MS )
  {
    sendPing();
    lastSend = now;
  }
  
  if ( digitalRead( BUTTON_PIN ) == LOW )
  {
    Serial.println("button low");
    if ( sendValue( callSent ? CLEAR_ALERT : SEND_ALERT ) )
    {
      callSent = !callSent;
      digitalWrite( RED_LED, callSent ? HIGH : LOW );
    }
    delay(500);
  }
}

void sendPing() 
{
  if ( sendValue( pingValue ) )
  {
    pingValue++;
    if ( pingValue > 0xf )
      pingValue = 0;
    digitalWrite( GREEN_LED, HIGH );
  }
  else
  {
    digitalWrite( GREEN_LED, LOW );
  }
}

bool sendValue( unsigned long sendValue ) 
{
  /****************** Ping Out Role ***************************/  
    
    radio.stopListening();                                    // First, stop listening so we can talk.
    
    
    Serial.print(F("Now sending "));
    Serial.println(sendValue);
    
    unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
     if (!radio.write( &sendValue, sizeof(unsigned long) )){
       Serial.println(F("failed"));
     }
        
    radio.startListening();                                    // Now, continue listening
    
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while ( ! radio.available() ){                             // While nothing is received
      if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
      }      
    }
        
    if ( timeout )
    {                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
        return false;
    }
    else
    {
        unsigned long got_echo;                                 // Grab the response, compare, and send to debugging spew
        radio.read( &got_echo, sizeof(unsigned long) );
        unsigned long end_time = micros();
        
        // Spew it
        Serial.print(F("Sent at "));
        Serial.print(start_time);
        Serial.print(F(", Got response "));
        Serial.print(got_echo);
        Serial.print(F(", Round-trip delay "));
        Serial.print(end_time-start_time);
        Serial.println(F(" microseconds"));

        return got_echo == sendValue;;
    }
} 


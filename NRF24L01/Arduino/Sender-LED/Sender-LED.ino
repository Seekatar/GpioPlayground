
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);
/**********************************************************/

byte addresses[][6] = {"LEDAK","LEDMG"};

// Used to control whether this node is sending or receiving
void setup() 
{
  Serial.begin(115200);
  Serial.println(F("LED Sender Started"));
  
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  
  // Start the radio listening for data
  radio.startListening();
}

unsigned long sendValue = 0;

void loop() {
  
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
        
    if ( timeout ){                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
    }else{
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
    }

    sendValue++;
    if ( sendValue > 0xf )
      sendValue = 0;
      
    // Try again 1s later
    delay(1000);
} // Loop


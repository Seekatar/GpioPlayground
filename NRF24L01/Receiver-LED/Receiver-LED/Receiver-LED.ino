
/*
 * Based on:
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

#define LED_YELLOW 2
#define LED_GREEN 3
#define LED_RED 4
#define LED_BLUE 5

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);

/**********************************************************/

byte addresses[][6] = {"LEDAK","LEDMG"};

void setup() 
{
  pinMode( LED_YELLOW, OUTPUT );
  pinMode( LED_GREEN, OUTPUT );
  pinMode( LED_RED, OUTPUT );
  pinMode( LED_BLUE, OUTPUT );
  
  Serial.begin(115200);
  Serial.println(F("LED Receiver Started"));
  
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);
  
  // Start the radio listening for data
  radio.startListening();
}

unsigned long sendValue = 0;

void loop() {
  
    unsigned long got_echo;
    
    if( radio.available()){
                                                                    // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &got_echo, sizeof(unsigned long) );             // Get the payload
      }
     
      radio.stopListening();                                        // First, stop listening so we can talk   
      radio.write( &got_echo, sizeof(unsigned long) );              // Send the final one back.      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F("Sent response "));
      Serial.print(got_echo);  
      Serial.print(" ");  
      Serial.print((got_echo & 1) ? HIGH : LOW );  
      Serial.print(" ");  
      Serial.print((got_echo & 2) ? HIGH : LOW );  
      Serial.print(" ");  
      Serial.print((got_echo & 4) ? HIGH : LOW );  
      Serial.print(" ");  
      Serial.println((got_echo & 8) ? HIGH : LOW );  

      digitalWrite( LED_YELLOW, (got_echo & 1) ? HIGH : LOW );
      digitalWrite( LED_GREEN, (got_echo & 2) ? HIGH : LOW );
      digitalWrite( LED_RED, (got_echo & 4) ? HIGH : LOW );
      digitalWrite( LED_BLUE, (got_echo & 8) ? HIGH : LOW );
   }
 
} // Loop


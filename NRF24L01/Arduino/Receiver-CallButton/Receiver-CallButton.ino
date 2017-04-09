
/*
 * Based on:
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

#define LED_RED 9

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7,8);

/**********************************************************/

#define SEND_ALERT 0xffffffff
#define CLEAR_ALERT 0xfffffffe
byte addresses[][6] = {"CALLR","RECVR"};
bool blinking = false;
bool redOn = false;
#define BLINK_FREQ_MS 100
long lastBlink = 0;

void setup() 
{
  pinMode( LED_RED, OUTPUT );
  digitalWrite( LED_RED, HIGH );
  delay(1000);
  digitalWrite( LED_RED, LOW );
  
  Serial.begin(115200);
  Serial.println(F("CallButton Receiver Started"));
  
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

void loop()
{
  getValue();

  if ( blinking )
  {
    long now = millis();
    if ( now - lastBlink > BLINK_FREQ_MS )
    {
      lastBlink = now;
      redOn = !redOn;
      digitalWrite( LED_RED, redOn ? HIGH : LOW );
    }
  }
  else if ( redOn )
  {
    digitalWrite( LED_RED, LOW );
    redOn = false;
  }

}

void getValue() 
{
    unsigned long got_echo;
    
    if( radio.available())
    {
      while (radio.available()) {                                   // While there is data ready
        radio.read( &got_echo, sizeof(unsigned long) );             // Get the payload
      }

      if ( got_echo == SEND_ALERT )
        blinking = true;
      else if ( got_echo == CLEAR_ALERT )
        blinking = false;
        
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
   }
} // Loop


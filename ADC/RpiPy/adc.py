#!/usr/bin/env python

# from https://learn.adafruit.com/reading-a-analog-in-and-controlling-audio-volume-with-the-raspberry-pi?view=all#run-it 
# changed a bit by jmw to do multiple inputs
# note that this does not use teh SPI interface on the chip, but does bit-banging 

# Written by Limor "Ladyada" Fried for Adafruit Industries, (c) 2015
# This code is released into the public domain

import time
import os
import RPi.GPIO as GPIO
import math

GPIO.setmode(GPIO.BCM)
DEBUG = 1

# read SPI data from MCP3008 chip, 8 possible adc's (0 thru 7)
def readadc(adcnum, clockpin, mosipin, misopin, cspin):
        if ((adcnum > 7) or (adcnum < 0)):
                return -1
        GPIO.output(cspin, True)

        GPIO.output(clockpin, False)  # start clock low
        GPIO.output(cspin, False)     # bring CS low

        commandout = adcnum
        commandout |= 0x18  # start bit + single-ended bit
        commandout <<= 3    # we only need to send 5 bits here
        for i in range(5):
                if (commandout & 0x80):
                        GPIO.output(mosipin, True)
                else:
                        GPIO.output(mosipin, False)
                commandout <<= 1
                GPIO.output(clockpin, True)
                GPIO.output(clockpin, False)

        adcout = 0
        # read in one empty bit, one null bit and 10 ADC bits
        for i in range(12):
                GPIO.output(clockpin, True)
                GPIO.output(clockpin, False)
                adcout <<= 1
                if (GPIO.input(misopin)):
                        adcout |= 0x1

        GPIO.output(cspin, True)
        
        adcout >>= 1       # first bit is 'null' so drop it
        return adcout

# change these as desired - they're the pins connected from the
# SPI port on the ADC to the Cobbler
SPICLK = 18
SPIMISO = 23
SPIMOSI = 24
SPICS = 25

GPIO.setwarnings(False)

# set up the SPI interface pins
GPIO.setup(SPIMOSI, GPIO.OUT)
GPIO.setup(SPIMISO, GPIO.IN)
GPIO.setup(SPICLK, GPIO.OUT)
GPIO.setup(SPICS, GPIO.OUT)

trim_adc = 1;
light_adc = 0;
temp_adc = 2;

last_read = 0       # this keeps track of the last potentiometer value
tolerance = 5       # to keep from being jittery we'll only change
                    # volume when the pot has moved more than 5 'counts'

beta = 4090 # the beta of the thermistor, per Arduino sample
try:

    while True:

            # read the analog pins
            trim_value = readadc(trim_adc, SPICLK, SPIMOSI, SPIMISO, SPICS)
            light_value = readadc(light_adc, SPICLK, SPIMOSI, SPIMISO, SPICS)
            temp_value = readadc(temp_adc, SPICLK, SPIMOSI, SPIMISO, SPICS)

            # conversion from Arduino sample
            tempC = beta /(math.log((1025.0 * 10.0 / temp_value - 10) / 10.0) + beta / 298.0) - 273.0
            tempF = 1.8*tempC + 32.0;

            print(light_value,trim_value,temp_value,"%.1f" % tempF)

            # hang out and do nothing for a half second
            time.sleep(0.5)

except KeyboardInterrupt:
    GPIO.cleanup()
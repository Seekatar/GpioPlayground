#! /usr/bin/python3
# From https://learn.adafruit.com/adafruit-bme280-humidity-barometric-pressure-temperature-sensor-breakout/python-circuitpython-test

import board
import busio
import digitalio
import adafruit_bme280
from time import sleep

# I2C wasn't working for me for my board
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
cs = digitalio.DigitalInOut(board.D5)
bme280 = adafruit_bme280.Adafruit_BME280_SPI(spi, cs)

# Pi 3V3 to sensor VIN
# Pi GND to sensor GND
# Pi MOSI to sensor SDI (Master Out Slave In i.e. SPI Data out)
# Pi MISO to sensor SDO (Master In Slave Out i.e. SPI Data in)
# Pi SCLK to sensor SCK (SPI Clock)
# Pi #5 to sensor CS (or use any other free GPIO pin) (Chip Select)

while True:
    print("\nTemperature: %0.1f C %0.1f" % (bme280.temperature, ((bme280.temperature * 9/5) + 32)))
    adjustedTemp = bme280.temperature - 2 # mine a bit off
    print("Adjusted Temperature: %0.1f C %0.1f F" % (adjustedTemp, ((adjustedTemp * 9/5) + 32)))
    print("Humidity: %0.1f %%" % bme280.humidity)
    print("Pressure: %0.1f hPa" % bme280.pressure)
    bme280.sea_level_pressure = 1020
    print("Altitude = %0.2f meters %0.2f feet" % (bme280.altitude, bme280.altitude*3.28084))
    sleep(1)
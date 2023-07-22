# NeoPixel Nightlight Technical Details

Multi-function nightlight that uses a pinhole projector to show the time on the ceiling. The [wiki](https://github.com/Seekatar/GpioPlayground/wiki/Neopixel-Nightlight) has details

The original design used a real-time clock (RTC) module, but time would skew, and daylight savings time was a pain. Now it uses an [Adafruit Huzzah 8266 breakout board](https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout) and NTP to get the time from the internet.

## Programming the Huzzah8266

Using the FTDI, it must be upside down.

1. Press and hold the GPIO0 button
2. Press and release the reset button, a red LED will light dimly
3. Upload the code to the Huzzah8266 (Ctrl+Alt+U in VSCode/Arduino)
4. Press the reset button to start the program

COM port is running at 115200 baud.

# NeoPixel Nightlight Technical Details

> Note that in December 2025 the Huzzah8266 would not connect to a newer ASUS WiFi router, even using an Iot network. It still connects to my older XFinity router.

Multi-function nightlight that uses a pinhole projector to show the time on the ceiling. The [wiki](https://github.com/Seekatar/GpioPlayground/wiki/Neopixel-Nightlight) has details

The original design used a real-time clock (RTC) module, but time would skew, and daylight savings time was a pain. Now it uses an [Adafruit Huzzah 8266 breakout board](https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout) and NTP to get the time from the internet.

## Programming the Huzzah 8266

Using the FTDI, it must be upside down.

1. Press and hold the `GPIO0` button and while still down...
2. Press and release the `Reset` button, a red LED will light dimly
3. Upload the code to the Huzzah8266 (Ctrl+Alt+U in VSCode/Arduino)
4. Press the `Reset` button to start the program

COM port is running at 115200 baud.

## Using Arduino IDE

2.3.7 as of December 2025

Added Ada Fruit URL to board manager to get Huzzah in the list per [Ada Fruit](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide)

`http://arduino.esp8266.com/stable/package_esp8266com_index.json`

On Linux, I got a permission error when on the USB port when trying to upload code or open the serial monitor. To fix that, I added myself to the `dialout` group:

```bash
sudo usermod -a -G dialout $USER
```
To add the C libraries used by this sketch, go to: Sketch->Include Library->Manage Libraries (Ctrl+Shift+l) and add these libraries:

- Adafruit NeoPixel by Adafruit
- ezTime by Paul Stoffregen

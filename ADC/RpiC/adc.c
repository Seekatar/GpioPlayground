/*
gcc -Wall -o adc adc.c -lwiringPi -lm

*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define	TRUE	            (1==1)
#define	FALSE	            (!TRUE)
#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0

static int myFd;

void spiSetup(int spiChannel)
{
	if ((myFd = wiringPiSPISetup(spiChannel, 1000000)) < 0)
	{
		fprintf(stderr, "Can't open the SPI bus: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

int myAnalogRead(int spiChannel, int channelConfig, int analogChannel)
{
	if (analogChannel<0 || analogChannel>7)
		return -1;
	unsigned char buffer[3] = { 1 }; // start bit
	buffer[1] = (channelConfig + analogChannel) << 4;
	wiringPiSPIDataRW(spiChannel, buffer, 3);
	return ((buffer[1] & 3) << 8) + buffer[2]; // get last 10 bits
}

int main(int argc, char *argv[])
{
	int spiChannel = 0; // pi has two SPI channels
	int channelConfig = CHAN_CONFIG_SINGLE;

	wiringPiSetup();
	spiSetup(spiChannel);

	printf("Setup ok on spiChannel %d\n", spiChannel);

	int trim_adc = 1;
	int light_adc = 0;
	int temp_adc = 2;
	int beta = 4090; // the beta of the thermistor, per Arduino sample

	// read the analog pins
	while (TRUE)
	{

		int trim_value = myAnalogRead(spiChannel, channelConfig, trim_adc);
		int light_value = myAnalogRead(spiChannel, channelConfig, light_adc);
		int temp_value = myAnalogRead(spiChannel, channelConfig, temp_adc);

		// conversion from Arduino sample
		float tempC = beta / (log((1025.0 * 10.0 / temp_value - 10) / 10.0) + beta / 298.0) - 273.0;
		float tempF = 1.8*tempC + 32.0;

		printf("%d %d %d %.1f\n", light_value, trim_value, temp_value, tempF);

		usleep(500000);
	}

	close(myFd);
	return 0;
}
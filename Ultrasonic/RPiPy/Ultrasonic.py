import RPi.GPIO as io
import time
import math

trigger = 5
echo = 6

def initialize( triggerGpio, echoGpio ):
	"""
	Initialize the Ultrasonic sensor, passing in
	the trigger and echo pins
	"""
	global trigger
	global echo
	trigger = triggerGpio
	echo = echoGpio

	io.setup( trigger, io.OUT )
	io.setup( echo, io.IN )

	io.output( trigger, io.LOW )
	time.sleep(.5) # let it settle


# get the distance, pulsing the sensor
# <returns>the distance or -1 if timed out waiting for response</returns>
def getDistance(quiet = True):
	
	distance = 3000
	while distance >= 3000: # get occasionally 3000+ reading, toss it
		# Send 10us pulse to trigger
		io.output(trigger, io.HIGH)
		time.sleep(.00001)
		io.output(trigger,io.LOW)

		# now wait for echo pulse, checking for timeouts so it doesn't get stuck
		stop = 0
		tempStop = 0

		tempStart = start = time.time()

		while (io.input(echo) == io.LOW):
			tempStop = start = time.time()
			if (tempStop - tempStart > .500):
				if (not quiet):
					print ("Timed out in first wait")
				return -1

		tempStart = time.time()
		while (io.input(echo) == io.HIGH):
			stop = tempStop = time.time()
			if (tempStop - tempStart > .500):
				if (not quiet):
					print("Timed out in second wait")
				return -1

		# Calculate pulse length
		elapsed = (stop - float(start))

		# Distance pulse travelled in that time is time
		# multiplied by the speed of sound (cm/s) cut 
		# in half since time is round trip
		distance = elapsed * (34000 / 2)

		if ( distance > 3000 and not quiet ):
			print( "Outlier", distance, elapsed )

	return distance

################ MAIN
if __name__ == "__main__":

	import argparse

	parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("--delay", type=float, default=.5, help="Delay between freq changes in seconds (float)")
	parser.add_argument("--delta", type=float, default=0, help="If not zero, prints if delta greater than this")
	args = parser.parse_args()


	io.setwarnings(False)
	io.setmode(io.BCM)

	initialize( trigger, echo )

	try:

		prev = getDistance(False)
		while True:
			dist = getDistance(False)
			if ( args.delta > 0 ):
				if ( abs(dist-prev) > args.delta ):
					print("|%0.2f - %0.2f| = %0.2f > %0.2f" % (dist, prev, abs(dist-prev), args.delta ))
			else:
				print ("%0.2f" % dist)
			prev = dist
			time.sleep(args.delay)
		
	except KeyboardInterrupt:
		pass

	io.cleanup()

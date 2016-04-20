import RPi.GPIO as io
import time
import argparse
import math

# for breathing LED 100+ freq and changing duty cycle (% of time on) to dim
parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--freq", type=int, default=100, help="PWM frequency")
parser.add_argument("--delay", type=float, default=.05, help="Delay between freq changes")
args = parser.parse_args()


io.setwarnings(False)
io.setmode(io.BCM)

trigger = 5
echo = 6
red = 26
green = 19
blue = 13
beeper = 21


io.setup( trigger, io.OUT )
io.setup( beeper, io.OUT )
io.setup( red, io.OUT )
io.setup( green, io.OUT )
io.setup( blue, io.OUT )
io.setup( echo, io.IN )

_averageCount = 3    # how many distance reading to average
_sleepSec = .100

_threshold1 = 25  # moving 25/cm/(.5)sec (approx) turn on green light
_threshold2 = 100 # moving in faster, turn on red light


def getAverageDistance( count, sleepMs ):

	total = 0

	for i in range(count):
		dist = getDistance()
		total += dist 
		print(">",dist)
		if ( i < count-1 ):
			time.sleep(sleepMs)
	return total / count

# get the distance, pulsing the sensor
# <returns>the distance or -1 if timed out waiting for response</returns>
def getDistance():
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
			print ("Timed out in first wait")
			return -1

	tempStart = time.time()
	while (io.input(echo) == io.HIGH):
		stop = tempStop = time.time()
		if (tempStop - tempStart > .500):
			print("Timed out in second wait")
			return -1

	# Calculate pulse length
	elapsed = (stop - float(start))

	# Distance pulse travelled in that time is time
	# multiplied by the speed of sound (cm/s) cut 
	# in half since time is round trip
	distance = elapsed * (34000 / 2)

	return distance

################ MAIN
io.output( trigger, io.LOW )
time.sleep(.5) # let it settle

try:

	prevDistance = getAverageDistance(_averageCount, _sleepSec)
	beeperOn = False

	io.output( blue, io.HIGH )

	while True:
		currentDistance = getAverageDistance(_averageCount, _sleepSec)
		change = abs(currentDistance - prevDistance)
		print ("Change: %0.2fcm" % change)

		if (change < _threshold1):
			io.output(green, io.LOW)
			io.output(red, io.LOW)
			if ( beeperOn ):
				io.output(beeper, io.LOW)
				beeperOn = False
		elif (change > _threshold2):
			io.output(beeper, io.HIGH)
			beeperOn = True
			io.output(green, io.HIGH)
			io.output(red, io.HIGH)
		else:
			io.output(green, io.HIGH)
		prevDistance = currentDistance

		time.sleep(.250)
		
except KeyboardInterrupt:
	pass

io.cleanup()

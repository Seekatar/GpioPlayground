import RPi.GPIO as io
import time
import Ultrasonic 

def getAverageDistance( count, sleepMs ):

	total = 0

	for i in range(count):
		dist = Ultrasonic.getDistance()
		total += dist 
		print(">",dist)
		if ( i < count-1 ):
			time.sleep(sleepMs)
	return total / count


if (__name__ == "__main__" ):
	import argparse

	parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("--closeLimit", type=float, default=3, help="close limit in feet ")
	parser.add_argument("--farLimit", type=float, default=6, help="far limit in feet")
	args = parser.parse_args()


	io.setwarnings(False)
	io.setmode(io.BCM)

	trigger = 5
	echo = 6
	red = 26
	green = 19
	blue = 13
	beeper = 21

	Ultrasonic.initialize(trigger, echo )
	io.setup( beeper, io.OUT )
	io.setup( red, io.OUT )
	io.setup( green, io.OUT )
	io.setup( blue, io.OUT )

	_averageCount = 3    # how many distance reading to average
	_sleepSec = .100


	################ MAIN
	io.output( trigger, io.LOW )
	time.sleep(.5) # let it settle

	try:

		beeperOn = False

		io.output( blue, io.HIGH )

		while True:

			distanceFt = getAverageDistance(_averageCount, _sleepSec) / (2.54*12)

			if (distanceFt > args.farLimit):
				# too far, turn all off
				io.output(green, io.LOW)
				io.output(red, io.LOW)
				if ( beeperOn ):
					io.output(beeper, io.LOW)
					beeperOn = False
			elif (distanceFt < args.closeLimit):
				# getting too close!
				io.output(beeper, io.HIGH)
				beeperOn = True
				io.output(green, io.HIGH)
				io.output(red, io.HIGH)
			else:
				# getting close
				io.output(green, io.HIGH)
				io.output(red, io.LOW)
				if ( beeperOn ):
					io.output(beeper, io.LOW)
					beeperOn = False

			time.sleep(.1)
		
	except KeyboardInterrupt:
		pass

	io.cleanup()

ximport RPi.GPIO as io
import time
import argparse

# for breathing LED 100+ freq and changing duty cycle (% of time on) to dim
parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--freq", type=int, default=100, help="PWM frequency")
parser.add_argument("--delay", type=float, default=.01, help="Delay between freq changes")
args = parser.parse_args()


io.setwarnings(False)
io.setmode(io.BCM)

ledRed = 2
ledGreen = 3
ledBlue = 4

io.setup( ledRed, io.OUT )
io.setup( ledGreen, io.OUT )
io.setup( ledBlue, io.OUT )


pwmRed = io.PWM(ledRed, args.freq )
pwmGreen = io.PWM(ledGreen, args.freq )
pwmBlue = io.PWM(ledBlue, args.freq )

pwmRed.start(0)
pwmGreen.start(0)
pwmBlue.start(0)

# this works, changing to many different colors, but
# the intensity changes quite a bit as the color changes 
# so not real pleasant, but you get the idea of all the colors
# a longer --delay holds each color longer


# note there's supposedly only 1 h/w PWM on the Pi, but
# RPi.GPIO does software PWM

try:

	for j in xrange(10):

		for i in xrange(0,100,5):
			pwmRed.ChangeDutyCycle(i)
			for j in xrange(0,100,5):
				pwmGreen.ChangeDutyCycle(j)
				for k in xrange(0,100,5):
					pwmBlue.ChangeDutyCycle(k)
					time.sleep(args.delay)
				for k in xrange(100,0,-5):
					pwmBlue.ChangeDutyCycle(k)
					time.sleep(args.delay)
			for j in xrange(100,0,-5):
				pwmGreen.ChangeDutyCycle(j)
		
except KeyboardInterrupt:
	pass

pwmRed.stop()
pwmGreen.stop()
pwmBlue.stop()

io.cleanup()

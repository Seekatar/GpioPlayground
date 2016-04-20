import RPi.GPIO as io
import time
import argparse

io.setwarnings(False)
io.setmode(io.BOARD)

ledPin = 37

io.setup( ledPin, io.OUT )


# for breathing LED 100+ freq and changing duty cycle (% of time on) to dim
parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--freq", type=int, default=100, help="PWM frequency")
parser.add_argument("--delay", type=float, default=.01, help="Delay between freq changes")
args = parser.parse_args()

pwm = io.PWM(ledPin, args.freq )

pwm.start(0)
for j in xrange(10):
	for i in xrange(0,100,1):
		pwm.ChangeDutyCycle(i)
		time.sleep(args.delay)
	for i in xrange(100,0,-1):
		pwm.ChangeDutyCycle(i)
		time.sleep(args.delay)
		
		
pwm.stop()

io.cleanup()

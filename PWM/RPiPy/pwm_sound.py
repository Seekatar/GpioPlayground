import RPi.GPIO as io
import time
import argparse

io.setwarnings(False)
io.setmode(io.BOARD)

ledPin = 35

io.setup( ledPin, io.OUT )


# this is like LED, but changes freq instead of duty cycle
# low C 65Hz
# middle C 261Hz
# high C 1046Hz
parser = argparse.ArgumentParser()
parser.add_argument("freq", type=int, default=100)
parser.add_argument("delay", type=float, default=.01)
parser.add_argument("duty",type=int,default=65)
args = parser.parse_args()

pwm = io.PWM(ledPin, args.freq )

pwm.start(65)
pwm.ChangeDutyCycle(args.duty)

for j in xrange(10):
	for i in xrange(65,1046,1):
		print "\r",i,"   "
		pwm.ChangeFrequency(i)
		time.sleep(args.delay)
		
pwm.stop()

io.cleanup()

"""
	Little sample to simulate PWM with software
"""
import RPi.GPIO as io
import time
import argparse

io.setwarnings(False)
io.setmode(io.BOARD)

ledPin = 3

io.setup( 5,io.OUT)
io.setup( 7,io.OUT)
io.setup( ledPin, io.OUT )


# for breathing LED 100+ freq and changing duty cycle (% of time on) to dim
parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--freq", type=float, default=.01, help="PWM frequency")
parser.add_argument("--percent", type=float, default=.5, help="How long on")
args = parser.parse_args()

# pwm = io.PWM(ledPin, args.freq )


# pwm.start(0)

freq = 100
cycle =args.freq # 1/100 # 1 sec / 100

def lightIt(percent):
	io.output(ledPin,io.HIGH)
	sleep = cycle*percent
	time.sleep(sleep)
	io.output(ledPin, io.LOW)
	time.sleep(cycle-sleep)
	
import threading
class thread(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self,None)
		self.percent = .1
		self.originalPercent = self.percent
		self.setDaemon(1)

	def run(self):
		while (self.percent != 0):
			if ( self.percent != self.originalPercent):
				#print("Now doing",self.percent)
				self.originalPercent = self.percent
			lightIt(self.percent)
		print("Exiting")		

		return 


if __name__ == "__main__":
        t = thread()
        t.start()

        try:
            while True:
                    for i in xrange(1,50):
                          t.percent = i/100.0       
                          time.sleep(.025)
                    for i in xrange(50,1,-1):
                          t.percent = i/100.0       
                          time.sleep(.025)
                  
        except KeyboardInterrupt:
                pass
            
        x = raw_input("Enter tenth ")
        while (float(x) != 0 ):
                t.percent = float(x)/10
                x = raw_input("Enter tenth")

        t.percent = 0
        print("Waiting for thread")
        t.join()		

        io.cleanup()

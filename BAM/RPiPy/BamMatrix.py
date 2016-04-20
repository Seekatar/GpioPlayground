import os
import sys
import threading
import time
sys.path.append(os.path.join(os.path.dirname(__file__),'../../../pcg/GpioUtil'))

import RPi.GPIO as io

from ShiftRegister import LedMatrix, ShiftRegister

class thread(threading.Thread):
	"""
		background thread for writing to the 8x8 matrix
	"""

	def __init__(self,lightOn,lightOff,freq,cycle,initialValue=.1):
		threading.Thread.__init__(self,None)
		self.percent = initialValue
		self.lightOn = lightOn
		self.lightOff = lightOff
		self.freq = 100 # per second
		self.cycle = cycle
		self.originalPercent = self.percent
		self.setDaemon(1)

	def run(self):
		while (self.percent != 0):
			if ( self.percent != self.originalPercent):
				self.originalPercent = self.percent
			self.lightIt()
		print("Exiting")		

		return 

	def lightIt(self):
		self.lightOn()
		sleep = self.cycle*self.percent
		time.sleep(sleep)
		self.lightOff()
		time.sleep(self.cycle-sleep)


class multiPlex(threading.Thread):
	def __init__(self,onFns,cycle):
		threading.Thread.__init__(self,None)
		self.onFns = onFns
		self.cycle = cycle

	def run(self):
		sleep = self.cycle/len(self.onFns)
		
		while ( self.onFns != None ):
			for f in self.onFns:
				f()
				time.sleep(sleep)

			
if __name__ == "__main__":

	import argparse
	parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("--dataPin", type=int, default=36, help="pin number for data")
	parser.add_argument("--latchPin", type=int, default=38, help="latch pin number")
	parser.add_argument("--clockPin", type=int, default=40, help="clock pin number")
	parser.add_argument("--delay", type=float, help="delay")
	args = parser.parse_args()

	# setup IO
	io.setwarnings(False)
	io.setmode(io.BOARD)
	io.setup(3,io.OUT)


	leds = LedMatrix(args.dataPin, args.latchPin, args.clockPin)

	y = 1
	x = 1
	def on11():
		leds.set_dot(x,y)
	def on22():
		leds.set_dot(2,2)
	def on33():
		leds.set_dot(4,4)
	def on55():
		leds.set_dot(5,5)

	t = multiPlex((on11,on22,on33,on55),1/100)
	t.start()

	delay = .05
	# animate a rotating dot by chaning x,y of on11
	try:
		while True:
			time.sleep(delay)
			x = 2
			time.sleep(delay)
			x = 3
			time.sleep(delay)
			y = 2
			time.sleep(delay)
			y = 3
			time.sleep(delay)
			x = 2
			time.sleep(delay)
			x = 1
			time.sleep(delay)
			y = 2
			time.sleep(delay)
			y = 1
	except KeyboardInterrupt:
		pass
	
	t.onFns = None
	t.join()
	io.cleanup()
	exit(0)

	def lightOn():
		io.output(3,io.HIGH)

	def lightOff():
		io.output(3,io.LOW)

	t = thread(lightOn,lightOff,100,1/100.0)
	print("Thread cycle", t.cycle)
	t.start()
	try:
		while True:
			for i in range(1,50):
				t.percent = i/100.0
				time.sleep(.025)
			for i in range(50,1,-1):
				t.percent = i/100.0
				time.sleep(.025)

	except KeyboardInterrupt:
		pass
	t.percent = 0
	t.join()


	
	leds.rowOn(1)
	
	print("ok")
	input()

	def rowOn():
		leds.set_dot(3,3)
	def rowOff():
		 leds.off()
		 
	t = thread(rowOn, rowOff,100,1/100.0)
	t.start()
	

	try:
		while True:
			for i in range(1,50):
				t.percent = i/100.0
				time.sleep(.025)
			for i in range(50,1,-1):
				t.percent = i/100.0
				time.sleep(.025)

	except KeyboardInterrupt:
		pass

	t.percent = 0
	t.join()
	
	

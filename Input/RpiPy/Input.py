import os
import time
import sys
sys.path.append(os.path.join(os.path.dirname(__file__),'../../../pcg/GpioUtil'))

import RPi.GPIO as io


if __name__ == "__main__":
	"""
		test all the io pins
	"""
	import argparse
	parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("--up", "-u", action='store_true', help="pud up instead of pud down")
	args = parser.parse_args()

	pins =   3,5,7, 8,10,11,12,13,15,16,18,19,21,22,23,24,26,29,31,32,33,35,36,37,38,40
	iopins = 2,3,4,14,15,17,18,27,22,23,24,10, 9,25,11, 8, 7, 5, 6,12,13,19,16,26,20,21

	print "Pin Readings"
	s = ""

	updown = io.PUD_DOWN # default
	if args.up:
		updown = io.PUD_UP
	print "updown is",updown

	io.setwarnings(False)
	io.setmode(io.BOARD)
	s = "PIN "
	for i in pins:
		io.setup(i,io.IN,updown)
		s += "%3d" % i
	print s

	s = "BCM "
	for i in iopins:
		s += "%3d" % i
	print s

	try:
		while(True):
			s = "    "
			for i in pins:
				if ( io.input(i) == io.HIGH ):
					s += "  1"
				else:
					s += "  0"
			print s+"\r",
			time.sleep( .05 )
	except KeyboardInterrupt:
			pass

io.cleanup()
		

import os
import time
import sys
sys.path.append(os.path.join(os.path.dirname(__file__),'../../../pcg/GpioUtil'))

import RPi.GPIO as io

import colorama
from colorama import Fore, Back, Style
colorama.init()


if __name__ == "__main__":
	"""
		simple test for Motion sensor
	"""
	import argparse
	parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	args = parser.parse_args()

	sensorPin = 11
	io.setwarnings(False)
	io.setmode(io.BOARD)
	io.setup(sensorPin,io.IN)

	try:
		prevValue = io.LOW
		print("Searching...")
		while(True):
			value = io.input(sensorPin)
			if  value != prevValue:
				if value == 1:
						print(Fore.GREEN+"Target acquired.")
				else:
						print(Fore.RED+"Target lost")
				prevValue = value
	except KeyboardInterrupt:
			pass

io.cleanup()
		

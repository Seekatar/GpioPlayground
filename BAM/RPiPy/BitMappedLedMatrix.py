import os
import sys
import threading
import time
sys.path.append(os.path.join(os.path.dirname(__file__),'../../../pcg/GpioUtil'))

import RPi.GPIO as io

from ShiftRegister import ShiftRegister

debug = True

def printDebug( msg ):
	if debug:
		print(msg)

class LedMatrix(ShiftRegister):
	"""
	Bit mapped LED matrix driven by shift registers
	"""

	def __init__(self,dataPin, latchPin, clockPin,width=8):
		"""
		Construct the matrix
			data, latch, clock pins are for ShiftRegister
			width is number of columns, defaulting to 8 (assume 8 high)
		"""
		super(LedMatrix,self).__init__(dataPin, latchPin, clockPin)
		self._width = width
		self._height = 8
		self._bitmap = [0] * width # we'll use 8 bits of the number in the array
		self.cursor_x = -1
		self.cursor_y = -1


	def renderColumn(self,bitMapColumn,ledColumn):
		"""
		Render a specific column in the bitmap to the led column
		"""
		if bitMapColumn >= self._width or ledColumn > 7:
			return
		self.set(~self._bitmap[bitMapColumn],1 << ledColumn)

	def rowOn(self,row):
		"""
		Turn on a row, zero-based
		"""
		# 0 for first turns all leds across on
		# second byte is row bit, set only one row bit to turn on row
		if ( row > 7 ):
			return
		for i in range(len(self._bitmap)):
			self._bitmap[i] = self._bitmap[i] | (1 << row)

	def columnOn(self,column):
		"""
		Turn on a column, zero-based
		"""
		if ( column > 7 ):
			return
		self._bitmap[column] = 0xff

	def off(self):
		"""
		Turn everything off
		"""
		for i in range(len(self._bitmap)):
			self._bitmap[i] = 0

	def clear_dot(self,x,y):
		self.set_dot(x,y,False)

	def toggle_dot(self,x,y):
		if ( x > 7 or y > 7 ):
			return
		self.set_dot(x,y,(self._bitmap[y] & (1 << x)) != 1)

	def set_dot(self,x,y,set=True):
		"""
		Set one dot x,y, zero based
		"""
		if ( x > 7 or y > 7 ):
			return
		# set the bit in the bitmap
		self._bitmap[y] = self._bitmap[y] | (1 << x)

	def test(self):
		self.off()

		x = input('enter a row ')
		while (len(x) > 0 ):
			self.rowOn(int(x))
			x = input('enter a row ')

		self.off();

		x = input('enter a column ')
		while (len(x) > 0 ):
			self.columnOn(int(x))
			x = input('enter a column ')

		self.off();

		x = input('enter numbers x,y ')
		while (len(x) > 0 and x.find(',') >= 0):
			(x,y) = x.split(',')
			self.set_dot(int(x),int(y))
			x = input('enter numbers x,y ')

	def edit(self):
		from GetKey import get

		self.cursor_x = 0
		self.cursor_y = 0

		try:
			while True:
				k = get()
				if k == "up":
					self.cursor_x = self.cursor_x + 1
					if self.cursor_x > 7:
						self.cursor_x = 0
				elif k == "down":
					self.cursor_x = self.cursor_x - 1
					if self.cursor_x < 0:
						self.cursor_x = 7
				elif k == "left":
					self.cursor_y = self.cursor_y - 1
					if self.cursor_y < 0:
						self.cursor_y = 7
				elif k == "right":
					self.cursor_y = self.cursor_y + 1
					if self.cursor_y > 7:
						self.cursor_y = 0
				elif k == ' ':
					self.toggle_dot(self.cursor_x,self.cursor_y)
				elif k == '\r':
					print ('yo!')
		except KeyboardInterrupt:
			return

if __name__ == '__main__':
	import argparse

	parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("--dataPin", type=int, default=36, help="pin number for data")
	parser.add_argument("--latchPin", type=int, default=38, help="latch pin number")
	parser.add_argument("--clockPin", type=int, default=40, help="clock pin number")
	parser.add_argument("--delay", type=float, help="delay")
	args = parser.parse_args()


	print ("Testing....")
	io.setmode(io.BOARD)

	print ("testing matrix")
	m = LedMatrix(args.dataPin, args.latchPin, args.clockPin )



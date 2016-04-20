import os
import sys
import threading
import time
sys.path.append(os.path.join(os.path.dirname(__file__),'../../../pcg/GpioUtil'))

import RPi.GPIO as io

from BitGrid import BitGrid
from ShiftRegister import ShiftRegister

debug = True

def printDebug( *more ):
	if debug:
		print(*more)

# letters copied from http://www.instructables.com/id/48x8-SCROLLING-MATRIX-LED-DISPLAY-USING-ARDUINO-CO/?ALLSTEPS
# then updated for Python, making some letters narrower
# each letter is five or less columns and 7 rows, with one column of 0 for spacing  
#  [1,0], # .

# added numbers, letters, symbols using edit mode
letters = [
  [62,65,73,65,62,0], # 0
  [33,127,1,0], # 1
  [35,69,73,49,0], # 2
  [34,65,73,54,0], # 3
  [120,8,8,127,0], # 4
  [121,73,73,70,0], # 5
  [62,73,73,38,0], # 6
  [65,66,68,120,0], # 7
  [54,73,73,54,0], # 8
  [50,73,73,62,0], # 9
  [0,10,0], # :
  [1,10,0], # ;
  [8,20,34,65,0], # <
  [20,20,20,0], # =
  [65,34,20,8,0], # >
  [32,64,69,72,48,0], # ?
  [30,33,77,93,62,0], # @
  [31, 36, 68, 36, 31,0], # A
  [127, 73, 73, 73, 54,0], # B
  [62, 65, 65, 65, 34,0], # C
  [127, 65, 65, 34, 28,0], # D
  [127, 73, 73, 65, 0], # E
  [127, 72, 72, 64,0], # F
  [62, 65, 65, 69, 38,0], # G
  [127, 8, 8, 8, 127,0], # H
  [65, 127, 65, 0], # I
  [2, 1, 1, 1, 126,0], # J
  [127, 8, 20, 34, 65,0], # K
  [127, 1, 1, 1, 0], # L
  [127, 32, 16, 32, 127,0], # M
  [127, 32, 16, 8, 127,0], # N
  [62, 65, 65, 65, 62,0], # O
  [127, 72, 72, 72, 48,0], # P
  [62, 65, 69, 66, 61,0], # Q
  [127, 72, 76, 74, 49,0],# R
  [50, 73, 73, 73, 38,0], # S
  [64, 64, 127, 64, 64,0], # T
  [126, 1, 1, 1, 126,0], # U
  [124, 2, 1, 2, 124,0], # V
  [126, 1, 6, 1, 126,0], # W 
  [99, 20, 8, 20, 99,0], # X 
  [96, 16, 15, 16, 96,0], # Y
  [67, 69, 73, 81, 97,0], # Z
  [127,65,0], # [
  [64,48,8,6,1,0], # \
  [65,127,0], # ]
  [32,64,32,0], # ^
  [1,1,1,1,0], # _
  [64,32,0], # `
  [2,21,21,15,0], # a
  [63,9,9,6,0], # b
  [6,9,9,0], # c
  [6,9,9,63,0], # d
  [14,21,21,9,0], # e
  [8,63,40,0], # f
  [8,21,21,14,0], # g
  [63,8,8,7,0], # h
  [23,0], # i
  [2,1,1,46,0], # j
  [63,4,11,0], # k
  [63,0], # l
  [15,8,7,8,7,0], # m
  [15,8,7,0], # n
  [6,9,9,6,0], # o
  [31,36,36,24,0], # p
  [24,36,36,31,0], # q
  [15,8,4,0], # r
  [8,21,21,2,0], # s
  [8,31,8,0], # t
  [14,1,1,14,0], # u
  [14,1,14,0], # v
  [14,1,14,1,14,0], # w
  [5,2,5,0], # x
  [26,9,30,0], # y
  [9,11,13,9,0], # z
  [0,0], # space
]
# Cat head [252,66,33,33,33,66,252,0]
Square1 = [255,129,129,129,129,129,129,255]
Square2 = [0,126,66,66,66,66,126,0]
Square3 = [0,0,60,36,36,60,0,0]
Square4 = [0,0,0,24,24,0,0,0]

class LedMatrix(threading.Thread):
	"""
	Class for controling and rendering on an 8xn matrix
	"""

	def __init__(self,shiftRegister,width=8,height=8,refreshRateHz=100,animationSpeed=.1,direction=1,orientation=1):
		"""
		pass in number of times a second to refresh
		"""
		threading.Thread.__init__(self,name="LedMatrix")

		self._run = True

		self.cycle = 1/refreshRateHz
		self.sleep = self.cycle/8
		self.sleep = self.cycle/800
		printDebug("sleep is",self.sleep)

		self._shifRegister = shiftRegister

		self._animationSpeed = animationSpeed

		self._grid = BitGrid(width+8,height) # add 8 for empty
		self._currentColIndex = 0
		self._message = ""
		self._direction = direction
		self._messageLen = 0
		self._cursor_x = -1
		self._cursor_y = -1
		self._cursorOn = False
		self._underCursor = 0 # value under cursor
		self._orientation = orientation

		self._ledWidth = 8
		self._ledHeight = 8

	def set_message(self,message):
		"""
		Set the message on the matrix
		"""

		self._message = message
		self._messageLen = 0
		self._currentColIndex = 0
		printDebug("Len of leds is",len(self._grid.Grid))

		if self._direction == 1:
			self.set_letter([0,0,0,0,0,0,0,0]) # leading empty grid 
		
		for c in self._message:
			n = ord(c)-ord('0')
			if c == ' ':
				self.set_letter(letters[-1])
			elif n >= len(letters) or n < 0:
				raise Exception("Invalid letter in text "+c)
			else:
				self.set_letter(letters[n])
		if self._direction == 2:
			self.set_letter([0,0,0,0,0,0,0,0]) # trailing empty grid 

	def set_letter(self,array):
		for i in array:
			self._grid.Grid[self._currentColIndex] = i
			self._currentColIndex = self._currentColIndex + 1
		self._messageLen += len(array)

	def stop(self):
		"""
		Graceful stop
		"""
		self._run = False

	def _renderColumn(self,bitMapColumn,ledColumn):
		"""
		Render a specific column in the bitmap to the led column
		"""
		if bitMapColumn >= self._grid.Width or ledColumn > 7:
			return

		if self._orientation == 1: # L->R
			self._shifRegister.set(~self._grid.Grid[bitMapColumn],1 << ledColumn)
		elif self._orientation == 2: # 90 deg of 1
			self._shifRegister.set(~(1 << (7-ledColumn)),self._grid.Grid[bitMapColumn])
		elif self._orientation == 3: # 180 deg of 2
			num = (self._grid.Grid[bitMapColumn] & 1) << 7 | \
				(self._grid.Grid[bitMapColumn] & 2) << 5 | \
				(self._grid.Grid[bitMapColumn] & 4) << 3 | \
				(self._grid.Grid[bitMapColumn] & 8) << 1 | \
				(self._grid.Grid[bitMapColumn] & 0x10) >> 1 | \
				(self._grid.Grid[bitMapColumn] & 0x20) >> 3 | \
				(self._grid.Grid[bitMapColumn] & 0x40) >> 5 | \
				(self._grid.Grid[bitMapColumn] & 0x80) >> 7
			self._shifRegister.set(~(1 << ledColumn),num)
		else: # 180 of 1
			num = (self._grid.Grid[bitMapColumn] & 1) << 7 | \
				(self._grid.Grid[bitMapColumn] & 2) << 5 | \
				(self._grid.Grid[bitMapColumn] & 4) << 3 | \
				(self._grid.Grid[bitMapColumn] & 8) << 1 | \
				(self._grid.Grid[bitMapColumn] & 0x10) >> 1 | \
				(self._grid.Grid[bitMapColumn] & 0x20) >> 3 | \
				(self._grid.Grid[bitMapColumn] & 0x40) >> 5 | \
				(self._grid.Grid[bitMapColumn] & 0x80) >> 7
			self._shifRegister.set(~num,1 << (7-ledColumn))


	def edit(self):
		"""
		enter edit mode
		"""
		from GetKey import get

		self._cursor_x = 0
		self._cursor_y = 0

		printDebug("Entering edit mode")

		try:
			while True:
				k = get()
				if k == "up":
					self._cursor_x = self._cursor_x + 1
					if self._cursor_x > 7:
						self._cursor_x = 0
				elif k == "down":
					self._cursor_x = self._cursor_x - 1
					if self._cursor_x < 0:
						self._cursor_x = 7
				elif k == "left":
					self._cursor_y = self._cursor_y - 1
					if self._cursor_y < 0:
						self._cursor_y = 7
				elif k == "right":
					self._cursor_y = self._cursor_y + 1
					if self._cursor_y > 7:
						self._cursor_y = 0
				elif k == ' ':
					if self._underCursor:
						self._underCursor = 0
					else:
						self._underCursor = 1
					self._cursor_y = self._cursor_y + 1
					if self._cursor_y > 7:
						self._cursor_y = 0
				elif k == '\r':
					# turn off cursor
					prevX = self._cursor_x 
					self._cursor_x = -1
					time.sleep(.001)
					self._grid.set_at(prevX,self._cursor_y,self._underCursor)

					s = "  ["
					for i in self._grid.Grid:
						s = s + str(i) + ","
					s = s[0:len(s)-1]
					s = s + "]"
					print(s);

					# turn it on
					self._cursor_x = prevX

				elif k.lower() == 'c':
					self._grid.clear()
					self._underCursor = 0
				printDebug("Moved cursor to",self._cursor_x,self._cursor_y)
		except KeyboardInterrupt:
			return


	def run(self):
		"""
		thread override
		"""

		if self._direction == 1:
			startIndex = 0
			end = self._messageLen
		elif self._direction == 0:
			startIndex = 0
			end = 8 
		else:
			startIndex = self._messageLen-1
			end = -7

		j = startIndex
	
		start = time.time()
		cursorStart = time.time()
		prevCursorX = self._cursor_x 
		prevCursorY = self._cursor_y
		self._underCursor = self._grid.get_at(prevCursorX,prevCursorY)

		while self._run:

			for i in range(8): # led only 8 wide
				self._renderColumn(j+i,i) # bitmapCol, ledCol
				time.sleep(self.sleep)
			now = time.time()

			if self._cursor_x >= 0:
				# set the bit for the cursor, and blink it
				if now - cursorStart > .2:
					self._cursorOn = not self._cursorOn
					self._grid.set_at(prevCursorX,prevCursorY,self._cursorOn)

					if self._cursor_x != prevCursorX or self._cursor_y != prevCursorY:
						self._grid.set_at(prevCursorX ,prevCursorY, self._underCursor)
						prevCursorX = self._cursor_x 
						prevCursorY = self._cursor_y
						self._underCursor = self._grid.get_at(self._cursor_x,self._cursor_y)
					cursorStart = now

			if now - start > self._animationSpeed:
				if self._direction == 0: # not scrolling, but if > 1 letter, show next one
					j = j + self._ledWidth
				elif self._direction == 1:
					j = j + 1
				else:
					j = j - 1
				printDebug("j is",j,"end is",end,"start is",startIndex,"\r")

				if j == end:
					j = startIndex

				start = now

		printDebug("Turning off all leds")
		self._grid.clear()
		for i in range(8): # led only 8 wide
			self._renderColumn(j+i,i) # bitmapCol, ledCol

			
if __name__ == "__main__":

	import argparse
	parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
	parser.add_argument("--dataPin", type=int, default=36, help="pin number for data")
	parser.add_argument("--latchPin", type=int, default=38, help="latch pin number")
	parser.add_argument("--clockPin", type=int, default=40, help="clock pin number")
	parser.add_argument("--delay", "-y", type=float, help="delay", default=.1)
	parser.add_argument("--message", "-m", type=str, help="message to show, ~squares for square test", default="HELLO")
	parser.add_argument("--debug", "-g", action='store_true', help="turn on debug messages")
	parser.add_argument("--edit", "-e", action='store_true', help="turn on edit mode")
	parser.add_argument("--orientation", "-o", type=int, default=1, help="1 = normal, 2 = sideways, 3 = other sideways", choices=range(1,5))
	parser.add_argument("--direction", "-d", type=int, default=1, help="0 = no scroll, 1 = L->R, 2 = R->L", choices=range(0,3))
	args = parser.parse_args()

	debug = args.debug

	# setup IO
	io.setwarnings(False)
	io.setmode(io.BOARD)
	io.setup(3,io.OUT)

	sr = ShiftRegister(args.dataPin,args.latchPin,args.clockPin)
	if args.edit:
		t = LedMatrix(sr,8,8,100,args.delay,0,args.orientation)

		if len(args.message) > 0:
			t.set_message(args.message[0])
		t.start()
		printDebug("Started, about to enter edit mode")
		t.edit()
	elif args.message == "~squares":
		t = LedMatrix(sr,8*7,8,100,args.delay,args.direction,args.orientation)

		t.set_letter(Square1)
		t.set_letter(Square2)
		t.set_letter(Square3)
		t.set_letter(Square4)
		t.set_letter(Square4)
		t.set_letter(Square2)
		t.set_letter(Square1)

		t.start()

		try:
			input("Press enter to exit\r\n")
		except KeyboardInterrupt:
			pass

	else:
		t = LedMatrix(sr,8*len(args.message),8,100,args.delay,args.direction,args.orientation)
		t.set_message(args.message)
		t.start()

		try:
			input("Press enter to exit")
		except KeyboardInterrupt:
			pass

	t.stop()
	t.join()
	
	

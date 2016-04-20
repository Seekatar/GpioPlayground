
class BitGrid(object):
	"""
	Grid for tracking bits, used for the BitMappedLedMatrix
	"""

	def __init__(self,width=8,height=8):
		"""
		Construct the grid
			width is number of columns, defaulting to 8 (assume 8 high)
		"""
		if height > 32 or width < 1 or height < 1:
			raise "Height must be between 1 and 32, width must be greater than 0"

		self.Width = width
		self.Height = height
		self.Grid = [0] * width # we'll use 8 bits of the number in the array

	def _validate(self,x,y):
		return x < self.Width and x >= 0 and y < self.Height and y >= 0

	def set_row(self,row):
		"""
		Turn on a row, zero-based
		"""
		# 0 for first turns all leds across on
		# second byte is row bit, set only one row bit to turn on row
		if ( not self.validate(row,1)):
			return
		for i in range(len(self.Grid)):
			self.Grid[i] = self.Grid[i] | (1 << row)

	def set_column(self,column):
		"""
		Turn on a column, zero-based
		"""
		if ( not self.validate(1,column)):
			return
		self.Grid[column] = 0xff

	def clear(self):
		"""
		Turn everything off
		"""
		for i in range(len(self.Grid)):
			self.Grid[i] = 0

	def clear_at(self,x,y):
		"""
		set a bit to 0 at location
		"""
		self.set_at(x,y,False)

	def get_at(self,x,y):
		"""
		Get the value at x,y
		"""
		if ( not self._validate(x,y )):
			return

		if (self.Grid[y] & (1 << x)) != 0:
			return 1
		else:
			return 0

	def toggle_at(self,x,y):
		"""
		Toggles the value of the bit at the location
		"""
		self.set_at(x,y,self.get_at(x,y) != 1)

	def set_at(self,x,y,set=True):
		"""
		Set one dot x,y, zero based, ignores values out of range
		"""
		if ( not self._validate(x,y )):
			return

		# set the bit in the grid
		if set:
			self.Grid[y] = self.Grid[y] | (1 << x)
		else:
			self.Grid[y] = self.Grid[y] & ~(1 << x)



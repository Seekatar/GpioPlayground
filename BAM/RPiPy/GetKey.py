import sys,tty,termios
class _Getch:
	def __call__(self,chars=1):
			fd = sys.stdin.fileno()
			old_settings = termios.tcgetattr(fd)
			try:
				tty.setraw(sys.stdin.fileno())
				ch = sys.stdin.read(chars)
			finally:
				termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
			return ch

def get():
		inkey = _Getch()
		k=inkey()

		if k == "\x1b":
			k = k + inkey()
			if k == '\x1b[':
				k = k + inkey()
				if k=='\x1b[A':
						return "up"
				elif k=='\x1b[B':
						return "down"
				elif k=='\x1b[C':
						return "right"
				elif k=='\x1b[D':
						return "left"
				else:
					print( "not an arrow key! "+k)
					return ""
			else:
				print( "not an arrow key! combination "+k)
				return ""
		elif k == "\x03": # Ctrl+C
			raise KeyboardInterrupt
		else:
			return k

def main():
		for i in range(0,20):
				get()

if __name__=='__main__':

	try:
		main()
	except KeyboardInterrupt:
		pass

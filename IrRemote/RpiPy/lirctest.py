"""
	Tiny test program to get IR codes from the input
	Must have LIRC setup and configured on the machine

	Also must have a ~/lircrc file such as this to get 
	any output from below

begin
button = KEY_1
prog = testir
config = one
end
begin
button = KEY_NEXT
prog = testir
config = next
end

http://www.lirc.org/html/configure.html#lircrc_format

"""
import lirc

lirc.init("testir")

while True:
	code = lirc.nextcode()
	print( code )



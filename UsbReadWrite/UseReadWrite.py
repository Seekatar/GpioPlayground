import serial
import time

# doc at http://pyserial.readthedocs.org/en/latest/pyserial_api.html
ser = serial.Serial(port="COM7",baudrate=9600,timeout=1)

print("Is open is",ser.is_open)

if ser.is_open:
    try:
        a = bytearray([2,0,0,0])
        for i in range(100):
            input("Press enter to alert ")
            print( "Sent", ser.write(a) )
            ser.flush()
            i = 0
            while ser.in_waiting < 40 and i < 100:
                time.sleep(.01)
                i = i + 1
            print ("in_waiting is",ser.in_waiting)
            if ser.in_waiting > 0:
                print ("Received:", ser.read(ser.in_waiting) )
        ser.close()
    except Exception as e:
        print("Got exception!",e)
        try:
            ser.close()
        except:
            pass
else:
    print("Port not open")

print( "exiting")

import http.server
import socketserver
import webbrowser as web
from urllib.parse import urlparse
import serial
import time

port = 'COM7'
alertMsg = bytearray([2,0,0,0])
path = "C:\\code\\GpioPlayground\\UsbReadWrite\\" # need trailing slash

ser = serial.Serial(port=port,baudrate=9600,timeout=1)
print( "alert opened port",port)

class GetHandler(http.server.BaseHTTPRequestHandler):
    
    def do_GET(self):
        parsed_path = urlparse(self.path)
        print( "Parsed path is",parsed_path.path)
        if parsed_path.path == '/classical':
            web.open('http://www.pandora.com/station/play/2491058965530864909')
        elif parsed_path.path == '/swing':
            web.open('http://www.pandora.com/station/play/2616279733478157581')
        elif parsed_path.path == '/electroswing':
            web.open('http://www.pandora.com/station/play/2772260146666335501')
        elif parsed_path.path == '/electronic':
            web.open('http://www.pandora.com/station/play/2852350257260719373')
        elif parsed_path.path == '/alert':
            if ser.is_open:
                try:
                    print("Sent",ser.write(alertMsg),"bytes to Arduino")
                    if ser.in_waiting >= 40:
                        print ("Received from Arduinio:", ser.read(40) )
                except Exception as e:
                    print("Got exception processing Arduino!",e)
            else:
                print("Serial port not open")
        elif parsed_path.path == "/":
            file = open(path+'music.htm')
            lines = file.read()
        
            message = lines
            # print(message)
            self.send_response(200)
            self.end_headers()
            self.wfile.write(message.encode())
            
            return

        self.send_response(200)
        self.end_headers()
            
        return
    
if __name__ == '__main__':
    server = socketserver.TCPServer(('', 8080), GetHandler)
    print ('Starting server, use <Ctrl-C> to stop')
    server.serve_forever()

import http.server
import socketserver
import webbrowser as web
from urllib.parse import urlparse
import serial
import time

alertMsg = bytearray([2,0,0,0])
path = "C:\\code\\GpioPlayground\\UsbReadWrite\\" # need trailing slash

class GetHandler(http.server.BaseHTTPRequestHandler):
    
    def do_GET(self):
        parsed_path = urlparse(self.path)
        print( "Headers are ",self.headers)
        print( "Parsed path is",parsed_path.path)

        self.send_response(200)
        self.end_headers()
        self.wfile.write("ok".encode())
        return
    
    def do_POST(self):
        parsed_path = urlparse(self.path)
        print( "Headers are ",self.headers)
        print( "Parsed path is",parsed_path.path)
        content_len = int(self.headers.get('content-length', 0))
        print( "Content len is", content_len)
        print( "body is: ", self.rfile.read(content_len))
            
        self.send_response(200)
        self.end_headers()
        self.wfile.write('{"result":"ok"}'.encode())

        return
    
if __name__ == '__main__':
    server = socketserver.TCPServer(('', 8081), GetHandler)
    print ('Starting server, use <Ctrl-C> to stop')
    server.serve_forever()

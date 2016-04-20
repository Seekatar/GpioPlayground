int cmd = 0;         // incoming serial byte
int r = 0;         // incoming serial byte
int g,b;

char s[41];
char ss[40];

void setup() {
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 3) {
    // get incoming byte:
    cmd = Serial.read();
    r = Serial.read();
    g = Serial.read();
    b = Serial.read();

    sprintf( ss, "Got four bytes %02d %02d %02d %02d",cmd,r,g,b );
    sprintf( s, "%-40s", ss );
    Serial.print(s);
    
  }
}



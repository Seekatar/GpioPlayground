#ifndef __JSDEBUG_H__
#define __JSDEBUG_H__

#ifdef DEBUG

#define DEBUG_PRINT(n) Serial.print(n);
#define DEBUG_PRINTLN(n) {Serial.println(n); delay(100); }
#define DEBUG_PRINTTIME(n) printTime(n);

#else

#define DEBUG_PRINT(n) {}
#define DEBUG_PRINTLN(n) {}
#define DEBUG_PRINTTIME

#endif


#endif
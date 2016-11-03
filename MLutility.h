/*
  __MLutility_h - Library for MOST Link Lora
  MOST-Link utility

  Created by Macbear Chen, September 26, 2016.
  Released into the public domain.
*/

#ifndef __MLutility_h
#define __MLutility_h

#include <Arduino.h>

// common utility
class MLutility {
public:
    static boolean parseGPGGA(const char *GPGGAstr, unsigned long &ts, double &dbLat, double &dbLng, char &gpsStatus);
    
    static int readSerial(char *buf);
    static void printBinary(const byte *data, const int szData);
};


#endif // __MLutility_h


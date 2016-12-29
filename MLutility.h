/*
  __MLutility_h - Library for MOST Link Lora
  MOST-Link utility

  Created by Macbear Chen, September 26, 2016.
  Released into the public domain.
*/

#ifndef __MLutility_h
#define __MLutility_h

#include "MOSTLora_def.h"

// common utility
class MLutility {
public:
    static boolean parseGPGGA(const char *GPGGAstr, unsigned long &ts, double &dbLat, double &dbLng, char &gpsStatus);
    
    static int readSerial(char *buf);
    static void printBinary(const byte *data, const int szData);
    static void generateHMAC(uint8_t *dataDst, const char *keySrc, uint8_t *dataSrc, int szDataSrc);
    
//    static void encryptAES_GCM(byte *srcData, int szData, const byte *srcKey, const byte *srcIV, byte *outTag = NULL);
//    static void decryptAES_GCM(byte *srcData, int szData, const byte *srcKey, const byte *srcIV);

    static boolean encryptAES_CBC(byte *srcData, int szData, const byte *srcKey, const byte *srcIV);
    static boolean decryptAES_CBC(byte *srcData, int szData, const byte *srcKey, const byte *srcIV);
};
#if defined(__LINKIT_ONE__)
extern void dtostrf(float fVal, int width, int precision, char *strBuf);
#endif // (__LINKIT_ONE__)

#endif // __MLutility_h


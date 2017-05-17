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
    static void blinkLed(int nCount = 3, int msOn = 100, int msOff = 100, int idPin = 13);
    static void blinkSOS(int msOn = 100);
    static int Fcopy(char* buf, const __FlashStringHelper *ifsh);

    // PH sensor related
    static float measurePH(int pinPh);  // analog-pin for PH sensor

    
    static boolean parseGPGGA(const char *GPGGAstr, unsigned long &ts, double &dbLat, double &dbLng, char &gpsStatus);
    
    static int readSerial(char *buf);
    static void printBinary(const uint8_t *data, const int szData);
    static void MLutility::stringToHex(char *dst, const char *strSrc, const int szSrc);
    static void stringHexToBytes(uint8_t *dst, const char *strSrc, const int szSrc);
    static int convertMQTTtoHex(uint8_t *dst, const char *username, const char *password, const char *clientID);
    
    static void generateHMAC(uint8_t *dataDst, const char *keySrc, uint8_t *dataSrc, int szDataSrc);
    
//    static void encryptAES_GCM(byte *srcData, int szData, const byte *srcKey, const byte *srcIV, byte *outTag = NULL);
//    static void decryptAES_GCM(byte *srcData, int szData, const byte *srcKey, const byte *srcIV);

    static boolean encryptAES_CBC(byte *srcData, int szData, const byte *srcKey, const byte *srcIV);
    static boolean decryptAES_CBC(byte *srcData, int szData, const byte *srcKey, const byte *srcIV);
    
    static String generateChannelData(const char *strID, const char *strValue);
    static String generateChannelData(const char *strID, float fVal);
    static String generateChannelData(const char *strID, int nVal);
    static boolean parseDownlinkMCS(const char *strBuf, const char *strToken, int &nVal);
};
#if defined(__LINKIT_ONE__)
extern void dtostrf(float fVal, int width, int precision, char *strBuf);
#endif // (__LINKIT_ONE__)

#endif // __MLutility_h


/* viWave LoRa Module by GlobalSat

	MOST-Link protocol
	
 #include <LGPS.h>
 gpsSentenceInfoStruct info;
	
 */

#include "MLutility.h"

// crypto
#ifdef USE_LIB_CRYPTO_HMAC
#include <Crypto.h>
#include <SHA256.h>
#endif // USE_LIB_CRYPTO_HMAC

#if defined(__LINKIT_ONE__)

void dtostrf(float fVal, int width, int precision, char *strBuf)
{
    char strFormat[32];
    sprintf(strFormat, "%%%d.%df", width, precision);
    sprintf(strBuf, strFormat, fVal);
}
#endif // (__LINKIT_ONE__)

static unsigned char getComma(unsigned char num,const char *str)
{
    unsigned char i,j = 0;
    int len=strlen(str);
    for(i = 0;i < len;i ++)
    {
        if(str[i] == ',')
            j++;
        if(j == num)
            return i + 1;
    }
    return 0;
}

static double getDoubleNumber(const char *s)
{
    char buf[10];
    unsigned char i;
    double rev;
    
    i=getComma(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    rev=atof(buf);
    return rev;
}

static double getIntNumber(const char *s)
{
    char buf[10];
    unsigned char i;
    double rev;
    
    i=getComma(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    rev=atoi(buf);
    return rev;
}
/*
 void blink13()
 {
     pinMode(13, OUTPUT);
     digitalWrite(13, HIGH);
     int i;
     for (i = 0; i < 5; i++) {
         delay(100);
         digitalWrite(13, LOW);
         delay(100);
         digitalWrite(13, HIGH);
     }
 }
 */

// Linkit GPS library
// init GPS
//    LGPS.powerOn();
// get GPS data by NMEA code: gpsSentenceInfoStruct info;
//    LGPS.getData(&info);

boolean MLutility::parseGPGGA(const char *GPGGAstr, unsigned long &ts, double &dbLat, double &dbLng, char &gpsStatus)
{
    /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
     * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
     * Where:
     *  GGA          Global Positioning System Fix Data
     *  123519       Fix taken at 12:35:19 UTC
     *  4807.038,N   Latitude 48 deg 07.038' N
     *  01131.000,E  Longitude 11 deg 31.000' E
     *  1            Fix quality: 0 = invalid
     *                            1 = GPS fix (SPS)
     *                            2 = DGPS fix
     *                            3 = PPS fix
     *                            4 = Real Time Kinematic
     *                            5 = Float RTK
     *                            6 = estimated (dead reckoning) (2.3 feature)
     *                            7 = Manual input mode
     *                            8 = Simulation mode
     *  08           Number of satellites being tracked
     *  0.9          Horizontal dilution of position
     *  545.4,M      Altitude, Meters, above mean sea level
     *  46.9,M       Height of geoid (mean sea level) above WGS84
     *                   ellipsoid
     *  (empty field) time in seconds since last DGPS update
     *  (empty field) DGPS station ID number
     *  *47          the checksum data, always begins with *
     */
    char buff[100];
    boolean bRet = false;
    double latitude;
    double longitude;
    int tmp, hour, minute, second, num ;
    if(GPGGAstr[0] == '$')
    {
        // UTC time
        tmp = getComma(1, GPGGAstr);
        ts = getIntNumber(&GPGGAstr[tmp]);
        hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
        minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
        second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
        
        sprintf(buff, "UTC %2d-%2d-%2d", hour, minute, second);
        Serial.println(buff);
        
        // latitude
        tmp = getComma(2, GPGGAstr);
        latitude = getDoubleNumber(&GPGGAstr[tmp]);
        tmp = getComma(3, GPGGAstr);
        if (GPGGAstr[tmp] != 'N')
            latitude = -latitude;
        
        // longitude
        tmp = getComma(4, GPGGAstr);
        longitude = getDoubleNumber(&GPGGAstr[tmp]);
        tmp = getComma(5, GPGGAstr);
        if (GPGGAstr[tmp] != 'E')
            longitude = -longitude;

        // GPS location status
        tmp = getComma(6, GPGGAstr);
        gpsStatus = getIntNumber(&GPGGAstr[tmp]);
        
        // satellite number
        tmp = getComma(7, GPGGAstr);
        num = getIntNumber(&GPGGAstr[tmp]);
        
        sprintf(buff, "lat = %10.4f, lng = %10.4f, GPS num(%d), status:%d", latitude, longitude, num, gpsStatus);
        Serial.println(buff);
        
        // get latitude, longitude data
        dbLat = latitude / 100;
        dbLng = longitude / 100;
        int nLat = dbLat;
        int nLng = dbLng;
        dbLat = nLat + (dbLat - nLat) * 100.0 / 60.0;
        dbLng = nLng + (dbLng - nLng) * 100.0 / 60.0;
        bRet = true;
    }
    return bRet;
}

void MLutility::printBinary(const byte *data, const int szData)
{
    int i;
    for (i = 0; i < szData; i++) {
        if (data[i] < 16)
            Serial.print("0");
        
        Serial.print(data[i], HEX);
    }
    Serial.print(F(" ("));
    Serial.print(szData, DEC);
    Serial.print(F(" bytes)\n"));
}

int MLutility::readSerial(char *buf)
{
    int countBuf = 0;
    while (Serial.available()) {
        int c = Serial.read();
        buf[countBuf] = c;
        
        countBuf++;
        delay(5);
    }
    return countBuf;
}

/////////////////////////////////////////
// challenge-response: HMAC
/////////////////////////////////////////
#define HMAC_SIZE    16
void MLutility::generateHMAC(uint8_t *dataDst, const char *keySrc, uint8_t *dataSrc, int szDataSrc)
{
#ifdef USE_LIB_CRYPTO_HMAC
    SHA256 hash;
    
    hash.resetHMAC(keySrc, strlen(keySrc));
    hash.update(dataSrc, szDataSrc);
    hash.finalizeHMAC(keySrc, strlen(keySrc), dataDst, HMAC_SIZE);
#endif // USE_LIB_CRYPTO_HMAC
    
#ifdef DEBUG_LORA
    debugSerial.print(F("HMAC data: "));
    MLutility::printBinary(dataDst, HMAC_SIZE);
#endif // DEBUG_LORA
}

/////////////////////////////////////////
// AES encrypt, decrypt
/////////////////////////////////////////
#define AES_KEY_SIZE    16
#define AES_IV_SIZE     16

#ifdef USE_LIB_CRYPTO_AES128
#include <AES.h>
#include <Speck.h>
#include <SpeckTiny.h>
#include <CBC.h>
#endif // USE_LIB_CRYPTO_AES128

/*
#define AES_TAG_SIZE    16
#include <GCM.h>
// AES/GCM/NoPadding encrypt, decrypt
void MLutility::encryptAES_GCM(byte *srcData, int szData, const byte *srcKey, const byte *srcIV, byte *outTag) {
    byte bufferAES[99];
    byte tagAES[AES_TAG_SIZE];
    
    GCM<AES128> *gcmaes128 = new GCM<AES128>();
    AuthenticatedCipher *cipher = gcmaes128;
    
    cipher->clear();
    cipher->setKey(srcKey, cipher->keySize());
    cipher->setIV(srcIV, AES_IV_SIZE);
    memset(bufferAES, 0xBA, sizeof(bufferAES));
    
    cipher->encrypt(bufferAES, srcData, szData);
    cipher->computeTag(tagAES, sizeof(tagAES));
    
    MLutility::printBinary(bufferAES,szData);
    memcpy(srcData, bufferAES, szData);
    if (outTag != NULL) {
        memcpy(outTag, tagAES, AES_TAG_SIZE);
    }
    
    cipher->clear();
    delete gcmaes128;
}

void MLutility::decryptAES_GCM(byte *srcData, int szData, const byte *srcKey, const byte *srcIV) {
    byte bufferAES[99];
    
    GCM<AES128> *gcmaes128 = new GCM<AES128>();
    AuthenticatedCipher *cipher = gcmaes128;
    
    cipher->clear();
    cipher->setKey(srcKey, cipher->keySize());
    cipher->setIV(srcIV, AES_IV_SIZE);
    memset(bufferAES, 0xBA, sizeof(bufferAES));

    cipher->decrypt(bufferAES, srcData, szData);
    
    MLutility::printBinary(bufferAES,szData);
    memcpy(srcData, bufferAES, szData);
    
    cipher->clear();
    delete gcmaes128;
}
*/

// AES/CBC/NoPadding encrypt, decrypt
boolean MLutility::encryptAES_CBC(byte *srcData, int szData, const byte *srcKey, const byte *srcIV) {
    if (szData > 96)
        return false;
    
#ifdef USE_LIB_CRYPTO_AES128
    byte bufferAES[96];
    
    CBC<AES128> *aes128 = new CBC<AES128>();
    Cipher *cipher = aes128;
    
    cipher->clear();
    cipher->setKey(srcKey, AES_KEY_SIZE);
    cipher->setIV(srcIV, AES_IV_SIZE);
    cipher->encrypt(bufferAES, srcData, szData);
    
    memcpy(srcData, bufferAES, szData);
    
    cipher->clear();
    delete aes128;
#endif // USE_LIB_CRYPTO_AES128
    
    return true;
}

boolean MLutility::decryptAES_CBC(byte *srcData, int szData, const byte *srcKey, const byte *srcIV) {
    if (szData > 96)
        return false;

#ifdef USE_LIB_CRYPTO_AES128
    byte bufferAES[96];
    
    CBC<AES128> *aes128 = new CBC<AES128>();
    Cipher *cipher = aes128;
    
    cipher->clear();
    cipher->setKey(srcKey, AES_KEY_SIZE);
    cipher->setIV(srcIV, AES_IV_SIZE);
    cipher->decrypt(bufferAES, srcData, szData);
    
    memcpy(srcData, bufferAES, szData);
    
    cipher->clear();
    delete aes128;
#endif // USE_LIB_CRYPTO_AES128
    
    return true;
}

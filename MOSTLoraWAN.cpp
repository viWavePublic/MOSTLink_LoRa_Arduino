//
//  MOSTLoraWAN.cpp
//
//  Created by Macbear Chen on 2017/2/2.
//  Copyright © 2017 viWave. All rights reserved.
//
// Reference:
//   1. 亞太 LoRaTM Modem User’s Guide
/////////////////////////////////////////

#include "MOSTLoraWAN.h"
#include "MLutility.h"

#define MAX_SIZE_CMD     40

// Flash string (to reduce memory usage in SRAM)
char *MOSTLoraWAN::command(const __FlashStringHelper *strCmd)
{
    char *strBuf = (char*)_buf;
    // read back a char
    MLutility::Fcopy(strBuf, strCmd);
    command(strBuf);
}

char *MOSTLoraWAN::command(const char *strCmd)
{
    if (NULL == strCmd || strlen(strCmd) > MAX_SIZE_CMD - 3)
        return NULL;
    
    char strFull[MAX_SIZE_CMD] = {0};
    sprintf(strFull, "%s\r\n", strCmd);     // add CR,LR
    sendData(strFull);
    
    unsigned long tsStart = millis();
    int szRece = 0;
    while (millis() - tsStart < 3000) {
        szRece = receData();
        if (szRece > 0) {
            Serial.println("------");
            break;
        }
        delay(100);
    }
    _buf[szRece] = 0;
    return (char*)_buf;
}
/////////////////////////////////////////
// response "ok", return true
boolean MOSTLoraWAN::isOK(const char *strResponse)
{
    boolean bRet = false;
    if (NULL != strResponse && strlen(strResponse) >= 2) {
        bRet = ('o' == strResponse[0] && 'k' == strResponse[1]);
    }
    return bRet;
}

void MOSTLoraWAN::testAll()
{
    getFwVersion();
    getTestMode();
    getDevAddr();
    getDevEui();
    getAppEui();
    getNwkSKey();
    getAppSKey();
}

/////////////////////////////////////////

void MOSTLoraWAN::updateFW()
{
    command(F("AAT1 UpdateFW"));
}
void MOSTLoraWAN::saveSetting()
{
    command(F("AAT1 Save"));
}
char *MOSTLoraWAN::getFwVersion()
{
    return command(F("AAT1 FwVersion"));
}
void MOSTLoraWAN::reset()
{
    command(F("AAT1 Reset"));
}

void MOSTLoraWAN::setSLEEP()
{
    command(F("AAT1 SLEEP"));
}

void MOSTLoraWAN::restore()
{
    command(F("AAT1 Restore"));
}

boolean MOSTLoraWAN::setTx(int port, char *confirm, char *payload)
{
    sprintf(_strBuf, "AAT2 Tx=%d,%s,%s", port, confirm, payload);
    char *strRet = command(_strBuf);
    
    return isOK(strRet);
}

/////////////////////////////////////////
// TestMode
boolean MOSTLoraWAN::setTestMode(int mode)
{
    sprintf(_strBuf, "AAT1 TestMode=%d", mode);
    command(_strBuf);
}
int MOSTLoraWAN::getTestMode()
{
    char *strRet = command(F("AAT1 TestMode=?"));
    return atoi(strRet);
}
/////////////////////////////////////////
// DevAddr 02410113
boolean MOSTLoraWAN::setDevAddr(const char *strAddr)
{
    sprintf(_strBuf, "AAT2 DevAddr=%s", strAddr);
    char *strRet = command(_strBuf);

    return isOK(strRet);
}
char *MOSTLoraWAN::getDevAddr()
{
    return command(F("AAT2 DevAddr=?"));
}

/////////////////////////////////////////
// DevEui
boolean MOSTLoraWAN::setDevEui(const char *strEui)
{
    sprintf(_strBuf, "AAT2 DevEui=%s", strEui);
    char *strRet = command(_strBuf);
    
    return isOK(strRet);
}

char *MOSTLoraWAN::getDevEui()
{
    return command(F("AAT2 DevEui=?"));
}
/////////////////////////////////////////
// AppEui
boolean MOSTLoraWAN::setAppEui(const char *strEui)
{
    sprintf(_strBuf, "AAT2 AppEui=%s", strEui);
    char *strRet = command(_strBuf);
    
    return isOK(strRet);
}
char *MOSTLoraWAN::getAppEui()
{
    return command(F("AAT2 AppEui=?"));
}

/////////////////////////////////////////
// NwkSKey
boolean MOSTLoraWAN::setNwkSKey(const char *strKey)
{
    sprintf(_strBuf, "AAT2 NwkSKey=%s", strKey);
    char *strRet = command(_strBuf);
    
    return isOK(strRet);
}
char *MOSTLoraWAN::getNwkSKey()
{
    return command(F("AAT2 NwkSKey=?"));
}

/////////////////////////////////////////
// AppSKey
boolean MOSTLoraWAN::setAppSKey(const char *strKey)
{
    sprintf(_strBuf, "AAT2 AppSKey=%s", strKey);
    char *strRet = command(_strBuf);
    
    return isOK(strRet);
}
char *MOSTLoraWAN::getAppSKey()
{
    return command(F("AAT2 AppSKey=?"));
}

/////////////////////////////////////////
// AppKey
boolean MOSTLoraWAN::setAppKey(const char *strKey)
{
    sprintf(_strBuf, "AAT2 AppKey=%s", strKey);
    char *strRet = command(_strBuf);
    
    return isOK(strRet);
}
char *MOSTLoraWAN::getAppKey()
{
    return command(F("AAT2 AppKey=?"));
}

/////////////////////////////////////////
// ADR
boolean MOSTLoraWAN::setADR(int state)
{
    sprintf(_strBuf, "AAT2 ADR=%d", state);
    command(_strBuf);
}
int MOSTLoraWAN::getADR()
{
    char *strRet = command(F("AAT2 ADR=?"));
    return atoi(strRet);
}





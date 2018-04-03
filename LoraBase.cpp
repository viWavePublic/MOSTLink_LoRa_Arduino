//
//  LoraBase.cpp
//  libMOST
//
//  Created by Macbear Chen on 2017/2/2.
//  Copyright © 2017 viWave. All rights reserved.
//

#include "LoraBase.h"
#include "MLutility.h"
#include "MOSTLora.h"

///////////////////////
// LoRa Serial bus: RX/TX
///////////////////////
#if defined(__LINKIT_ONE__)
    #define loraSerial Serial1       // for LinkIt ONE

#else // __LINKIT_ONE__

#ifdef USE_VINDUINO
#define loraSerial Serial       // for Vinduino
#else // USE_VINDUINO
    // for arduino Uno
    #include <SoftwareSerial.h>
    const int pinLoraRX = 10;
    const int pinLoraTX = 11;
    //      pinMode(pinLoraRX, INPUT);
    //      pinMode(pinLoraTX, OUTPUT);
    SoftwareSerial loraSerial(pinLoraRX, pinLoraTX);    // RX, TX

#endif // USE_VINDUINO

#endif // __LINKIT_ONE__

/////////////////////////////////////////

LoraBase::LoraBase(int pinP1, int pinP2, int pinBusy)
{
    _pinP1 = pinP1;
    _pinP2 = pinP2;
    _pinBZ = pinBusy;
    _eMode = E_UNKNOWN_LORA_MODE;
    
    _strBuf = (char*)_buf;
}

void LoraBase::begin(long speed)
{
    pinMode(_pinP1, OUTPUT);
    pinMode(_pinP2, OUTPUT);
    pinMode(_pinBZ, INPUT);
    
    loraSerial.begin(speed);
#ifdef DEBUG_LORA
    debugSerial.print(F("(rx/tx)baud-rate="));
    debugSerial.println(speed);
#endif // DEBUG_LORA
    
}

// LM-110H1 module for 3 firmware mode:
// a. AT command: LoRaWAN
// b. AT command: MOST
// c. p1p2 mode: MOST
void LoraBase::setFirmwareMode(E_LORA_FW_MODE modeFW)
{
    unsigned char arrayLORAWAN[8] = {0XFF,0X4C,0XCF,0X52,0XA1,0X64,0X47,0X00};
    E_LORA_FW_MODE eModeCurr = E_UNKNOWN_FW_MODE;
    eModeCurr = getFirmwareMode();
    if (eModeCurr == modeFW)
        return;
    
    switch (modeFW) {
        case E_FW_AAT_LORAWAN:
            if (E_FW_AAT_MOST == eModeCurr)
            {
                // Firmware: AAT MOST --> LoRaWAN
                command(F("AAT1 LW=0"));
                saveSetting();
                reset();
            }
            else if (E_FW_P1P2_MOST == eModeCurr)
            {
                // Firmware: p1p2--> LoRaWAN
                loraSerial.begin(9600);

                setMode(E_LORA_NORMAL);     // p1(0 -> 1) pin for setup
                delay(1000);
                setMode(E_LORA_SETUP);
                sendData(arrayLORAWAN, 8);
                // success: 41 43 4B 18 21 C9
                loraSerial.begin(57600);
                const bool bReceResponse = waitRece("Program start", 10000);
            }
#ifdef DEBUG_LORA
            debugSerial.println(F("*** AAT LoRaWAN ***"));
#endif // DEBUG_LORA
            break;
            
        case E_FW_AAT_MOST:
            if (E_FW_P1P2_MOST == eModeCurr) {
                setFirmwareMode(E_FW_AAT_LORAWAN);
            }
            
            if (E_FW_P1P2_MOST == eModeCurr || E_FW_AAT_LORAWAN == eModeCurr) {
                command(F("AAT1 LW=1"));
                saveSetting();
                reset();
            }
#ifdef DEBUG_LORA
            debugSerial.println(F("*** AAT MOST ***"));
#endif // DEBUG_LORA
            break;

        case E_FW_P1P2_MOST:
            if (E_FW_AAT_MOST == eModeCurr) {
                setFirmwareMode(E_FW_AAT_LORAWAN);
            }
            
            if (E_FW_AAT_MOST == eModeCurr || E_FW_AAT_LORAWAN == eModeCurr) {
                command(F("AAT2 Lora_Most_Switch=1"));
                saveSetting();
                reset();
                //MOSTLora loraP1P2;
                loraSerial.begin(9600);
            }
#ifdef DEBUG_LORA
            debugSerial.println(F("*** p1p2 MOSTLink ***"));
#endif // DEBUG_LORA
            break;
        default:
            break;
    }
}

E_LORA_FW_MODE LoraBase::getFirmwareMode()
{
    E_LORA_FW_MODE nRet = E_UNKNOWN_FW_MODE;
    loraSerial.begin(57600);
    const char *strResult = getProtocol();
    if (strlen(strResult) > 0)
    {
        if ('1' == strResult[0]) {
            nRet = E_FW_AAT_MOST;
        }
        else if ('0' == strResult[0]) {
            nRet = E_FW_AAT_LORAWAN;
        }
    }
    if (E_UNKNOWN_FW_MODE == nRet) {
        MOSTLora loraP1P2;
        loraSerial.begin(9600);
        if (loraP1P2.readConfig(3)) {
            nRet = E_FW_P1P2_MOST;            
        }
    }
    return nRet;
}

/////////////////////////////////////////
// p1p2 MOST
void LoraBase::setMode(int mode)
{
    if (_eMode == mode)
        return;
    
    if (E_LORA_NORMAL == mode)
        setMode(0, 0);
    else if (E_LORA_WAKEUP == mode)
        setMode(0, 1);
    else if (E_LORA_POWERSAVING == mode) {
        if (E_LORA_SETUP == _eMode) {       // Setup -> Normal -> Power Saving
            setMode(0, 0);
        }
        setMode(1, 0);
    }
    else if (E_LORA_SETUP == mode) {
        if (E_LORA_POWERSAVING == _eMode) { // Power Saving -> Normal -> Setup
            setMode(0, 0);
        }
        setMode(1, 1);
    }
    
    // assign to new state
    _eMode = mode;
}

/////////////////////////////////////////
// setup(1,1), normal(0,0), wakeup(0,1), powersaving(1,0)
void LoraBase::setMode(int p1, int p2)
{
    digitalWrite(_pinP1, p1);  // setup(1,1), normal(0,0)
    digitalWrite(_pinP2, p2);
    delay(200);
}

boolean LoraBase::available()
{
    return loraSerial.available();
}

/////////////////////////////////////////
// send data via LoRa
int LoraBase::sendData(uint8_t *data, int szData)
{
    waitUntilReady(3000);
    int nRet = loraSerial.write(data, szData);
    delay(100);
#ifdef DEBUG_LORA
    debugSerial.print(F("Send > "));
    MLutility::printBinary(data, szData);
#endif // DEBUG_LORA
    return nRet;
}

/////////////////////////////////////////
// send string via LoRa
int LoraBase::sendData(const char *strData)
{
    if (NULL == strData)
        return 0;
    
    int nRet = sendData((uint8_t*)strData, strlen(strData));
    
#ifdef DEBUG_LORA
    debugSerial.print(F(">>> "));
    debugSerial.println(strData);
#endif // DEBUG_LORA
    return nRet;
}

/////////////////////////////////////////
// run loop to rece data
void LoraBase::run()
{
    if (available()) {
        receData();
    }
}
/////////////////////////////////////////
// receive data via LoRa
int LoraBase::receData()
{
    _szBuf = 0;
    if (!loraSerial.available())
        return _szBuf;

    int i, nRssi = 0;
    for (i = 0; i < 6; i++) {
        int nCharRead = 0;
        while (loraSerial.available() && (_szBuf < MAX_SIZE_BUF)) {
            int c = loraSerial.read();
            _buf[_szBuf] = c;
            
            _szBuf++;
            nCharRead++;
            delay(1);
        }
        
        delay(300);
        if (nCharRead > 0) {
#ifdef DEBUG_LORA
            debugSerial.print(nCharRead);
            debugSerial.print(F(") "));
#endif // DEBUG_LORA
            if (E_LORA_WAKEUP == _eMode) {      // get RSSI at last character
                _szBuf--;
#ifdef DEBUG_LORA
                nRssi = _buf[_szBuf];
                debugSerial.print(nRssi);
                debugSerial.print(F(" rssi. "));
#endif // DEBUG_LORA
            }
        }
    }
    _buf[_szBuf] = 0;
    
#ifdef DEBUG_LORA
    if (_szBuf > 0) {
        MLutility::printTime(millis());
        debugSerial.print(F("\nRece < "));
        MLutility::printBinary(_buf, _szBuf);
        debugSerial.print(F("<<< "));
        debugSerial.println((char*)_buf);
    }
#endif // DEBUG_LORA

    return _szBuf;
}

boolean LoraBase::isBusy()
{
    boolean bRet = true;
    int nBusy = 0;
    
    if (_pinBZ >= 14) {     // analog BZ
        nBusy = analogRead(_pinBZ);
        bRet = (nBusy < 512);
    }
    else {                  // digital BZ
        const int nBusy = digitalRead(_pinBZ);
        bRet = (nBusy < 1);
    }
    
    /*    if (bRet) {
     #ifdef DEBUG_LORA
     const char *strBZ = " busy ...";
     debugSerial.print(nBusy, DEC);
     debugSerial.println(strBZ);
     #endif // DEBUG_LORA
     }
     */
    return bRet;
}

boolean LoraBase::waitUntilReady(unsigned long timeout)
{
    boolean bRet = true;
    unsigned long tsStart = millis();
    while (isBusy()) {
        delay(100);
        if (timeout < millis() - tsStart) {
            bRet = false;
            break;
        }
    }
#ifdef DEBUG_LORA
    debugSerial.print((millis() - tsStart));
    debugSerial.println(F(" Ready"));
#endif // DEBUG_LORA
    return bRet;
}

///////////////////////
// for AT command
///////////////////////
#define MAX_SIZE_CMD     60

// Flash string (to reduce memory usage in SRAM)
char *LoraBase::command(const __FlashStringHelper *strCmd, int waitResponse)
{
    // read back a char
    MLutility::Fcopy(_strBuf, strCmd);
    return command(_strBuf, waitResponse);
}

char *LoraBase::command(const char *strCmd, int waitResponse)
{
    if (NULL == strCmd || strlen(strCmd) > MAX_SIZE_CMD - 3)
        return NULL;
    
    char strFull[MAX_SIZE_CMD] = {0};
    sprintf(strFull, "%s\r\n", strCmd);     // add CR,LR
    sendData(strFull);
    
    const bool bReceResponse = waitRece(NULL, waitResponse);
#ifdef DEBUG_LORA
    if (bReceResponse) {
        debugSerial.println(F("+++ AT Response is OK."));
    }
    else {
        debugSerial.println(F("!!! AT Response nothing."));
    }
#endif // DEBUG_LORA
    
    return _strBuf;
}

bool LoraBase::waitRece(const char *strWait, int waitResponse)
{
    bool bRet = false;
    unsigned long tsStart = millis();
    
    int szRece = 0;
    _strBuf[szRece] = 0;
    while (millis() - tsStart < waitResponse) {     // response in 6000ms
        szRece = receData();
        if (szRece > 0) {
            if ((NULL == strWait) || (NULL != strstr(_strBuf, strWait))) {
                bRet = true;
                break;
            }
        }
        delay(100);
    }
    return bRet;
}

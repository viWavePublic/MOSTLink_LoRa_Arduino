//
//  MOSTLoraWAN.h
//  LM-130 module AT Commands
//
//  Created by Macbear Chen on 2017/2/2.
//  Copyright © 2017 viWave. All rights reserved.
//

#ifndef MOSTLoraWAN_h
#define MOSTLoraWAN_h

#include "LoraBase.h"

class MOSTLoraWAN : public LoraBase
{
public:
    void begin(long speed = 57600) {
        LoraBase::begin(speed);
    }
    
    char *command(const __FlashStringHelper *strCmd);
    char *command(const char PROGMEM *strCmd);
    boolean isOK(const char *strResponse);
    void testAll();
    
    void updateFW();
    void saveSetting();
    char *getFwVersion();
    void reset();
    void setSLEEP();
    void restore();
    
    // send payload by Tx: port: 1~223, cnf/uncnf, HEX-payload(11 bytes)
    boolean setTx(int port, char *confirm, char *payload);

    // testmode: 0 - disable, 1- enable
    boolean setTestMode(int mode);
    int getTestMode();
    
    boolean setDevAddr(const char *strAddr);
    char *getDevAddr();
    
    boolean setDevEui(const char *strEui);
    char *getDevEui();
    
    boolean setAppEui(const char *strAddr);
    char *getAppEui();
    
    boolean setNwkSKey(const char *strKey);
    char *getNwkSKey();

    boolean setAppSKey(const char *strKey);
    char *getAppSKey();

    boolean setAppKey(const char *strKey);
    char *getAppKey();
    
    // testmode: 0 - disable, 1- enable
    boolean setADR(int state);
    int getADR();
};


#endif /* MOSTLoraWAN_h */

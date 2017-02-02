//
//  LoraBase.h
//  libMOST
//
//  Created by Macbear Chen on 2017/2/2.
//  Copyright © 2017 viWave. All rights reserved.
//

#ifndef LoraBase_h
#define LoraBase_h

#include "MOSTLora_def.h"

// 4 mode: normal, wakeup, powersaving, setup
enum E_LORA_MODE {
    E_UNKNOWN_LORA_MODE = -1,
    E_LORA_NORMAL = 0,
    E_LORA_WAKEUP = 1,
    E_LORA_POWERSAVING = 2,
    E_LORA_SETUP = 3,
};

class LoraBase
{
#define MAX_SIZE_BUF        99
protected:
    int _pinP1, _pinP2, _pinBZ;   // p1, p2, busy
    int _eMode;
    
    // buffer
    int _szBuf;
    unsigned char _buf[MAX_SIZE_BUF + 1];
    
public:
    LoraBase(int pinP1 = 7, int pinP2 = 6, int pinBusy = 5);
    void begin(long speed = 9600);
    unsigned char *getBuffer() { return _buf; }
    
protected:
    // setup(1,1), normal(0,0), wakeup(0,1), powersaving(1,0)
    void setMode(int p1, int p2);
    boolean available();

public:
    void setMode(int mode);
    int getMode() { return _eMode; }
    
    // send/rece data via LoRa
    int sendData(uint8_t *data, int szData);
    int sendData(const char *strData);
    int receData();
    
    // LoRa busy state
    boolean isBusy();
    boolean waitUntilReady(unsigned long timeout);
};


#endif /* LoraBase_h */

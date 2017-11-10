//
//  MOSTLoraAT.h
//  libMOST
//
//  Created by Macbear Chen on 2017/11/8.
//  Copyright © 2017年 viWave. All rights reserved.
//

#ifndef MOSTLoraAT_h
#define MOSTLoraAT_h

#include "MOSTLora.h"

class MOSTLoraAT : public MOSTLora
{
public:
    // initialize for begin baud-rate
    void begin(long speed = 57600) {
        LoraBase::begin(speed);
    }
    
    int sendDataAT(uint8_t *data, int szData);
    virtual int receData();
    
    char *getDeviceMac();           // Example: ABCDEF0123456789
    char *getDeviceName();          // Example: LM130H1
    char *getDeviceFwVer();         // Example: F-0LR-1E-1703221
    
    void setDeviceRole(int nRole);  // 0 -Node, 1 -Gateway
    char *getDeviceRole();          // 0 -Node, 1 -Gateway
    
    // Note:
    // 1. In Mode-2, the module stays awake and send wake up code
    //    (i.e. Normal mode plus with preamble byte) if data transmission needed.
    // 2. InMode-3,themodulekeptsleep,send[AAT1 WK] or any command would
    //    feedback “ACK” on screen as a wake up code. Within 0.5 sec the ACK respond,
    //    send [AAT3 MD=1] to change it back to Mode-1.
    //    It would only accept commands [AAT3 MD] and [AAT1 Save] during this mode.
    void setDeviceMode(int nMode);  // 1 -Normal mode, 2 -Wake up mode, 3 -Power saving mode
    char *getDeviceMode();          // 1 -Normal mode, 2 -Wake up mode, 3 -Power saving mode
    
    void setChannelGroupID(int idGroup);
    char *getChannelGroupID();      // Range: 0-255
    void setChannelFreq(int freq)
    {
        setChannelFreq_RxTx(freq, freq);
    }
    void setChannelFreq_RxTx(int freqRx, int freqTx);
    char *getChannelFreq_RxTx();    // Frequency default Rx/Tx shows [915000,915000]
    void setChannelDataRate(int nDataRate);
    char *getChannelDataRate();     // 0=0.81K, 1=1.46K, 2=2.6K, 3=4.56K, 4=9.11K, 5=18.23K bps
    void setChannelWakeupTime(int nWakeupTime);
    char *getChannelWakeupTime();   // 0=200ms, 1=400ms, 2=600ms, 3=1s, 4=1.5s, 5=2s, 6=2.5s, 7=3s, 8=4s, 9=5s
    void setTxPower(int nPower);
    char *getTxPower();             // Range: 0-7, Default=7, 20dBm
    
    // AES
    
};

#endif /* MOSTLoraAT_h */


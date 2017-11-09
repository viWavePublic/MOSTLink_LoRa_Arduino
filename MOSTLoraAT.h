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
        MOSTLora::begin(speed);
    }
    
    int sendDataAT(uint8_t *data, int szData);
    virtual int receData();
    
    char *getDeviceMac();           // Example: ABCDEF0123456789
    char *getDeviceName();          // Example: LM130H1
    char *getDeviceFwVer();         // Example: F-0LR-1E-1703221
    char *getChannelGroupID();      // Range: 0-255
    char *getChannelFreq_RxTx();    // Frequency default Rx/Tx shows [915000,915000]
    char *getChannelDataRate();     // 0=0.81K, 1=1.46K, 2=2.6K, 3=4.56K, 4=9.11K, 5=18.23K bps
    char *getChannelWakeupTime();   // 0=200ms, 1=400ms, 2=600ms, 3=1s, 4=1.5s, 5=2s, 6=2.5s, 7=3s, 8=4s, 9=5s
    char *getTxPower();             // Range: 0-7, Default=7, 20dBm
};

#endif /* MOSTLoraAT_h */


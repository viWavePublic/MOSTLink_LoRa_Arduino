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
    
    // send payload by Tx: port: 1~223, cnf/uncnf, Hex-payload(11 bytes)
    boolean setTx(int port, char *confirm, char *payload);

    // TestMode: 0 - disable, 1 - enable
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
    
    // ADR: 0 - disable, 1 - enable
    boolean setADR(int state);
    int getADR();

    // EVK_TxCycle: 1~254 sec
    boolean setEVK_TxCycle(int interval);
    int getEVK_TxCycle();

    // JoinMode: 0 - disable, 1 - enable
    boolean setJoinMode(int mode);
    int getJoinMode();

    // reTx: 0~8
    boolean setreTx(int retry);
    int getreTx();

    // RxDelay1: 100000~10000000
    boolean setRxDelay1(long delayMicroSec);
    long getRxDelay1();
    
    // DutyCycle: 0 - disable, 1 - enable
    boolean setDutyCycle(int state);
    int getDutyCycle();
    
    // PLCheck: 0 - disable, 1 - enable
    boolean setPLCheck(int state);
    int getPLCheck();
    
    // Rx2_Freq_DR: frequency 000000001~999999999 in Hz, data-rate 0~15
    boolean setRx2_Freq_DR(long freq, int dataRate);
    void getRx2_Freq_DR(long &freq, int &dataRate);

    // ClassMode: 0 - Class A, 2 - Class C
    boolean setClassMode(int mode);
    int getClassMode();
    
    // Rx1DrOffset: index of Rx1 offset 0~3
    boolean setRx1DrOffset(int offset);
    int getRx1DrOffset();
    
    // Tx_Channel[x]:
    //      channel: US 0~71, EU 0~15
    //      frequency 000000001~999999999 in Hz
    //      data-rate 0~15
    //      channel state: 0 - close, 1 - open
    //      band grouping: US 0, EU 0~3
    boolean setTx_Channel(int channel, long freq, int dataRate, int channelOpen, int bandGroup);
    void getTx_Channel(int channel, long &freq, int &dataRate, int &channelOpen, int &bandGroup);

    // Tx_Band[x]: band grouping: US 0, EU 0~3, dutyCycle 1~9999, ID-power 0~15
    boolean setTx_Band(int bandGroup, int dutyCycle, int idPower);
    void getTx_Band(int bandGroup, int &dutyCycle, int &idPower);

    // Uplink_Count
    int getUplink_Count();

    // Downlink_Count
    int getDownlink_Count();

    // Tx_Power[x]: ID-power 0~15, dbm: US 0~30, EU 0~20
    boolean setTx_Power(int idPower, int dbmPower);
    int getTx_Power(int idPower);
    
    // Pl_Max_Length[x]: dataRate 0~15, payload length: 0~255
    boolean setPl_Max_Length(int dataRate, int lenPayload);
    int getPl_Max_Length(int dataRate);

    // Plre_Max_Length[x]: dataRate 0~15, payload length: 0~255
    boolean setPlre_Max_Length(int dataRate, int lenPayload);
    int getPlre_Max_Length(int dataRate);
};


#endif /* MOSTLoraWAN_h */

//
//  MOSTLoraAT.cpp
//  libMOST
//
//  Created by Macbear Chen on 2017/11/8.
//  Copyright © 2017年 viWave. All rights reserved.
//

#include "MOSTLoraAT.h"
#include "MLutility.h"

/////////////////////////////////////////
// Tx/Rx data by LoRa Module
/////////////////////////////////////////
int MOSTLoraAT::sendDataAT(uint8_t *data, int szData)
{
    char dataHex[200];
    char strCmd[256];
    MLutility::stringToHex(dataHex, data, szData);
    sprintf(strCmd, "AAT1 T1=%s", dataHex);
    
    return MOSTLora::command(strCmd);
}

int MOSTLoraAT::receData()
{
    int szRece = LoraBase::receData();
    if (strstr(_strBuf, "AAT1 R1=") == _strBuf)
    {
        const char *strRssi = _strBuf + 8;
        int rssi = atoi(strRssi) - 159;
        debugSerial.print("rssi=");
        debugSerial.print(rssi);

        // rece from LoRa Module
        const char *dataHex = strstr(strRssi, ",");
        if (NULL != dataHex)
        {
            dataHex++;
            uint8_t dataBuf[100];
            int szHex = strlen(dataHex);
            MLutility::stringHexToBytes(dataBuf, dataHex, szHex);
            memcpy(_strBuf, dataBuf, szHex / 2);
            debugSerial.print(", Binary=");
            debugSerial.println(_strBuf);
        }
    }
   
    return szRece;
}
/////////////////////////////////////////
// get LoRa Device info
/////////////////////////////////////////
char *MOSTLoraAT::getDeviceMac()
{
    return command(F("AAT1 L0=?"));
}
char *MOSTLoraAT::getDeviceName()
{
    return command(F("AAT1 L1=?"));
}
char *MOSTLoraAT::getDeviceFwVer()
{
    return command(F("AAT1 L2=?"));
}
char *MOSTLoraAT::getChannelGroupID()
{
    return command(F("AAT1 L3=?"));
}
char *MOSTLoraAT::getChannelFreq_RxTx()
{
    return command(F("AAT1 L4=?"));
}
char *MOSTLoraAT::getChannelDataRate()
{
    return command(F("AAT1 L5=?"));
}
char *MOSTLoraAT::getChannelWakeupTime()
{
    return command(F("AAT1 L6=?"));
}
char *MOSTLoraAT::getTxPower()
{
    return command(F("AAT1 L7=?"));
}
/////////////////////////////////////////
// set LoRa Device configure
/////////////////////////////////////////

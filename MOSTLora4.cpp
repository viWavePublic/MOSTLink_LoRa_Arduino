/*
  Library for MOSTLink LoRa Shield Library
 
  MOSTLink LoRa Shield with Semtech sx1276 for LoRa technology.
  Support device: Arduino UNO, Linkit One, Vinduino
 
  2016 Copyright (c) viWave Co. Ltd., All right reserved.
  http://mostlink.viwave.com/
 
  Original Author: Macbear Chen, Jan 21, 2018.
  --------------------------------------------------------------
 
 */

#include "MOSTLora.h"
#include "MLPacketGen2.h"
#include "MLPacketGen3.h"

// ACK: Acknowledgement, for reply ACK-packet
void MOSTLora::sendPacketACK(uint8_t *idMac, bool bUplink)
{
    int nDirection = bUplink ? 1 : 0;
    MLPacketGen mlPacketGen(0, 0, 1, nDirection, idMac);
    
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

/////////////////////////////////////////
// KOSTLink 1.5 protocol (payload ver: 0x0C)
/////////////////////////////////////////
// REQ_NDCALL (Downlink)
void MOSTLora::sendPacketReqNDCall(uint8_t *idMac, uint8_t ansType, uint8_t action, uint16_t ansTypeEx_ResInterval, uint16_t ansTypeEx_DelayTime)
{
    MLPacketGen mlPacketGen(0,0,0,0,idMac);
    MLPayloadGen *pPayload = new MLReqNDCallPayloadGen(ansType, action, ansTypeEx_ResInterval, ansTypeEx_DelayTime);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);

    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// REQ_LOCATION (Downlink)
void MOSTLora::sendPacketReqLocation(uint8_t *idMac, uint8_t reportType, uint8_t action, uint16_t ansTypeEx_ResInterval, uint16_t ansTypeEx_DelayTime)
{
    MLPacketGen mlPacketGen(0,0,0,0,idMac);
    MLPayloadGen *pPayload = new MLReqLocationPayloadGen(reportType, action, ansTypeEx_ResInterval, ansTypeEx_DelayTime);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

//CMD_REQ_GTR_COMMAND
void MOSTLora::sendPacketReqGtrCommand(uint8_t *idMac, char *cmdParam, bool bPresetL2)
{
    MLPacketGen mlPacketGen(0,0,0,0,idMac);
    MLPayloadGen *pPayload = new MLReqGtrCommandPayloadGen(cmdParam, bPresetL2);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// ANS_ALARM (Downlink)
void MOSTLora::sendPacketAnsAlarm(uint8_t *idMac, uint8_t conditionFlag)
{
    MLPacketGen mlPacketGen(0,0,0,0,idMac);
    MLPayloadGen *pPayload = new MLAnsAlarmPayloadGen(getMacAddress(), conditionFlag);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

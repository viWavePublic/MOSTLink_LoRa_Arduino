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
#include "MLutility.h"

/////////////////////////////////////////
// KOSTLink 1.5 protocol
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

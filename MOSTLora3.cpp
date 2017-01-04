/*
  Library for MOSTLink LoRa Shield Library
 
  MOSTLink LoRa Shield with Semtech sx1276 for LoRa technology.
  Support device: Arduino UNO, Linkit One, Vinduino
 
  2016 Copyright (c) viWave Co. Ltd., All right reserved.
  http://mostlink.viwave.com/
 
  Original Author: Macbear Chen, Auguest 12, 2016.
  --------------------------------------------------------------
 
 */

#include "MOSTLora.h"
#include "MLPacketGen.h"

/////////////////////////////////////////
// Mediatek Cloud Sandbox (MCS)
/////////////////////////////////////////
// send CMD_REQ_LOGIN_MCS to gateway, to login MCS
void MOSTLora::sendPacketReqLoginMCS(uint8_t *data, int szData)
{
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLReqLoginMcsPayloadGen(szData, data);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// send uplink command to MCS
void MOSTLora::sendPacketSendMCSCommand(uint8_t *data, int szData)
{
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLSendMcsCommandPayloadGen(szData, data);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

/////////////////////////////////////////
// myDevices(Cayenne)
/////////////////////////////////////////
void MOSTLora::sendPacketReqLoginMydevices(uint8_t *data, int szData)
{
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLReqLoginMydevicesPayloadGen(szData, data);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}
void MOSTLora::sendPacketSendMydevicesCommand(uint8_t *data, int szData)
{
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLSendMydevicesCommandPayloadGen(szData, data);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

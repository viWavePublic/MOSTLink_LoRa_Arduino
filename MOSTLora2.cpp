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
#include "MLutility.h"
#include "MLpacket.h"
#include "MLPacketGen.h"
#include "MLPacketParser.h"

// crypto
#include <Crypto.h>
#include <SHA256.h>

int MOSTLora::parsePacket()
{
    int nRet = -1;
    if (_buf[0] == '$') {     // for LT200 protocol
        
    }
    else if (_buf[0] == 0xFB && _buf[1] == 0xFC) {    // for MOST Link protocol
        MLPacketCtx pkctx;
        MLPacketParser pkParser;
        
        // packet header
        int nResult = pkParser.mostloraPacketParse(&pkctx, _buf);
        if (nResult == 0) {     // packet CRC correct
            byte *pMac = (byte*)&pkctx._id;
#ifdef DEBUG_LORA
            if (pkctx._direction == 0)
                debugSerial.print("Downlink");
            else
                debugSerial.print("Uplink");
            
            debugSerial.print(": cmd(");
            debugSerial.print((int)pkctx._mlPayloadCtx._cmdId, 10);
            debugSerial.print(") 0x");
            debugSerial.print((int)pkctx._mlPayloadCtx._cmdId, 16);
            
            debugSerial.print(", node-ID:");
            MLutility::printBinary(pMac, 8);
#endif // DEBUG_LORA
            
            if (memcmp(pMac, _data.mac_addr, 8) == 0) // packet for me
            {
                nRet = pkctx._mlPayloadCtx._cmdId;
                if (nRet == CMD_REQ_AUTH_CHALLENGE) {
                    byte *pData = &_buf[17];
#ifdef DEBUG_LORA
                    debugSerial.print("HMAC key: ");
                    MLutility::printBinary(pData, 4);
#endif // DEBUG_LORA
                    sendPacketResAuthResponse(pData, 4);
                }
                else if (nRet == CMD_RES_AUTH_TOKEN) {
#ifdef DEBUG_LORA
                    debugSerial.println("AUTH_TOKEN");
#endif // DEBUG_LORA
                }
            }
        }
    }
    return nRet;
}

/////////////////////////////////////////
// auth related: challenge-reponse
/////////////////////////////////////////
void MOSTLora::setKeyHMAC(const char *strKey)
{
    _keyHMAC = strKey;
}

// send REQ_AUTH_JOIN to gateway, then will receive REQ_AUTH_CHALLENGE
void MOSTLora::sendPacketReqAuthJoin()
{
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLReqAuthJoinPayloadGen();
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    delete pPayload;
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// hash
void generateHMAC(uint8_t *dataDst, const char *keySrc, uint8_t *dataSrc, int szDataSrc)
{
    SHA256 hash;
    
    hash.resetHMAC(keySrc, strlen(keySrc));
    hash.update(dataSrc, szDataSrc);
    hash.finalizeHMAC(keySrc, strlen(keySrc), dataDst, 16);
#ifdef DEBUG_LORA
    debugSerial.print("HMAC data: ");
    MLutility::printBinary(dataDst, 16);
#endif // DEBUG_LORA
}

// received REQ_AUTH_CHALLENGE, then RES_AUTH_RESPONSE
void MOSTLora::sendPacketResAuthResponse(uint8_t *data, int szData)
{
    uint8_t dataHMAC[16] = {0};
    generateHMAC(dataHMAC, _keyHMAC, data, szData);
    
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLResAuthResponsePayloadGen(dataHMAC);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    delete pPayload;
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

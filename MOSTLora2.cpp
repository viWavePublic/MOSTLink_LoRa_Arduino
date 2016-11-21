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

//////////////////////////////////////////////////////////////////////////////////

void MOSTLora::run()
{
    if (available()) {
        receData();
    }
}

void MOSTLora::setCallbackReceData(CALLBACK_ReceData cbFunc)
{
    _cbReceData = cbFunc;
}

void MOSTLora::setCallbackPacketReqData(CALLBACK_ReceData cbFunc)
{
    _cbPacketReqData = cbFunc;
}
void MOSTLora::setCallbackPacketReqAuthChallenge(CALLBACK_ReceData cbFunc)
{
    _cbPacketReqAuthChallenge = cbFunc;
}
void MOSTLora::setCallbackParseMOSTLink(CALLBACK_ParseCommand cbFunc)
{
    _cbParseMOSTLink = cbFunc;
}

/////////////////////////////////////////////////////////////
// 1: set normal mode, 2: send data 3: recover original mode
int MOSTLora::sendPacket(byte *pPacket, int szPacket)
{
    int nModeBackup = getMode();
    setMode(E_LORA_NORMAL);
    
    // AES/CBC encrypt
    boolean bUseAES = false;
    if (bUseAES) {
        pPacket[4] |= 0x10;    // add AES flag
        int szAES = ((szPacket - 5) + 15) / 16 * 16;    // 16 alignment
        MLutility::encryptAES_CBC(pPacket, szAES, _keyAES, _ivAES);
    }
    /////////////////////
    // send data is ready
    int nRet = sendData(pPacket, szPacket);
    
    setMode(nModeBackup);
    return nRet;
}

/////////////////////////////////////////////////////////////

int MOSTLora::parsePacket()
{
    int szRet = -1;
    if (_buf[0] == '$') {     // for LT200 protocol
        
    }
    else if (_buf[0] == 0xFB && _buf[1] == 0xFC) {    // for MOST Link protocol
        MLPacketParser pkParser;
        MLPacketGen pkGen;
        szRet = _buf[3];        // packet size
        uint8_t flag = _buf[4]; // packet flag

        if (flag & 0x10) {      // AES128 decrypt
            _buf[4] &= 0xEF;    // clear AES flag
            int szAES = ((szRet - 5) + 15) / 16 * 16;    // 16 alignment
            MLutility::decryptAES_CBC(_buf + 5, szAES, _keyAES, _ivAES);
#ifdef DEBUG_LORA
            debugSerial.print(F("Decrypt: AES="));
            debugSerial.print(szAES, DEC);
            MLutility::printBinary(_buf, szRet);
#endif // DEBUG_LORA
            szRet = szAES + 5;  // packet with AES
        }
        
        // packet header
        int nResult = pkParser.mostloraPacketParse(&pkGen, _buf);
        if (nResult == 0) {     // packet CRC correct
            const byte *pNodeID = pkGen.getID();
            const uint16_t cmdID = pkGen.getMLPayload()->getCmdId();
            
#ifdef DEBUG_LORA
            if (pkGen.getDirection() == 0)
                debugSerial.print(F("DnLink"));
            else
                debugSerial.print(F("UpLink"));
            
            debugSerial.print(F(": cmd("));
            debugSerial.print(cmdID, DEC);
            debugSerial.print(F(") 0x"));
            debugSerial.print(cmdID, HEX);
            
            debugSerial.print(F(", nodeID:"));
            MLutility::printBinary(pNodeID, 8);
#endif // DEBUG_LORA
            
            const boolean bForMe = (memcmp(pNodeID, _data.mac_addr, 8) == 0);
            // parse MOSTLink command
            if (_cbParseMOSTLink) {
                _cbParseMOSTLink(cmdID);
            }
            if (bForMe) // packet for me
            {
                if (cmdID == CMD_REQ_DATA) {
                    if (_cbPacketReqData) {
                        _cbPacketReqData(_buf + 20, _buf[19]);
                    }
                }
                else if (cmdID == CMD_REQ_AUTH_CHALLENGE) {
                    byte *pData = &_buf[17];
#ifdef DEBUG_LORA
                    debugSerial.print(F("HMAC key: "));
                    MLutility::printBinary(pData, 4);
#endif // DEBUG_LORA
                    if (_cbPacketReqAuthChallenge)
                        _cbPacketReqAuthChallenge(pData, 4);
                    else {
                        delay(500);
                        sendPacketResAuthResponse(pData, 4);
                    }
                }
                else if (cmdID == CMD_RES_AUTH_TOKEN) {
#ifdef DEBUG_LORA
                    debugSerial.println(F("AUTH_TOKEN"));
#endif // DEBUG_LORA
                }

            }
        }
        else {
#ifdef DEBUG_LORA
            debugSerial.println(F("ERROR: parse packet."));
#endif // DEBUG_LORA
        }
    }
    return szRet;
}

/////////////////////////////////////////
// auth related: challenge-reponse
/////////////////////////////////////////
void MOSTLora::setKeyHMAC(const char *strKey)
{
    _keyHMAC = strKey;
}

void MOSTLora::setKeyAES(const char *strKey)
{
    _keyAES = strKey;
}

void MOSTLora::setIvAES(const char *strIv)
{
    _ivAES = strIv;
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

// received REQ_AUTH_CHALLENGE, then RES_AUTH_RESPONSE
void MOSTLora::sendPacketResAuthResponse(uint8_t *data, int szData)
{
    uint8_t dataHMAC[16] = {0};
    MLutility::generateHMAC(dataHMAC, _keyHMAC, data, szData);
    
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLResAuthResponsePayloadGen(dataHMAC);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    delete pPayload;
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

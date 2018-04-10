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
#include "MLPacketGen.h"
#include "MLPacketGen2.h"
#include "MLPacketGen3.h"
#include "MLPacketParser.h"

//////////////////////////////////////////////////////////////////////////////////

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
void MOSTLora::setCallbackPacketNotifyMcsCommand(CALLBACK_ReceData cbFunc)
{
    _cbPacketNotifyMcsCommand = cbFunc;
}
void MOSTLora::setCallbackPacketNotifyMydevicesCommand(CALLBACK_ReceData cbFunc)
{
    _cbPacketNotifyMydevicesCommand = cbFunc;
}

void MOSTLora::setCallbackParseMOSTLink(CALLBACK_ParseCommand cbFunc)
{
    _cbParseMOSTLink = cbFunc;
}

/////////////////////////////////////////////////////////////
// 1: set normal mode, 2: send data 3: recover original mode
int MOSTLora::sendPacket(byte *pPacket, int szPacket)
{   
    // AES/CBC encrypt
    if (_bPacketAES) {
        pPacket[4] |= 0x10;    // add AES flag
        int szAES = ((szPacket - 5) + 15) / 16 * 16;    // 16 alignment
        MLutility::encryptAES_CBC(pPacket + 5, szAES, (byte*)_keyAES, (byte*)_ivAES);
        
        szPacket = szAES + 5;
    }
    /////////////////////
    // send data is ready
    int nRet = sendData(pPacket, szPacket);
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

#ifdef USE_LIB_CRYPTO_AES128
        if (flag & 0x10) {      // AES128 decrypt
            _buf[4] &= 0xEF;    // clear AES flag
            int szAES = ((szRet - 5) + 15) / 16 * 16;    // 16 alignment
            MLutility::decryptAES_CBC(_buf + 5, szAES, (byte*)_keyAES, (byte*)_ivAES);
#ifdef DEBUG_LORA
            debugSerial.print(F("Decrypt: AES="));
            debugSerial.print(szAES, DEC);
            MLutility::printBinary(_buf, szRet);
#endif // DEBUG_LORA
            szRet = szAES + 5;  // packet with AES
        }
#endif // USE_LIB_CRYPTO_AES128
        
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
            
            char strFmt[128] = {0};
            const boolean bForMe = (memcmp(pNodeID, _data.mac_addr, 8) == 0);
            // parse MOSTLink command
            if (_cbParseMOSTLink) {
                _cbParseMOSTLink(cmdID);
            }
            // uplink for gateway
            if (cmdID == CMD_REP_LOCATION) {
                debugSerial.println(F("CMD_REP_LOCATION"));
                MLReportLocationPayloadGen *pPayload = (MLReportLocationPayloadGen*)pkGen.getMLPayload();
                
                ///////////////////////////
                // output for debug string
                char strLat[16], strLng[16];
                dtostrf(pPayload->getLat(), 8, 6, strLat);
                dtostrf(pPayload->getLng(), 8, 6, strLng);
                sprintf(strFmt, "(%s, %s), tReport=%d, tGPS=%d, battery=%d, timeUX:%ld ======", \
                        strLat, strLng, pPayload->getTypeReport(), pPayload->getTypeGPS(), (int)pPayload->getBatteryLevel(), pPayload->getDataTime());
                Serial.println(strFmt);
            }
            else if (cmdID == CMD_REP_BEACON) {
                debugSerial.println(F("CMD_REP_BEACON"));
                MLReportBeaconPayloadGen *pPayload = (MLReportBeaconPayloadGen*)pkGen.getMLPayload();
                ///////////////////////////
                // output for debug string
                debugSerial.print(F("UUID="));
                MLutility::printBinary(pPayload->getUuid(), 20);
                sprintf(strFmt, "tReport=%d, tBeacon=%d, rssi=%d, tx=%d, battery=%d", \
                        pPayload->getTypeReport(), pPayload->getTypeBeacon(), pPayload->getRssi(), pPayload->getTxpower(), pPayload->getBatteryLevel());
                debugSerial.println(strFmt);
            }
            else if (cmdID == CMD_REQ_ALARM_BEACON) {
                debugSerial.println(F("CMD_REQ_ALARM_BEACON"));
                MLReqAlarmBeaconPayloadGen *pPayload = (MLReqAlarmBeaconPayloadGen*)pkGen.getMLPayload();

            }
            else if (cmdID == CMD_REQ_ALARM_GPS) {
                debugSerial.println(F("CMD_REQ_ALARM_GPS"));
                MLAlarmGPSPayloadGen *pPayload = (MLAlarmGPSPayloadGen*)pkGen.getMLPayload();
                ///////////////////////////
                // output for debug string
                double fLat = pPayload->getLat();
                double fLng = pPayload->getLng();
                uint8_t batteryLvl = pPayload->getBatteryLevel();
                uint8_t gpsStatus = pPayload->getStatusGPS();
                uint32_t dateTime = pPayload->getDataTime();

                sprintf(strFmt, "battery=%d, GPS(%d)", batteryLvl, gpsStatus);
                debugSerial.println(strFmt);
            }
            
            // downlink
            if (bForMe) // packet for me
            {
                delay(100);    // delay to separate further send-data

                if (cmdID == CMD_REQ_DATA) {
                    if (_cbPacketReqData) {
                        MLReqDataPayloadGen *pPayload = (MLReqDataPayloadGen*)pkGen.getMLPayload();
                        
                        uint8_t nDataLen;
                        uint8_t *pData = pPayload->getData(nDataLen);
                        _cbPacketReqData(pData, nDataLen);
                    }
                }
                else if (cmdID == CMD_REQ_AUTH_CHALLENGE) {
                    MLReqAuthChallengePayloadGen *pPayload = (MLReqAuthChallengePayloadGen*)pkGen.getMLPayload();
                    uint8_t *pData = pPayload->getKeyHMAC();
#ifdef DEBUG_LORA
                    debugSerial.print(F("HMAC key: "));
                    MLutility::printBinary(pData, 4);
#endif // DEBUG_LORA
                    if (_cbPacketReqAuthChallenge)
                        _cbPacketReqAuthChallenge(pData, 4);
                    else {
                        delay(500);
                        sendPacketAnsAuthResponse(pData, 4);
                    }
                }
                else if (cmdID == CMD_RET_AUTH_TOKEN) {
#ifdef DEBUG_LORA
                    debugSerial.println(F("AUTH_TOKEN"));
#endif // DEBUG_LORA
                }
                else if (cmdID == CMD_NOTIFY_MCS_COMMAND) {
                    if (_cbPacketNotifyMcsCommand) {
                        MLNotifyMcsCommandPayloadGen *pPayload = (MLNotifyMcsCommandPayloadGen*)pkGen.getMLPayload();
                        _cbPacketNotifyMcsCommand(pPayload->getData(), pPayload->getDataLen());
                    }
                }
                else if (cmdID == CMD_NOTIFY_MYDEVICES_COMMAND) {
                    if (_cbPacketNotifyMydevicesCommand) {
                        MLNotifyMydevicesCommandPayloadGen *pPayload = (MLNotifyMydevicesCommandPayloadGen*)pkGen.getMLPayload();
                        _cbPacketNotifyMydevicesCommand(pPayload->getData(), pPayload->getDataLen());
                    }
                }
                else {
                    // other command
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
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// received REQ_AUTH_CHALLENGE, then ANS_AUTH_RESPONSE
void MOSTLora::sendPacketAnsAuthResponse(uint8_t *data, int szData)
{
    uint8_t dataHMAC[16] = {0};
    MLutility::generateHMAC(dataHMAC, _keyHMAC, data, szData);
    
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLAnsAuthResponsePayloadGen(dataHMAC);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

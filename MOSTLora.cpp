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

const char KEY_AES128[16] = {'1','2','3','4','5','6','7','8','9','0','A','B','C','D','E','F'};
const char IV_AES128[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

/////////////////////////////////////////

MOSTLora::MOSTLora(int pinP1, int pinP2, int pinBusy)
: LoraBase(pinP1, pinP2, pinBusy)
{
    _bPacketAES = false;
    setKeyHMAC("PublicKey");
    setKeyAES(KEY_AES128);
    setIvAES(IV_AES128);

    _cbReceData = NULL;
    _cbPacketReqData = NULL;
    _cbPacketReqAuthChallenge = NULL;
    _cbParseMOSTLink = NULL;
}

/////////////////////////////////////////

void MOSTLora::begin(long speed)
{
    // call parent-class function
    LoraBase::begin();
    
#ifdef DEBUG_LORA
  debugSerial.println(F("== MOSTLink v1.5.6 =="));
  debugSerial.print(F("CPU: "));
  debugSerial.println(F_CPU);
    
#endif // DEBUG_LORA

    setMode(E_LORA_SETUP);          // E_LORA_SETUP
    // read setting in lora shield
    int i;
    for (i = 0; i < 5; i++) {
        setMode(E_LORA_NORMAL);     // E_LORA_NORMAL
        if (readConfig()) {
            break;
        }
    }
    setMode(E_LORA_NORMAL);         // E_LORA_NORMAL
}

boolean MOSTLora::printConfig(DataLora &data)
{
  boolean bRet = false;
#ifdef DEBUG_LORA
  int i;
  if (!data.isValid()) {
    debugSerial.print(F("+++ invalid config"));
    return bRet;
  }
  bRet = true;
  debugSerial.print(F("*LoRa*"));
  debugSerial.write(data.module_no, 4);

  debugSerial.print(F(", Ver:"));
  debugSerial.write(data.ver_no, 7);

  debugSerial.print(F(", MAC:"));
  MLutility::printBinary(data.mac_addr, 8);

  debugSerial.print(F("  g_id:"));
  debugSerial.print(data.group_id, DEC);

  long nFrequency;
  nFrequency = ((long)data.freq[0] << 16) + ((long)data.freq[1] << 8) + data.freq[2];
  debugSerial.print(F(", freq:"));
  debugSerial.print(nFrequency);
  
  debugSerial.print(F(", datarate:"));
  debugSerial.print(data.data_rate, DEC);
  
  debugSerial.print(F(", pwr:"));
  debugSerial.print(data.power, DEC);

  debugSerial.print(F(", UART baud:"));
  debugSerial.print(data.uart_baud, DEC);
  debugSerial.print(F(", chk:"));
  debugSerial.print(data.uart_check, DEC);
  
  debugSerial.print(F("; wakeup time:"));
  debugSerial.println(data.wakeup_time, DEC);

#endif // DEBUG_LORA
  return bRet;
}

boolean MOSTLora::printInfo()
{
    boolean bRet = MOSTLora::printConfig(_data);
#ifdef DEBUG_LORA
    switch (_eMode) {
        case E_LORA_NORMAL:
            debugSerial.println(F("[Normal]"));
            break;
        case E_LORA_WAKEUP:
            debugSerial.println(F("[Wakeup]"));
            break;
        case E_LORA_POWERSAVING:
            debugSerial.println(F("[Pwr Saving]"));
            break;
        case E_LORA_SETUP:
            debugSerial.println(F("[Setup]"));
            break;
        default:
            debugSerial.println(F("[*Unknown*]"));
            break;
    }
#endif // DEBUG_LORA
    return bRet;
}

boolean MOSTLora::setReceiverID(const char *strID)
{
  if (strID == NULL || strlen(strID) != 16)
    return false;

  boolean bRet = true;
  char strHex[16];
  int i;
  for (i = 0; i < 16; i++) {
    if (!isHexadecimalDigit(strID[i])) {
      bRet = false;
      break;
    }
    strHex[i] = toupper(strID[i]);
  }
  if (bRet) {
    char strVal[4] = {0};
    for (i = 0; i < 8; i++) {
      strVal[0] = strHex[i * 2];
      strVal[1] = strHex[i * 2 + 1];
      
      _receiverID[i] = strtoul (strVal, NULL, 16);
    }
  }
  
  return bRet;
}

/////////////////////////////////////////
// config setting
/////////////////////////////////////////
boolean MOSTLora::readConfig()
{
  setMode(E_LORA_SETUP);    // setup(1,1), normal(0,0)
  
  uint8_t cmdRead[] = {0xFF,0x4C,0xCF,0x52,0xA1,0x52,0xF0};
  sendData(cmdRead, 7);

  // receive setting
  return receConfig(_data);
}

boolean MOSTLora::writeConfig(long freq, unsigned char group_id, char data_rate, char power, char wakeup_time)
{
  waitUntilReady(5000);
  setMode(E_LORA_SETUP);    // setup(1,1), normal(0,0)
  
  uint8_t cmdWrite[16] = {0xFF,0x4C,0xCF,0x52,0xA1,0x57,0xF1};
  cmdWrite[7] = (freq >> 16) & 0xff;
  cmdWrite[8] = (freq >> 8) & 0xff;
  cmdWrite[9] = freq & 0xff;
  cmdWrite[10] = group_id;
  cmdWrite[11] = data_rate;
  cmdWrite[12] = power;
  cmdWrite[13] = 3;               // uart baud
  cmdWrite[14] = 0;               // uart check
  cmdWrite[15] = wakeup_time;     // wakeup time
  
  sendData(cmdWrite, 16);
  
  // receive setting
  return receConfig(_data);
}

boolean MOSTLora::receConfig(DataLora &data)
{
  delay(300);
    
  boolean bRet = false;
  int szData = sizeof(DataLora);
  int szRece = receData();
  if (szData <= szRece) {
      memcpy(&data, _buf, szData);
      if (data.isValid()) {
        bRet = true;
        printConfig(data);
      }
  }
#ifdef DEBUG_LORA
  if (!bRet) {
    debugSerial.print(szRece);
    debugSerial.println(F(") - ERR: rece config!"));
  }
#endif // DEBUG_LORA
  return bRet;
}


/////////////////////////////////////////
// receive data via LoRa
int MOSTLora::receData()
{
    if (LoraBase::receData() > 0) {
      
        if (_buf[0] == '/') {  // ACK message (as Echo)
            char *strBuf = (char*)_buf;
            strBuf[0] = '>';
            sendData((uint8_t*)_buf, _szBuf);
        }
        // parse downlink packet
        if (_cbReceData) {
            _cbReceData(_buf, _szBuf);
        }
        int szParse = parsePacket();  // parse Packet by your code
    }

    return _szBuf;
}

// ANS_DATA command for humidity & temperature
void MOSTLora::sendPacketAnsData(float h, float t)
{
    byte dataHT[8];

    memcpy(dataHT, &h, 4);          // humidity (4 bytes)
    memcpy(dataHT + 4, &t, 4);      // temperature (4 bytes)
    
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLAnsDataPayloadGen(0, 8, dataHT);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// REQ_SOS (Uplink) for request SOS
void MOSTLora::sendPacketReqSOS(long datetime, char statusGPS, double lat, double lng, char battery)
{
    int szPacket = 22 + 19;
    MLUplink headUplink(0x0A, szPacket, 0x08, getMacAddress(), _receiverID);
    
    // prapare payload chunk
    byte payload[19];
    payload[0] = 0x0A;    // version
    payload[1] = 0x30;    payload[2] = 0x03;    // 0x0330 REQ_SOS commandID
    memcpy(payload + 3, &datetime, 4);          // datetime
    payload[7] = statusGPS;
    long outLat = lat * 1000000;
    long outLng = lng * 1000000;
    memcpy(payload + 8, &outLng, 4);
    memcpy(payload + 12, &outLat, 4);
    payload[16] = battery;
    
    payload[17] = 0;      // option flag

    // fill packet: header and payload
    memcpy(_buf, &headUplink, 22);
    memcpy(_buf + 22, payload, 19);
    _buf[szPacket - 1] =  getCrc(_buf, szPacket - 1);      // packet CRC
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, szPacket);
}

// RES_SOS (Downlink) for response SOS
void MOSTLora::sendPacketAnsSOS()
{
    int szPacket = 14 + 13;
    MLDownlink headDownlink(0x0A, szPacket, 0, _receiverID);
    
    // prapare payload chunk
    byte payload[13];
    payload[0] = 0x0A;    // version
    payload[1] = 0x84;    payload[2] = 0x03;    // 0x0384 RES_SOS commandID
    memcpy(payload + 3, getMacAddress(), 8);

    payload[11] = 0;      // option flag
    
    // fill packet: header and payload
    memcpy(_buf, &headDownlink, 14);
    memcpy(_buf + 14, payload, 13);
    _buf[szPacket - 1] =  getCrc(_buf, szPacket - 1);      // packet CRC
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, szPacket);
}

/////////////////////////////////////////

void MOSTLora::sendPacketNotifyLocation(unsigned long date_time, unsigned long lat, unsigned long lng)
{
    mllocation loc;
    loc.latitude = lat;
    loc.longtitude = lng;
    
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLNotifyLocationGen(date_time, loc, 0, 1);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

void MOSTLora::sendPacketVinduino(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLNotifyVindunoPayloadGen((uint8_t*)apiKey, f0, f1, f2, f3, f4, f5, f6, f7);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// NTF_UPLOAD_THINKSPEAK_FIELD command for thingSpeak project
void MOSTLora::sendPacketThingSpeak(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = new MLNotifyThingspeakPayloadGen((uint8_t*)apiKey, f0, f1, f2, f3, f4, f5, f6, f7);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}


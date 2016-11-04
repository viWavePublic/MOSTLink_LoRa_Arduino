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

#if defined(__LINKIT_ONE__)
    #define loraSerial Serial1       // for LinkIt ONE

#else // __LINKIT_ONE__
    #ifdef DEBUG_LORA

        // for arduino Uno
        #include <SoftwareSerial.h>
        const int pinLoraRX = 10;
        const int pinLoraTX = 11;
//      pinMode(pinLoraRX, INPUT);
//      pinMode(pinLoraTX, OUTPUT);
        SoftwareSerial loraSerial(pinLoraRX, pinLoraTX);    // RX, TX

    #else // DEBUG_LORA

        #define loraSerial Serial       // for Vinduino

    #endif // DEBUG_LORA

#endif // __LINKIT_ONE__


MOSTLora::MOSTLora(int pinP1, int pinP2, int pinBusy)
{
    _pinP1 = pinP1;
    _pinP2 = pinP2;
    _pinBZ = pinBusy;
    _eMode = E_UNKNOWN_LORA_MODE;
}
/*
void blink13()
{
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    int i;
    for (i = 0; i < 5; i++) {
        delay(100);
        digitalWrite(13, LOW);
        delay(100);
        digitalWrite(13, HIGH);
    }
}
*/
void MOSTLora::begin()
{
#ifdef DEBUG_LORA
  debugSerial.println("=== MOSTLink LoRa v1.0 ===");
  debugSerial.print("CPU clock: ");
  debugSerial.println(F_CPU);
    
#endif // DEBUG_LORA
    
  pinMode(_pinP1, OUTPUT);
  pinMode(_pinP2, OUTPUT);
  pinMode(_pinBZ, INPUT);

  loraSerial.begin(9600);

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

void MOSTLora::setMode(int mode)
{
  if (_eMode == mode)
    return;

  if (E_LORA_NORMAL == mode)
    setMode(0, 0);
  else if (E_LORA_WAKEUP == mode)
    setMode(0, 1);
  else if (E_LORA_POWERSAVING == mode) {
    if (E_LORA_SETUP == _eMode) {       // Setup -> Normal -> Power Saving
      setMode(0, 0);
    }
    setMode(1, 0);
  }
  else if (E_LORA_SETUP == mode) {
    if (E_LORA_POWERSAVING == _eMode) { // Power Saving -> Normal -> Setup
        setMode(0, 0);
    }
    setMode(1, 1);
  }
  
  // assign to new state
  _eMode = mode;
}

// setup(1,1), normal(0,0), wakeup(0,1), powersaving(1,0)
void MOSTLora::setMode(int p1, int p2)
{
  digitalWrite(_pinP1, p1);  // setup(1,1), normal(0,0)
  digitalWrite(_pinP2, p2);
  delay(200);
}

boolean MOSTLora::available()
{
  return loraSerial.available();
}

boolean MOSTLora::printConfig(DataLora &data)
{
  boolean bRet = false;
#ifdef DEBUG_LORA
  int i;
  if (!data.isValid()) {
    debugSerial.print("++++++ invalid config");
    return bRet;
  }
  bRet = true;
  debugSerial.print("*** Module:");
  debugSerial.write(data.module_no, 4);

  debugSerial.print(", Version:");
  debugSerial.write(data.ver_no, 7);

  debugSerial.print(", MAC:");
  MLutility::printBinary(data.mac_addr, 8);

  debugSerial.print("    group:");
  debugSerial.print(data.group_id, DEC);

  long nFrequency;
  nFrequency = ((long)data.freq[0] << 16) + ((long)data.freq[1] << 8) + data.freq[2];
  debugSerial.print(", frequency:");
  debugSerial.print(nFrequency);
  
  debugSerial.print(", data rate:");
  debugSerial.print(data.data_rate, DEC);
  
  debugSerial.print(", power:");
  debugSerial.print(data.power, DEC);

  debugSerial.print(", \n    uart baud:");
  debugSerial.print(data.uart_baud, DEC);
  debugSerial.print(", uart check:");
  debugSerial.print(data.uart_check, DEC);
  
  debugSerial.print(", wakeup time:");
  debugSerial.print(data.wakeup_time, DEC);

  char *pData = (char*)&data;
  debugSerial.println("\n");
#endif // DEBUG_LORA
  return bRet;
}

boolean MOSTLora::printInfo()
{
    boolean bRet = MOSTLora::printConfig(_data);
    const char *strMode = "* Unknown Mode *";
    switch (_eMode) {
        case E_LORA_NORMAL:
            strMode = "[ Normal Mode ]";
            break;
        case E_LORA_WAKEUP:
            strMode = "[ Wakeup Mode ]";
            break;
        case E_LORA_POWERSAVING:
            strMode = "[ Power Saving Mode ]";
            break;
        case E_LORA_SETUP:
            strMode = "[ Setup Mode ]";
            break;
    }
#ifdef DEBUG_LORA
    debugSerial.println(strMode);
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

/////////////////////////
// config setting 
/////////////////////////
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
  
  byte cmdWrite[16] = {0xFF,0x4C,0xCF,0x52,0xA1,0x57,0xF1};
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
    debugSerial.println(") ------ Fail to get config!");
  }
#endif // DEBUG_LORA
  return bRet;
}

int MOSTLora::sendData(const char *strData)
{
  waitUntilReady(3000);
  int nRet = loraSerial.print(strData);
  delay(100);
#ifdef DEBUG_LORA
  debugSerial.print(nRet);
  debugSerial.print(") Send String >>> ");
  debugSerial.println(strData);
#endif // DEBUG_LORA
  return nRet;
}

int MOSTLora::sendData(byte *data, int szData)
{
  waitUntilReady(3000);
  int nRet = loraSerial.write(data, szData);
  delay(100);
#ifdef DEBUG_LORA
  debugSerial.print("Send >>> ");
  MLutility::printBinary(data, szData);
#endif // DEBUG_LORA
  return nRet;
}

int MOSTLora::receData()
{
  _szBuf = 0;
  int nRssi = 0;
  if (!loraSerial.available())
    return 0;
  int i;
  for (i = 0; i < 6; i++) {
      int nCharRead = 0;
      while (loraSerial.available() && (_szBuf < MAX_SIZE_BUF)) {
        int c = loraSerial.read();
        _buf[_szBuf] = c;

        _szBuf++;
        nCharRead++;
        delay(1);
      }

      delay(300);
      if (nCharRead > 0) {
#ifdef DEBUG_LORA
          debugSerial.print(nCharRead);
          debugSerial.print(") ");
#endif // DEBUG_LORA
          if (E_LORA_WAKEUP == _eMode) {      // get RSSI at last character
              _szBuf--;
#ifdef DEBUG_LORA
              nRssi = _buf[_szBuf];
              debugSerial.print(nRssi);
              debugSerial.print(" rssi. ");
#endif // DEBUG_LORA
          }
      }
  }
  if (_szBuf > 0) {
    _buf[_szBuf] = 0;
      
#ifdef DEBUG_LORA
    debugSerial.print("\nRece <<< ");
    MLutility::printBinary(_buf, _szBuf);
    debugSerial.println((char*)_buf);
#endif // DEBUG_LORA
      
      if (_buf[0] == '/') {  // ack message
          char *strBuf = (char*)_buf;
          strBuf[0] = '>';
          sendData((byte*)_buf, _szBuf);
#ifdef DEBUG_LORA
          debugSerial.println("--- Echo ---");
#endif // DEBUG_LORA
      }
      // parse downlink packet
//      parsePacket(_buf, szData);  // parse Packet by your code
  }

  return _szBuf;
}

// 1: set normal mode, 2: send data 3: recover original mode
int MOSTLora::sendPacket(byte *pPacket, int szPacket)
{
    int nModeBackup = getMode();
    setMode(E_LORA_NORMAL);
    /////////////////////
    // send data is ready
    int nRet = sendData(pPacket, szPacket);
    
    setMode(nModeBackup);
    return nRet;
}

boolean MOSTLora::isBusy()
{
    boolean bRet = true;
    int nBusy = 0;
    
    if (_pinBZ >= 14) {     // analog BZ
        nBusy = analogRead(_pinBZ);
        bRet = (nBusy < 512);
    }
    else {                  // digital BZ
        const int nBusy = digitalRead(_pinBZ);
        bRet = (nBusy < 1);
    }
  
/*    if (bRet) {
#ifdef DEBUG_LORA
       const char *strBZ = " busy ...";
        debugSerial.print(nBusy, DEC);
        debugSerial.println(strBZ);
#endif // DEBUG_LORA
    }
  */
    return bRet;
}

boolean MOSTLora::waitUntilReady(unsigned long timeout)
{
  boolean bRet = true;
  unsigned long tsStart = millis();
  while (isBusy()) {
    delay(100);
    if (timeout < millis() - tsStart) {
      bRet = false;
      break;
    }
  }
#ifdef DEBUG_LORA
  debugSerial.print((millis() - tsStart));
  debugSerial.println(" Ready");
#endif // DEBUG_LORA
  return bRet;
}

// RES_DATA command for humidity & temperature
void MOSTLora::sendPacketResData(float h, float t)
{
    byte dataHT[8];

    memcpy(dataHT, &h, 4);          // humidity (4 bytes)
    memcpy(dataHT + 4, &t, 4);      // temperature (4 bytes)
    
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = MLPayloadGen::createResDataPayloadGen(0, 8, dataHT, 0, NULL);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    delete pPayload;
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// RES_DATA command for humidity & temperature
void MOSTLora::sendPacketResData_old(float h, float t)
{
    int szPacket = 22 + 15;
    MLUplink headUplink(0x0A, 22 + 15, 0x08, getMacAddress(), _receiverID);
    
    // prapare payload chunk
    byte payload[15];
    payload[0] = 0x0A;    // version
    payload[1] = 0x02;    payload[2] = 0x02;  // 0x0202 RES_DATA commandID
    payload[3] = 0;       // error code: 0 - success
    payload[4] = 8;       // data length
    // humidity (4 bytes)
    memcpy(payload + 5, &h, 4);
    
    // temperature (4 bytes)
    memcpy(payload + 9, &t, 4);
    
    payload[13] = 0;      // option flag
    
    // fill packet: header and payload
    memcpy(_buf, &headUplink, 22);
    memcpy(_buf + 22, payload, 15);
    _buf[szPacket - 1] =  getCrc(_buf, szPacket - 1);      // packet CRC
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, 37);
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
void MOSTLora::sendPacketResSOS()
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
    MLPayloadGen *pPayload = MLPayloadGen::createNotifyLocationGen(date_time, loc, 0, 1, 0, NULL);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    delete pPayload;
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

void MOSTLora::sendPacketVinduino(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    uint8_t pReceiverID[8] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
    MLPacketGen mlPacketGen(0,0,0,1,getMacAddress());
    MLPayloadGen *pPayload = MLPayloadGen::createNotifyVindunoPayloadGen((unsigned char*)apiKey, f0, f1, f2, f3, f4, f5, f6, f7, 0, NULL);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(_buf);
    
    delete pPayload;
    /////////////////////
    // send packet is ready
    sendPacket(_buf, packetLen);
}

// NTF_UPLOAD_VINDUINO_FIELD command for Vinduino project
void MOSTLora::sendPacketVinduino_old(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    int szPacket = 22 + 53;
    MLUplink headUplink(0x0A, szPacket, 0, getMacAddress(), _receiverID);
    
    // prapare payload chunk
    const float arrF[8] = {f0, f1, f2, f3, f4, f5, f6, f7};
    byte payload[53], *ptr;
    payload[0] = 0x0A;    // version
    // 0x1002 NTF_UPLOAD_VINDUINO_FIELD commandID
    payload[1] = 0x02;    payload[2] = 0x10;
    memcpy(payload + 3, apiKey, 16);
    
    // 8 floats (4 bytes)
    int i, index = 3 + 16;
    for (i = 0; i < 8; i++) {
        ptr = (byte*)(arrF + i);
        memcpy(payload + index, ptr, 4);
        index += 4;
    }
    
    payload[index] = 0;      // option flag
    payload[index + 1] = 0;  // payload CRC
    
    // fill packet: header and payload
    memcpy(_buf, &headUplink, 22);
    memcpy(_buf + 22, payload, 53);
    _buf[szPacket - 1] =  getCrc(_buf, szPacket - 1);      // packet CRC
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, szPacket);
}

// NTF_UPLOAD_VINDUINO_FIELD command for Vinduino project
void MOSTLora::sendPacketThingSpeak(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    int szPacket = 22 + 53;
    MLUplink headUplink(0x0A, szPacket, 0, getMacAddress(), _receiverID);
    
    // prapare payload chunk
    const float arrF[8] = {f0, f1, f2, f3, f4, f5, f6, f7};
    byte payload[53], *ptr;
    payload[0] = 0x0A;    // version
    // 0x02A1 NTF_UPLOAD_THINKSPEAK_FIELD commandID
    payload[1] = 0xA1;    payload[2] = 0x02;
    memcpy(payload + 3, apiKey, 16);
    
    // 8 floats (4 bytes)
    int i, index = 3 + 16;
    for (i = 0; i < 8; i++) {
        ptr = (byte*)(arrF + i);
        memcpy(payload + index, ptr, 4);
        index += 4;
    }
    
    payload[index] = 0;      // option flag
    payload[index + 1] = 0;  // payload CRC
    
    // fill packet: header and payload
    memcpy(_buf, &headUplink, 22);
    memcpy(_buf + 22, payload, 53);
    _buf[szPacket - 1] =  getCrc(_buf, szPacket - 1);      // packet CRC
    
    /////////////////////
    // send packet is ready
    sendPacket(_buf, szPacket);
}


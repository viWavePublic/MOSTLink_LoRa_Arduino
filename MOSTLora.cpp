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
#include "MLpacket.h"
#include "MLPacketGen.h"
#include "MLPacketParser.h"

#if defined(__LINKIT_ONE__)
    #define loraSerial Serial1       // for LinkIt ONE

#else // __LINKIT_ONE__
    #ifdef DEBUG_LORA

        // for arduino Uno
        #include <SoftwareSerial.h>
        const int pinLoraRX = 10;
        const int pinLoraTX = 11;
        SoftwareSerial loraSerial(pinLoraRX, pinLoraTX);    // RX, TX
//    pinMode(pinLoraRX, INPUT);
//    pinMode(pinLoraTX, OUTPUT);
    #else // DEBUG_LORA

        #define loraSerial Serial       // for Vinduino

    #endif // DEBUG_LORA

#endif // __LINKIT_ONE__

#ifdef DEBUG_LORA
#define debugSerial Serial
#else
// not ready yet
class DummySerial {
    
};
//#define debugSerial DummySerial

#endif

#ifdef USE_PIN_LED_LORA
const int pinLedLora = USE_PIN_LED_LORA;
#endif // USE_PIN_LED_LORA

MOSTLora::MOSTLora(int pinP1, int pinP2, int pinBusy)
{
    _pinP1 = pinP1;
    _pinP2 = pinP2;
    _pinBZ = pinBusy;
    _eMode = E_UNKNOWN_LORA_MODE;
}

void MOSTLora::begin()
{
#ifdef DEBUG_LORA
  debugSerial.println("=== MOSTLink LoRa v1.0 ===");
  debugSerial.print("CPU clock: ");
  debugSerial.println(F_CPU);
#endif // DEBUG_LORA
    
#ifdef USE_PIN_LED_LORA
  pinMode(pinLedLora, OUTPUT);
#endif // USE_PIN_LED_LORA
    
  pinMode(_pinP1, OUTPUT);
  pinMode(_pinP2, OUTPUT);
  pinMode(_pinBZ, INPUT);

  loraSerial.begin(9600);

  readConfig();
    
  setMode(E_LORA_NORMAL);   // E_LORA_NORMAL
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

void MOSTLora::printBinary(const byte *data, const int szData)
{
#ifdef DEBUG_LORA
  int i;
  for (i = 0; i < szData; i++) {
    if (data[i] < 16)
      debugSerial.print("0"); 
    
    debugSerial.print(data[i], HEX); 
  }
  debugSerial.print(" (");
  debugSerial.print(szData, DEC);
  debugSerial.print(" bytes)\n");
#endif // DEBUG_LORA
}

boolean MOSTLora::printConfig(DataLora &data)
{
  boolean bRet = false;
#ifdef DEBUG_LORA
  int i;
  if (data.tagBegin != 0x24 || data.tagEnd != 0x21) {
    debugSerial.print("++++++ incorrect config");
  }
  bRet = true;
  debugSerial.print("*** Module:");
  debugSerial.write(data.module_no, 4);

  debugSerial.print(", Version:");
  debugSerial.write(data.ver_no, 7);

  debugSerial.print(", MAC:");
  for (i = 0; i < 8; i++) {
    debugSerial.print(data.mac_addr[i], HEX);
  }

  debugSerial.print("\n    group:");
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
    char *strMode = "* Unknown Mode *";
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
void MOSTLora::readConfig()
{
  waitUntilReady(5000);
  setMode(E_LORA_SETUP);    // setup(1,1), normal(0,0)
  
  uint8_t cmdRead[] = {0xFF,0x4C,0xCF,0x52,0xA1,0x52,0xF0};
  sendData(cmdRead, 7);

  // receive setting
  delay(1000);
  receConfig(_data);
}

void MOSTLora::writeConfig(long freq, unsigned char group_id, char data_rate, char power, char wakeup_time)
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
  delay(2000);
  receConfig(_data);  
}

boolean MOSTLora::receConfig(DataLora &data)
{
  boolean bRet = false;
  int szData = sizeof(DataLora);
  int szRece = receData((uint8_t*)&data, szData);
  if (szData == szRece) {
    bRet = true;
    printConfig(data);
  }
  else {
#ifdef DEBUG_LORA
    debugSerial.print(szRece);
    debugSerial.println(") ------ Fail to get config!");
#endif // DEBUG_LORA
  }
  return bRet;
}

int MOSTLora::sendData(char *strData)
{
  waitUntilReady(3000);
  int nRet = loraSerial.print(strData);
  delay(100);
#ifdef DEBUG_LORA
  debugSerial.print(nRet);
  debugSerial.print(") Send String: ");
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
  debugSerial.print(nRet);
  debugSerial.print(") Send: ");
  printBinary(data, szData);
#endif // DEBUG_LORA
  return nRet;
}

int MOSTLora::receData(byte *data, int szData)
{
  int nRssi = 0;
  if (!loraSerial.available())
    return 0;
  int i, nCountBuf = 0;
//  while (loraSerial.available() && (nCountBuf < szData)) {
  for (i = 0; i < 6; i++) {
      int nCharRead = 0;
      while (loraSerial.available() && (nCountBuf < szData)) {
        if (0 == nCountBuf) {
#ifdef USE_PIN_LED_LORA
          digitalWrite(pinLedLora, HIGH);   // turn the LED on (HIGH is the voltage level)
#endif // USE_PIN_LED_LORA
        }
       
        int c = loraSerial.read();
        data[nCountBuf] = c;

        nCountBuf++;
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
              nCountBuf--;
#ifdef DEBUG_LORA
              nRssi = data[nCountBuf];
              debugSerial.print(nRssi);
              debugSerial.print(" rssi. ");
#endif // DEBUG_LORA
          }
      }
  }
  if (nCountBuf > 0) {
    data[nCountBuf] = 0;
#ifdef USE_PIN_LED_LORA
    digitalWrite(pinLedLora, LOW);    // turn the LED off by making the voltage LOW
#endif // USE_PIN_LED_LORA
      
#ifdef DEBUG_LORA
    debugSerial.print("\nRece: ");
    printBinary(data, nCountBuf);
    debugSerial.println((char*)data);
#endif // DEBUG_LORA

  }
  // parse downlink packet
//  parsePacket(data, szData);  // parse Packet by your code
  return nCountBuf;
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


int MOSTLora::parsePacket(byte *data, int szData)
{
  int nRet = -1;
  if (data[0] == '$') {     // for LT200 protocol
    
  }
  else if (data[0] == 0xFB && data[1] == 0xFC) {    // for MOST Link protocol
    // downlink header
      MLDownlink header;
      const int szHeader = sizeof(MLDownlink);
      if (szData > szHeader) {
          MLPacketCtx pkctx;
          MLPacketParser pkParser;
          
          pkParser.mostloraPacketParse(&pkctx, data);
          byte *pMac = (byte*)&pkctx._id;
          
#ifdef DEBUG_LORA
          debugSerial.print("*** cmdID: ");
          debugSerial.print((int)pkctx._mlPayloadCtx._cmdId, 10);
          debugSerial.print(", length: ");
          debugSerial.print((int)pkctx._mlPayloadCtx._dataLen, 10);

          debugSerial.print(") pkParser Mac:");
          printBinary(pMac, 8);
#endif // DEBUG_LORA
          
          memcpy(&header, data, szHeader);
          if (memcmp(header.receiver_id, _data.mac_addr, 8) == 0) // packet for me
          {
#ifdef DEBUG_LORA
              short cmd;
              memcpy(&cmd, data + 15, 2);//[15] + (data[16] << 8);
//              debugSerial.print("=== payload cmdID: 0x");
              debugSerial.println(cmd, 16);
#endif // DEBUG_LORA

              nRet = 0;
          }
      }
  }
  return nRet;
}

boolean MOSTLora::isBusy()
{
  int nBusy = analogRead(_pinBZ);
/*  debugSerial.print("busy:");
  debugSerial.print(nBusy, DEC);
  debugSerial.println(".");
  */
  return (nBusy < 512);
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
void MOSTLora::sendPacketResData2(float h, float t)
{
    byte dataHT[8], *ptr;
    // humidity (4 bytes)
    ptr = (byte*)&h;
    dataHT[0] = ptr[0];
    dataHT[1] = ptr[1];
    dataHT[2] = ptr[2];
    dataHT[3] = ptr[3];
    // temperature (4 bytes)
    ptr = (byte*)&t;
    dataHT[4] = ptr[0];
    dataHT[5] = ptr[1];
    dataHT[6] = ptr[2];
    dataHT[7] = ptr[3];
    
    uint8_t mlpacket[99];
    uint8_t pReceiverID[8] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
    uint8_t *pSenderID = (uint8_t*)getMacAddress();
    MLPacketGen mlPacketGen(0,0,0,1,pReceiverID);
    MLPayloadGen *pPayload = MLPayloadGen::createResDataPayloadGen(0, 8, dataHT, 0, NULL);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(mlpacket);
    
    /////////////////////
    // send packet is ready
    sendPacket(mlpacket, packetLen);
}

// RES_DATA command for humidity & temperature
void MOSTLora::sendPacketResData(float h, float t)
{
    byte buf[99];
    MLUplink headUplink(0x0A, 22 + 15, 0, getMacAddress(), _receiverID);
    
    // prapare payload chunk
    byte payload[15], *ptr;
    payload[0] = 0x0A;    // version
    payload[1] = 0x02;    payload[2] = 0x02;  // 0x0202 RES_DATA commandID
    payload[3] = 0;       // error code: 0 - success
    payload[4] = 8;       // data length
    // humidity (4 bytes)
    memcpy(payload + 5, &h, 4);
    
    // temperature (4 bytes)
    memcpy(payload + 9, &t, 4);
    
    payload[13] = 0;      // option flag
    payload[14] = 0;      // payload CRC
    
    // fill packet: header and payload
    memcpy(buf, &headUplink, 22);
    memcpy(buf + 22, payload, 15);
    
    /////////////////////
    // send packet is ready
    sendPacket(buf, 37);
}

void MOSTLora::sendPacketNotifyLocation(unsigned long date_time, unsigned long lat, unsigned long lng)
{
    byte buf[99];
    MLUplink headUplink(0x0A, 22 + 15, 0, getMacAddress(), _receiverID);
    
    // prapare payload chunk
    byte payload[15], *ptr;
    payload[0] = 0x0A;    // version
    payload[1] = 0x02;    payload[2] = 0x02;  // 0x0202 RES_DATA commandID
    payload[3] = 0;       // error code: 0 - success
    payload[4] = 8;       // data length
    // humidity (4 bytes)
    //    ptr = (byte*)&h;
    //    payload[5] = ptr[3];
    //    payload[6] = ptr[2];
    //    payload[7] = ptr[1];
    //    payload[8] = ptr[0];
    memcpy(payload + 5, &lat, 4);
    
    // temperature (4 bytes)
    //    ptr = (byte*)&t;
    //    payload[9] = ptr[3];
    //    payload[10] = ptr[2];
    //    payload[11] = ptr[1];
    //    payload[12] = ptr[0];
    memcpy(payload + 9, &lng, 4);
    
    payload[13] = 0;      // option flag
    payload[14] = 0;      // payload CRC
    
    // fill packet: header and payload
    memcpy(buf, &headUplink, 22);
    memcpy(buf + 22, payload, 15);
    
    /////////////////////
    // send packet is ready
    sendPacket(buf, 37);
}

void MOSTLora::sendPacketVinduino2(char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    uint8_t mlpacket[99];
    uint8_t pReceiverID[8] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
    uint8_t *pSenderID = (uint8_t*)getMacAddress();
    MLPacketGen mlPacketGen(0,0,0,1,pReceiverID);
    MLPayloadGen *pPayload = MLPayloadGen::createNotifyVindunoPayloadGen((unsigned char*)apiKey, f0, f1, f2, f3, f4, f5, f6, f7, 0, NULL);
    
    mlPacketGen.setMLPayloadGen(pPayload);
    uint8_t packetLen = mlPacketGen.getMLPacket(mlpacket);
    
    /////////////////////
    // send packet is ready
    sendPacket(mlpacket, packetLen);
}

// NTF_UPLOAD_VINDUINO_FIELD command for Vinduino project
void MOSTLora::sendPacketVinduino(char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    byte buf[99];
    MLUplink headUplink(0x0A, 22 + 53, 0, getMacAddress(), _receiverID);
    
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
    memcpy(buf, &headUplink, 22);
    memcpy(buf + 22, payload, 53);
    
    /////////////////////
    // send packet is ready
    sendPacket(buf, 75);
}



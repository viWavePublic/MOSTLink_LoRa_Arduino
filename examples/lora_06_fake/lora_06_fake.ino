//////////////////////////////////////////////////////
// This sample code is used for ShareCourse on MOSTLink protocol
//
//   Fake node to listen, and modify cache for fake issue
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLutility.h"
#include "MLPacketParser.h"

MOSTLora lora;

// data for listening broadcast message
struct DataUplink {
  byte _data[50];
  int _szData;
  DataUplink() { _szData = 0; };

  void setData(const byte *data, int szData) {
    if (szData > 0 && szData < 50) {
      _szData = szData;
      memcpy(_data, data, szData);
    }
  }
};
//const char KEY_AES128[16] = {'X','2','3','4','5','6','7','8','9','0','A','B','C','D','E','F'};

// cache uplink-buffer
DataUplink g_bufLora;
DataUplink g_bufResData;
DataUplink g_bufAuthJoin;
DataUplink g_bufAuthResponse;

// callback for rece data
void funcCustomRece(unsigned char *data, int szData)
{
  debugSerial.println(F("funcRece= cache latest message"));
  g_bufLora.setData(data, szData);        // #1 cache: Any Rece-Data
}

// callback for CMD_REQ_AUTH_CHALLENGE
void funcPacketReqAuthChallenge(unsigned char *data, int szData)
{
  debugSerial.println(F("ReqChallenge= "));
}

// callback for Parse MOSTLink Command
void funcParseMOSTLink(int cmdID)
{
  // store specify command ID
  const byte *data = g_bufLora._data;
  int szData = g_bufLora._szData;
  
  if (cmdID == CMD_RES_DATA)
  {
    float fTemp;
    g_bufResData.setData(data, szData);       // #2 cache: RES_DATA
    // modify temperature to add 10 degree
    memcpy(&fTemp, g_bufResData._data + 23, 4);
    fTemp += 10.0f;
    memcpy(g_bufResData._data + 23, &fTemp, 4);
  }
  else if (cmdID == CMD_REQ_AUTH_JOIN)
  {
    g_bufAuthJoin.setData(data, szData);      // #3 cache: AUTH_JOIN
  }
  else if (cmdID == CMD_RES_AUTH_RESPONSE)
  {
    g_bufAuthResponse.setData(data, szData);  // #4 cache: AUTH_RESPONSE
  }
}

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_NORMAL);         // E_LORA_NORMAL
//  lora.setKeyAES(KEY_AES128);

  delay(1000);
  lora.sendData("[Fake]");

  // custom callback
  lora.setCallbackReceData(funcCustomRece);
  lora.setCallbackParseMOSTLink(funcParseMOSTLink);
}

void loop() {
  lora.run();

  delay(100);

  // command to send
  if (Serial.available())
    inputBySerial();
}

// input serial for 
void inputBySerial()
{
  byte buf[256] = {0};
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 0) {
    if (buf[0] == '/')
    {
      if (buf[1] == '1') {
        lora.sendData(g_bufLora._data, g_bufLora._szData);
      }
      else if (buf[1] == '2') {
        lora.sendData(g_bufResData._data, g_bufResData._szData);
      }
      else if (buf[1] == '3') {   // send join
        lora.sendData(g_bufAuthJoin._data, g_bufAuthJoin._szData);
      }
      else if (buf[1] == '4') {   // send response
        lora.sendData(g_bufAuthResponse._data, g_bufAuthResponse._szData);
      }
      else if (buf[1] == '0') {
        Serial.println(F("cache: "));
        Serial.print(F("    Lora(1)= "));
        MLutility::printBinary(g_bufLora._data, g_bufLora._szData);
        Serial.print(F(" ResData(2)= "));
        MLutility::printBinary(g_bufResData._data, g_bufResData._szData);
        Serial.print(F("AuthJoin(3)= "));
        MLutility::printBinary(g_bufAuthJoin._data, g_bufAuthJoin._szData);
        Serial.print(F(" AuthRes(4)= "));
        MLutility::printBinary(g_bufAuthResponse._data, g_bufAuthResponse._szData);
      }
    }
  }  
}


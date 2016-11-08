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

int g_szBuf = 0;
byte g_bufLora[99] = {0};

// callback for rece data
void funcCustomRece(unsigned char *data, int szData)
{
  debugSerial.print("funcCustomRece= ");
  g_szBuf = szData;
  memcpy(g_bufLora, data, szData);
}

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_NORMAL);         // E_LORA_NORMAL

  delay(1000);
  lora.sendData("Hi, it's Fake.");

  // custom callback
  lora.setCallbackReceData(funcCustomRece);
}

void loop() {
  lora.run();

  delay(100);

  // command to send
  if (Serial.available())
    inputBySerial();
}

void inputBySerial()
{
  byte buf[256] = {0};
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 0) {
    if (buf[0] == '/')
    {
      if (buf[1] == '1') {
        lora.sendData(g_bufLora, g_szBuf);
      }
      else if (buf[1] == '2') {
        modifyResData(g_bufLora, g_szBuf);  
      }
      else if (buf[1] == '3') {   // send join
        modifyResData(g_bufLora, g_szBuf);  
      }
      else if (buf[1] == '4') {   // send response
        modifyResData(g_bufLora, g_szBuf);  
      }
      else if (buf[1] == '0') {
        Serial.print(" cache = ");
        MLutility::printBinary(g_bufLora, g_szBuf);
      }
    }
    else {
      buf[countBuf] = 0;
      Serial.print("chat> ");
      Serial.println((char*)buf);
      lora.sendData((char*)buf);
    }
  }  
}

int modifyResData(unsigned char *data, int szData)
{
  int nRet = -1;
  if (data[0] == 0xFB && data[1] == 0xFC) {    // for MOST Link protocol
      MLPacketCtx pkctx;
      MLPacketParser pkParser;
          
      int nResult = pkParser.mostloraPacketParse(&pkctx, data);
      if (nResult == 0) {
          byte *pMac = (byte*)&pkctx._id;
#ifdef DEBUG_LORA
          if (pkctx._direction == 0)
              debugSerial.print("*** downlink ");
          else
              debugSerial.print("*** uplink ");

          debugSerial.print("cmdID: ");
          debugSerial.print((int)pkctx._mlPayloadCtx._cmdId, 10);

          debugSerial.print(", pkParser Mac:");
          MLutility::printBinary(pMac, 8);
#endif // DEBUG_LORA
          
          nRet = pkctx._mlPayloadCtx._cmdId;
          if (nRet == CMD_REQ_DATA)
          {
            data[20] = 'f';
            data[21] = 'a';
            data[22] = 'k';
            data[23] = 'e';
          }
          else if (nRet == CMD_RES_DATA)
          {
            float fTemp;
            memcpy(&fTemp, data + 23, 4);
            fTemp += 10.0f;
            memcpy(data + 23, &fTemp, 4);
          }
      }
  }
}


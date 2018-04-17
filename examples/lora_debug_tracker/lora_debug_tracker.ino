//////////////////////////////////////////////////////
// LT-601 tracker
//
// command by "/" slash
//    1. "/f" set frequency: "/f915xxx"
//    2. "/s" REQ_NDCALL for LT-300
//    3. "/i" REQ_LOCATION one time: "/i30" response after 30 sec
//    4. "/j" REQ_LOCATION periodic: "/j60" periodic per 60 sec
//    5. "/0" stop help alarm
//    6. "/-" set device command
//            "/-D0=915123"
//            "/-O2=0" BLE enable; "/-O2=1" BLE disable
//            "/-CD=0" GPS enable; "/-CD=1" GPS disable
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLutility.h"

MOSTLora lora;

byte buf[100] = {0};

// callback for rece data
void funcCustomRece(unsigned char *data, int szData)
{ 
  debugSerial.print("funcCustomRece= ");
  MLutility::printBinary(data, szData);
}

// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  memcpy(buf, data, szData);  
  buf[szData] = 0;
  debugSerial.print("ReqData= ");
  debugSerial.println((const char*)buf);
}

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  Serial.println(F("*** lora_debug_tracker ***"));
  
  lora.begin();
  // config setting: frequency, group, data-rate, power, wakeup-time
//  lora.writeConfig(915600, 0, 0, 7, 5);
  lora.setMode(E_LORA_WAKEUP);         // E_LORA_WAKEUP

  delay(1000);
  lora.sendData("--- ready for debug LT601");

  // custom callback
  lora.setCallbackReceData(funcCustomRece);
  lora.setCallbackPacketReqData(funcPacketReqData);
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
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 0) {
    buf[countBuf] = 0;

    Serial.print("chat> ");
    Serial.println((char*)buf);
    if (buf[0] == '/')
    {
      char *strMac = "000DB53802703668";
      char bufSend[32] = {0};
      const char *strCmd = buf + 1;
      const char *strResult;
      if (('f' == strCmd[0]) || ('F' == strCmd[0]))  {
        const char *strFreq = buf + 2;
        long freq = atol(strFreq);
       
        Serial.print("set freq= ");
        Serial.println(freq);
        lora.writeConfig(freq, 0, 0, 7, 5);
        lora.setMode(E_LORA_WAKEUP);         // E_LORA_WAKEUP

      }
      else if (('s' == strCmd[0]) || ('S' == strCmd[0]))  {
        strMac = "9B667C110A001B80";
        MLutility::stringHexToBytes(bufSend, strMac, 16);

        Serial.println(F("---sendReqNDCall---"));
        lora.sendPacketReqNDCall(bufSend, 0x0c, 3, 10, 5);
      }
      else if (('t' == strCmd[0]) || ('T' == strCmd[0]))  {
        char *strHex = "FBFC0B15009B667C110A001B80180B31030C030036";
        MLutility::stringHexToBytes(bufSend, strHex, 42);

        Serial.println(F("---send-Preset---"));
        lora.sendData(bufSend, 21);
      }
      else if (('i' == strCmd[0]) || ('I' == strCmd[0]))  {
        MLutility::stringHexToBytes(bufSend, strMac, 16);

        int nDelay = 30;
        if (strCmd[1] != '\0') {
          nDelay = atoi(strCmd + 1);
        }
        
        MLutility::printTime(millis());
        Serial.print(F("---REQ_LOC: ONE TIME="));
        Serial.println(nDelay);
        // report type: 0x21 - one time: response location after delay time
        //              0x01 - one time: response location directly
        //              0x02 periodic
        lora.sendPacketReqLocation(bufSend, 0x21, 0x02, 0, nDelay);
      }
      else if (('j' == strCmd[0]) || ('J' == strCmd[0]))  {
        // command: "/j30" - periodic 30 sec
        MLutility::stringHexToBytes(bufSend, strMac, 16);

        int nPeriod = 30;
        if (strCmd[1] != '\0') {
          nPeriod = atoi(strCmd + 1);
        }
        
        MLutility::printTime(millis());
        Serial.print(F("---REQ_LOC: PERIODIC="));
        Serial.println(nPeriod);
        // report type: 0x21 - one time
        //              0x02 periodic
        lora.sendPacketReqLocation(bufSend, 0x02, 0x03, nPeriod, 20);
      }
      else if ('0' == strCmd[0])  {   // ans alarm to STOP tracker
        MLutility::stringHexToBytes(bufSend, strMac, 16);

        MLutility::printTime(millis());
        Serial.println(F("---sendAnsAlarm---"));
        // condition flag: 0x01 stop help
        //                 0x02 
        lora.sendPacketAnsAlarm(bufSend, 0x03);        
      }
      else if ('-' == strCmd[0])  {   // req gtr command
        MLutility::stringHexToBytes(bufSend, strMac, 16);
        char *cmdParam = strCmd + 1;
        if (strlen(cmdParam) > 0) {
          lora.sendPacketReqGtrCommand(bufSend, cmdParam);
        }
        else 
          lora.sendPacketReqGtrCommand(bufSend, "OD=1");
      }
    }
    else {
      lora.sendData((char*)buf);
      
    }
  }  
}

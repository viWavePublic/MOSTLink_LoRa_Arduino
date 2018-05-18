//////////////////////////////////////////////////////
// LT-601 tracker
//
// command by "/" slash
//    1. "/f" set frequency: "/f915xxx"
//    2. "/s" REQ_NDCALL for LT-300
//    3. "/i" REQ_LOCATION one time: "/i30" response after 30 sec
//    4. "/j" REQ_LOCATION periodic: "/j60" periodic per 60 sec
//    5. "/0" stop help alarm
//    6. "/7" standby mode: stop periodic
//    7. "/-" set device command, example below:
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
//  lora.setCallbackReceData(funcCustomRece);
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
      const char cCmd = tolower(strCmd[0]);
      if ('f' == cCmd)  {
        const char *strFreq = buf + 2;
        long freq = atol(strFreq);
       
        Serial.print("set freq= ");
        Serial.println(freq);
        lora.writeConfig(freq, 0, 0, 7, 5);
        lora.setMode(E_LORA_WAKEUP);         // E_LORA_WAKEUP
      }
      else if ('c' == cCmd)  {  // send REQ_SET_LORA_CONFIG to LT-300 (channel1)
        const char *strFreq = buf + 2;
        long freq = atol(strFreq);
        if (freq < 800000 || freq > 929000)
          freq = 915000;
          
        Serial.print("channel(1): set freq= ");
        Serial.println(freq);
                
        strMac = "9B667C110A001B80";
        MLutility::stringHexToBytes(bufSend, strMac, 16);        
        Serial.println(F("---sendSetLoraConfig---"));
        lora.sendPacketSetLoraConfig(bufSend, 0, freq, 0, 7, 5, 0);

        // ------ Update LoRa shield freq: same as LT-300
        delay(1500);
        lora.writeConfig(freq, 0, 0, 7, 5);
        lora.setMode(E_LORA_WAKEUP);         // E_LORA_WAKEUP        
      }
      else if ('d' == cCmd)  {  // send REQ_SET_LORA_CONFIG to LT-300 (channel2)
        const char *strFreq = buf + 2;
        long freq = atol(strFreq);
        if (freq < 800000 || freq > 999000)
          freq = 915600;

        Serial.print("channel(2): set freq= ");
        Serial.println(freq);          
                  
        strMac = "9B667C110A001B80";
        MLutility::stringHexToBytes(bufSend, strMac, 16);        
        Serial.println(F("---sendSetLoraConfig---"));
        lora.sendPacketSetLoraConfig(bufSend, 1, 915000, 0, 7, 5, 0);
      }
      else if ('s' == cCmd)  {  // send REQ_ND_CALL to tracker (LT-300)
        // 922222, 925555
        strMac = "9B667C110A001B80";
        MLutility::stringHexToBytes(bufSend, strMac, 16);

        Serial.println(F("---sendReqNDCall---"));
        lora.sendPacketReqNDCall(bufSend, 0x0c, 3, 10, 5);
      }
      else if ('+' == cCmd)  {  // send HEX by input string
        char *strHex = strCmd + 1;
        int nLen = strlen(strHex);
        MLutility::stringHexToBytes(bufSend, strHex, nLen);

        Serial.println(F("---send-HEX---"));
        lora.sendData(bufSend, nLen / 2);
      }
      
      else if ('t' == cCmd)  {    // send test preset
        // for test preset
//        char *strHex = "FBFC0B15009B667C110A001B80180B31030C030036";  // NDCall
        char *strHex = "FBFC0B2200000DB53802703668820C08000F4C322844303D393135303030290D0A37";  // SET device freq
        int nLen = strlen(strHex);
        MLutility::stringHexToBytes(bufSend, strHex, nLen);

        Serial.println(F("---send_TEST"));
        lora.sendData(bufSend, nLen / 2);
      }
      ////////////////////////////
      // LT-601
      ////////////////////////////
      else if ('a' == cCmd)  {   // reply ACK: 1=enable, 0=disable
        if (strCmd[1] == '0') {
          lora.setReplyACK(false);
          Serial.println(F("==replyACK Disable"));
        }
        else if (strCmd[1] == '1') {
          lora.setReplyACK(true);
          Serial.println(F("==replyACK Enable"));
        }
        else {
          MLutility::stringHexToBytes(bufSend, strMac, 16);
          lora.sendPacketACK(bufSend, true);
        }
      }
      else if ('i' == cCmd)  {  // one-time report location (30)
        // 915000, 915600
        MLutility::stringHexToBytes(bufSend, strMac, 16);

        int nDelay = 30;
        if (strCmd[1] != '\0') {
          nDelay = atoi(strCmd + 1);
        }
        
        Serial.print(F("---REQ_LOC: ONE TIME="));
        Serial.println(nDelay);
        // report type: 0x21 - one time: response location after delay time
        //              0x01 - one time: response location directly
        //              0x02 periodic
        if (nDelay <= 0)
          lora.sendPacketReqLocation(bufSend, 0x01, 0x03, 0, 45);
        else
          lora.sendPacketReqLocation(bufSend, 0x21, 0x03, 0, nDelay);
      }
      else if ('j' == cCmd)  {  // period report location
        // command: "/j30" - periodic 30 sec
        MLutility::stringHexToBytes(bufSend, strMac, 16);

        int nPeriod = 30;
        if (strCmd[1] != '\0') {
          nPeriod = atoi(strCmd + 1);
        }
        
        Serial.print(F("---REQ_LOC: PERIODIC="));
        Serial.println(nPeriod);
        // report type: 0x21 - one time
        //              0x02 periodic
        lora.sendPacketReqLocation(bufSend, 0x02, 0x03, nPeriod, 20);
      }
      else if ('0' == cCmd)  {   // ans alarm to STOP tracker
        MLutility::stringHexToBytes(bufSend, strMac, 16);

        Serial.println(F("---sendAnsAlarm---"));
        // condition flag: 0x01 stop help
        //                 0x02 
        lora.sendPacketAnsAlarm(bufSend, 0x03);
      }      
      else if ('7' == cCmd)  {   // req gtr command "M7
        MLutility::stringHexToBytes(bufSend, strMac, 16);
        lora.sendPacketReqGtrCommand(bufSend, "M7", false);
      }      
      else if ('-' == cCmd)  {   // req gtr command
        //////////////////////////////////
        // command sample:
        //   "M7" to standby mode (stop periodic report)
        //   "D0=915000" to change frequency
        //   "O2" DisableBLE:    0=enable, 1=disable
        //   "CD" disableGPS:    0=enable, 1=disable
        //   "C0" GPS always on: 0=disable, 1=enable
        //   "A1" wait ACK: 0=disable, 1=enable
        //   "A6" num of re-send

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
      Serial.println(F("** Undef command **"));
      lora.sendData((char*)buf);
      
    }
  }  
}

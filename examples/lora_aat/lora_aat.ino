//////////////////////////////////////////////////////
// LM110H1 module has 3 LoRa firmware mode
//    1. LoRaWAN (AT Command)
//    2. AAT-MOST (AT Command)
//    3. p1p2-MOST (to be deprecated)
//
// Serial Monitor command:
//    "/0" get current firmware mode
//    "/1" firmware switch to LoRaWAN
//    "/2" firmware switch to AAT-MOST
//    "/3" firmware switch to p1p2-MOST
//    "/11", "/22", "/33" for test firmware
//    "/a" change baud-rate to (9600)
//    "/b" change baud-rate to (57600)
//----------------------------------------------------
// AAT-MOST firmware:
//    "/t??????" send date "??????"
//    receive data would be parse as binary format
//
//////////////////////////////////////////////////////

#include "MOSTLoraAT.h"
#include "MLutility.h"

MOSTLoraAT lora;

byte buf[100] = {0};

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  Serial.println(F("*** lora_AAT ***"));

  lora.begin();
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
      const char *strCmd = buf + 1;
      const char *strResult;
      if (0 == strcmp(strCmd, "1")) {
        lora.setFirmwareMode(E_FW_AAT_LORAWAN);
      }
      else if (0 == strcmp(strCmd, "11")) {
        lora.command("AAT1 FwVersion");    // LoRaWAN firmware
      }    
      else if (0 == strcmp(strCmd, "2")) {
        lora.setFirmwareMode(E_FW_AAT_MOST);
      }
      else if (0 == strcmp(strCmd, "22")) {
        lora.setFirmwareMode(E_FW_AAT_MOST);
        lora.command("AAT1 L0=?");    // LoRa Device MAC
        lora.command("AAT1 L1=?");    // LoRa Device Name
        lora.command("AAT1 L2=?");    // LoRa Device FW Version
        lora.command("AAT1 L3=?");    // LoRa Channel Group ID
//        lora.command("AAT1 L4=?");    // LoRa Channel Rx / Tx Frequency
        lora.command("AAT1 L4=915555,915555");
        lora.command("AAT1 L5=0");    // DataRate: 0=0.81K, 1=1.46K, 2=2.6K, 3=4.56K, 4=9.11K, 5=18.23K bps
        lora.command("AAT1 L6=?");    // Wakeup-time: 0=200ms, 1=400ms, 2=600ms, 3=1s, 4=1.5s, 5=2s, 6=2.5s, 7=3s, 8=4s, 9=5s
        lora.command("AAT1 L7=?");    // Module Tx Power Value: (Range: 0-7, Default=7, 20dBm)       
      }
      else if (0 == strcmp(strCmd, "3")) {
        lora.setFirmwareMode(E_FW_P1P2_MOST);
      }
      else if (0 == strcmp(strCmd, "33")) {
        lora.readConfig(5);
      }
      else if (0 == strcmp(strCmd, "a")) {  // baud to 9600
        lora.LoraBase::begin(9600);
      }     
      else if (0 == strcmp(strCmd, "b")) {  // baud to 57600
        lora.LoraBase::begin(57600);
      }
      else if (strCmd[0] == 't') {          // firmware: AAT MOST
        lora.LoraBase::begin(57600);
        char *strData = strCmd + 1;
        lora.sendDataAT(strData, strlen(strData));
      }
      else if (0 == strcmp(strCmd, "8")) {
        lora.LoraBase::begin(57600);
        char *strRet = lora.getDeviceMac();
        debugSerial.print(strRet);
        strRet = lora.getDeviceName();
        strRet = lora.getTxPower();
      }
      else if (0 == strcmp(strCmd, "0")) {    // get current firmware mode
        E_LORA_FW_MODE eFwMode = lora.getFirmwareMode();
        char *strFw = "Unknown";
        switch (eFwMode) {
          case E_FW_AAT_LORAWAN:
            strFw = "AAT LoRaWAN";
            break;
          case E_FW_AAT_MOST:
            strFw = "AAT MOST";
            break;
          case E_FW_P1P2_MOST:
            strFw = "p1p2 MOST";
            break;
        }
        debugSerial.print(eFwMode);
        debugSerial.print(")Firmware = ");
        debugSerial.println(strFw);
      }
    }
    else {
      lora.command((char*)buf);
    }
  }  
}

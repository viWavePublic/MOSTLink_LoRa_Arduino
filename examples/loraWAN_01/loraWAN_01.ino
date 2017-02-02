//////////////////////////////////////////////////////
// lora_wan example: LoRa WAN
//
// Network Server
// GT IoT: https://nms.gtiot.net/admin/index.htmls
//    Step1: 設備->新增設備 add device DevEui/DevAddr 
//    Step2: 應用程序->Sample->資料控制->變更
//    Step3: 資料輸出->HTTP Push
//    Step4: POST的目地網址->https://www.abizin.com/api/lora/log
//    Step5: 送出
//
// Application Server
// Abizin: http://www.abizin.com/store/lora#
// 
//////////////////////////////////////////////////////

#include "LoraBase.h"

LoraBase lora;

void setup() {
  // put your setup code here, to run once:
  lora.begin(57600);
  debugSerial.begin(9600);

  debugSerial.println(F("*** lora_wan ***"));

  // get device data
  const char *strSave = "AAT1 Save\r\n";
  const char *strFw = "AAT1 FwVersion\r\n";
  const char *strReset = "AAT1 Reset\r\n";
  const char *strDevEui = "AAT2 DevEui=?\r\n";
  const char *strDevAddr = "AAT2 aDevicezEuin=?\r\n";
  const char *strReTx = "AAT2 reTx=?\r\n";
  const char *strSend = "AAT2 Tx=3,cnf,1234AAFF\r\n";

  // send command to LoraWan module
  sendCmdLoraWan(strFw);
  sendCmdLoraWan(strDevEui);
  sendCmdLoraWan(strSend);
}

void sendCmdLoraWan(const char* strData) {
  lora.sendData(strData);

  unsigned long tsStart = millis();
  char buf[100] = {0};
  int szBuf = 0;
  while (millis() - tsStart < 3000) {
    if (lora.receData() > 0) {
      Serial.println("------");
      break;
    }
    delay(100);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

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

#include "MOSTLoraWAN.h"

MOSTLoraWAN loraWAN;

void setup() {
  // put your setup code here, to run once:
  debugSerial.begin(9600);
  debugSerial.println(F("*** loraWAN_01 ***"));

  loraWAN.begin(57600);

  ////////////////////
  // get node info
  ////////////////////
//  const char *strFwVer = loraWAN.getFwVersion();    // firmware version
//  const char *strDevDui = loraWAN.getDevEui();      // dev EUI
//  const char *strDevAddr =loraWAN.getDevAddr();     // dev address

//  int nADR = loraWAN.getADR();
//  loraWAN.setADR(1);
  
  // send Tx: payload by HEX encode
  const char *payload = "aabbcc1122334488";
  loraWAN.setTx(3, "cnf", payload);
}


void loop() {
  // put your main code here, to run repeatedly:
  loraWAN.run();

}

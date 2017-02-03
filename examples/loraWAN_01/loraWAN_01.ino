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
  loraWAN.begin(57600);
  debugSerial.begin(9600);

  debugSerial.println(F("*** loraWAN_01 ***"));

  // get device data
  const char *strSend = "AAT2 Tx=3,cnf,ccddeeff11223344";

  // send command to LoraWan module
//  loraWAN.testAll();
  loraWAN.getADR();
  loraWAN.setTx(3, "cnf", "112233");
//  loraWAN.command(strSend);
}


void loop() {
  // put your main code here, to run repeatedly:
  loraWAN.run();

}

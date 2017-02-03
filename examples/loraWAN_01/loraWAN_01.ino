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

  loraWAN.setRxDelay1(123000);
  long resDelay = loraWAN.getRxDelay1();
  long freq = 0;
  int dataRate = 0;
  int channelOpen = 0, bandGroup = 0;
  loraWAN.getRx2_Freq_DR(freq, dataRate);
  loraWAN.getTx_Channel(2, freq, dataRate, channelOpen, bandGroup);

  // send Tx
//  loraWAN.command(strSend);
  loraWAN.setTx(3, "cnf", "1122339900AA");
}


void loop() {
  // put your main code here, to run repeatedly:
  loraWAN.run();

}

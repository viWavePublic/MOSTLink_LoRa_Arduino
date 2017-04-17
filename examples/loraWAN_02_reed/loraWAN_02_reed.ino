//////////////////////////////////////////////////////
// loraWAN_02_reed example:
//    When reed-sensor changed, send payload to GtIot.
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
#include "MLutility.h"

MOSTLoraWAN loraWAN;

#define PIN_LED    13
#define PIN_REED   2

int reedCurr = 1, reedPrev = 1, nCountChange = 0;
unsigned long tickNow, tickTx = 0, periodTx = 6000;


void setup() {
  // put your setup code here, to run once:
  debugSerial.begin(9600);
  debugSerial.println(F("*** loraWAN_02_reed ***"));
  
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_REED, INPUT);

  // init LoRaWAN
  loraWAN.begin(57600);

  MLutility::blinkSOS(100);
  RefreshLed(reedCurr);
    
  ////////////////////
  // get node info
  ////////////////////
//  const char *strFwVer = loraWAN.getFwVersion();    // firmware version
  const char *strDevDui = loraWAN.getDevEui();      // dev EUI
//  const char *strDevAddr =loraWAN.getDevAddr();     // dev address

//  int nADR = loraWAN.getADR();
//  loraWAN.setADR(1);
}

bool onReedChange()
{
  bool bRet = false;
  reedCurr = digitalRead(PIN_REED);
  if (reedPrev != reedCurr) {
    reedPrev = reedCurr;
    tickNow = millis();
    RefreshLed(reedCurr);
    bRet = true;
  }
  return bRet;
}

void RefreshLed(boolean bState)
{
    digitalWrite(PIN_LED, !bState);
}

void loop() {
  // put your main code here, to run repeatedly:
  loraWAN.run();

  const bool bReedChange = onReedChange();
  if (bReedChange) {
    nCountChange++;
  }

  if (nCountChange > 0) {
    tickNow = millis();
    unsigned long elpaseTx = tickNow - tickTx;
    if (0 == tickTx || (elpaseTx >= periodTx)) {
      char strTx[20] = {0};
      sprintf(strTx, "%02X%02X%08X", reedCurr, nCountChange, tickNow);

      debugSerial.println(strTx);

      bool bResultTx = loraWAN.setTx(3, "cnf", strTx);
      tickTx = millis();
      if (bResultTx) {
        nCountChange = 0;
        
        MLutility::blinkLed(3);
      }
      else
        MLutility::blinkLed(1);
      
        RefreshLed(reedCurr);
    }
  }

//  const int nStateReed = digitalRead(PIN_REED);
//  digitalWrite(PIN_LED, nStateReed);
  delay(10);
}

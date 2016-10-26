//////////////////////////////////////////////////////
// This sample code is used for debug
//
//   read config: last char "0xFF" is unknown as following
//   
//   Send >>> FF4CCF52A157F10DF863000007030005 (16 bytes)
//   Rece <<< 243130314831363130303631000DB535077C356F000DF863000703000521FF (31 bytes)
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"

MOSTLora lora;

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);      // module mode: power-saving

  lora.printInfo();
}

void loop() {
  lora.readConfig();
  delay(5000);
}


//////////////////////////////////////////////////////
// MOSTLink LoRa Arduino sample code for starter.
// For check gateway and LoRa node are working correctly.
// 
// https://github.com/viWavePublic/MOSTLink_Doc/blob/master/GettingStartedwithMOSTLinkLoRa.pdf
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#define LED_PIN 13
 
MOSTLora lora; 
 
void setup() { 
  pinMode(LED_PIN, OUTPUT);
  
  // Serial for log monitor
  Serial.begin(9600);
  Serial.println(F("*** lora_01_starter ***"));
  
  lora.begin(); 
  
  // config setting: drequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);
  
  // set polling request callback
  lora.setCallbackPacketReqData(funcBlink);
} 
  
void loop() { 
  lora.run(); // lora handle input messages
  delay(100); 
}   

// Callback method when getting request packet from a station gateway
void funcBlink(unsigned char *data, int szData) {
  delay(500);
  for (int i=0; i<10;i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);  
  }
}

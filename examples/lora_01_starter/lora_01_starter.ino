#include "MOSTLora.h"
#define LED_PIN 13
 
MOSTLora lora; 
 
void setup() { 
  pinMode(LED_PIN, OUTPUT); 
  Serial.begin(9600);
  
  lora.begin(); 
  //
  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);
  lora.setCallbackPacketReqData(funcBlink); // set polling request callback 
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

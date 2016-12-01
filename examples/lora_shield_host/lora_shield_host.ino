#include "MOSTLora.h"
#include "MLutility.h"

#define LED_PIN 13

MOSTLora lora;

// TODO: extract constant to .h file
const unsigned char CMD_START = 0xAE;
const unsigned char CMD_HELLO = 0x01;
const unsigned char CMD_HELLO_ACK = 0x02;
const int CMD_LENGTH = 10;


void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  lora.begin();
  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_NORMAL);    // module mode: power-saving
  lora.setCallbackReceData(receData);
  digitalWrite(LED_PIN, HIGH); // high led for indicating ready to work
}

void receData(unsigned char* data, int szData) {
  Serial.println("receiver - receData");
  MLutility::printBinary(data, szData);
  if (CMD_START == data[0]) {
    char cmd = data[1];
    if (CMD_HELLO == cmd) {
      Serial.println("Yes Hello");  
      char mac[8];
      memcpy(mac, data+2, 8);        
      char helloAck[10];
      cmdcpy(helloAck, CMD_HELLO_ACK, mac);
      lora.sendData(helloAck, CMD_LENGTH);
    }
  } else {
    Serial.println("It's a noise");
  }
}

void loop() {
  lora.run(); // lora handle input messages
//  lora.sendData("hello lora!");
  delay(100);
}

// TODO: extract common utils to other file
void cmdcpy(char *cmdData, char *cmdKey, char* mac) {
    cmdData[0] = CMD_START;
    cmdData[1] = cmdKey;
    memcpy(cmdData+2, mac, 8);  
}


const int pinLedRece = 9;

#include "MOSTLora.h"
#include "MLpacket.h"

MOSTLora lora;

char *loraApiKey = "0GFUGE371WNPMMJE";
int szBuf = 0;
byte buf[256] = {0};
long countRun = 0;
boolean bLedRun = HIGH;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinLedRece, OUTPUT);
  
  digitalWrite(pinLedRece, LOW);

  // Serial for log monitor
  Serial.begin(9600);  // use serial port
  
  lora.begin();

  // init sensor for humidity & temperature

  char strHello[] = "Hello Vinduino";
  int szHello = strlen(strHello);
  delay(3000);
  lora.waitUntilReady(5000);
//  lora.sendData((unsigned char*)strHello, szHello);
  lora.sendData(strHello);
}

void loop() {
  if (lora.available()) {
    szBuf = lora.receData(buf, 255);
    if (szBuf >= 2) {
      if (lora.parsePacket(buf, szBuf) >= 0) {
        float arrF[8] = {100, 200, 300, 400, 555, 66, 77, 8};
        responseVinduino(loraApiKey, arrF);
      }
      else {
        char *strBuf = (char*)buf;
        strcat(strBuf, " (ack)");
        szBuf = strlen(strBuf);
        lora.sendData(buf, szBuf);
      }
    }
  }
//  lora.isBusy();
  delay(10);

  // led builtin
  countRun++;
  if (countRun > 20) {
    bLedRun = !bLedRun;
    digitalWrite(pinLedRece, bLedRun);
    countRun = 0;
  }

  // command to test vinduino.io
  if (Serial.available()) {
    int countBuf = 0;
    while (Serial.available()) {
      int c = Serial.read();
      buf[countBuf] = c;
  
      countBuf++;    
      delay(20);
    }
    if (countBuf > 0) {
      buf[countBuf] = 0;
      Serial.print("Command> ");
      Serial.println((char*)buf);
      if (strcmp((char*)buf, "/s") == 0) {
        float arrF8[8] = {11, 22, 33, 44, 50, 60, 77, 88};
        responseVinduino(loraApiKey, arrF8);       
      }
      else {
        lora.sendData((char*)buf);
      }
    }     
  }
}

void responseVinduino(char *apiKey, float *arrF)
{
    /////////////////////////
    // RES_DATA packet
    /////////////////////////   
    MLUplink headUplink;
    headUplink.length = 22 + 53;
    memcpy(headUplink.sender_id, lora.getMacAddress(), 8);
    // prepare uplink header
    memcpy(buf, &headUplink, 22);
    
    // prapare payload chunk
    byte payload[53], *ptr;
    payload[0] = 0x0A;    // version
    payload[1] = 0x10;    payload[2] = 0x02;  // 0x1002 RES_DATA command
    memcpy(payload + 3, apiKey, 16);

    // 8 floats (4 bytes)
    int i, index = 3 + 16;
    for (i = 0; i < 8; i++) {
      ptr = (byte*)(arrF + i);
      payload[index] = ptr[3];
      payload[index + 1] = ptr[2];
      payload[index + 2] = ptr[1];
      payload[index + 3] = ptr[0];
      index += 4;
    }

    payload[index] = 0;      // option flag
    payload[index + 1] = 0;  // payload CRC

    int nModeBackup = lora.getMode();
    lora.setMode(E_LORA_WAKEUP);    
    /////////////////////
    // send data is ready
    memcpy(buf + 22, payload, 53);
    lora.sendData(buf, 75);
    
    lora.setMode(nModeBackup);
}


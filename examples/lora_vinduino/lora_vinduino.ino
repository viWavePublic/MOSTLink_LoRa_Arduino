//////////////////////////////////////////////////////
// This sample code is used for Vinduino project base on MOSTLink protocol
//
// Note:
//    If you use Vinduino board, check MOSTLora library folder.
//    in "MostLora.h" header file, you must unmark "#define USE_VINDUINO".
//    Then MOST Link library would work correctly. 
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLPacketComm.h"

#ifdef USE_VINDUINO       
MOSTLora lora(3, 4, A7);
#else
MOSTLora lora;
#endif

const char *loraApiKey = "0GFUGE371WNPMMJE";    // ThingSpeak API-key for vinduino.io

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_WAKEUP);
  
  delay(1000);
  // test to vinduino.io
  testUploadVinduino();
}

void loop() {
  if (lora.available()) {
    int szRece = lora.receData();
    // reply to vinduino.io when receive "REQ_DATA" by MOSTLink-protocol
    if (lora.parsePacket() == CMD_REQ_DATA) {
      testUploadVinduino();
    }
  }
  delay(100);
}

// prepare random value for upload to vinduino.io
void testUploadVinduino()
{
  int i;
  float arr[8];
  for (i = 0; i < 8; i++) {
    arr[i] = rand() % 50 + (i + 1) * 100;        
  }
// ps: custom your sensor info here
  lora.sendPacketVinduino(loraApiKey, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]);
}


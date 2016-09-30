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

#ifdef USE_VINDUINO       
MOSTLora lora(3, 4, A7);
#else
MOSTLora lora;
#endif

char *loraApiKey = "0GFUGE371WNPMMJE";    // ThingSpeak API-key for vinduino.io

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
//  lora.writeConfig(915000, 0, 0, 7, 5);

  delay(1000);
  // test to vinduino.io
  lora.sendPacketVinduino(loraApiKey, 121, 202, 301, 406, 500, 606, 707, 880);
}

void loop() {
  int szBuf = 0;
  char buf[256] = {0};
  if (lora.available()) {
    szBuf = lora.receData((unsigned char*)buf, 255);
    // reply to vinduino.io when receive "reply" message
    if (strcmp("reply", buf) == 0) {
      lora.sendPacketVinduino(loraApiKey, 150, 252, 350, 456, 590, 650, 750, 850);
      delay(10000);
    }
  }
  delay(100);
}


//////////////////////////////////////////////////////
// This sample code is used for Vinduino project base on MOSTLink protocol
//
// Note:
//    If you use Vinduino board, check MOSTLora library folder:
//    "Arduino/libraries/MOSTLink/MostLora.h" header file,
//    you must unmark "#define USE_VINDUINO".
//
//    Then MOST Link library is ready for use.
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"       // MOSTLora library 
#include "MLPacketComm.h"   // MOSTLink protocol command-ID


#ifdef USE_VINDUINO
MOSTLora lora(3, 4, A7);  // Arduino Pro mini borad: D3 for P1, D4 for P2, A7 for BZ
#else
MOSTLora lora;
#endif

const char *loraApiKey = "0GFUGE371WNPMMJE";    // ThingSpeak API-key for vinduino.io

void setup() {
#ifdef DEBUG_LORA
  Serial.begin(9600);  // use serial port for log monitor
#endif // DEBUG_LORA

  /////////////////////
  // MOSTLink lora lib
  /////////////////////
  lora.begin();         // lora library init
  
  // custom LoRa config: freq(915555kHz), groupid(0), data_rate(0), power(7), wakeup_time(5)
  lora.writeConfig(915555, 0, 0, 7, 5);
  
  lora.setMode(E_LORA_WAKEUP);    // lora module: wakeup-mode 

  // test data to Vinduino.io
  lora.sendPacketVinduino(loraApiKey, 10, 20, 30, 40, 50, 60, 70, 80);
}

void loop() {
  // check lora RX: receive something
  if (lora.available())
  {
    int szRece = lora.receData();   // read receive data to buffer
    
    // reply to vinduino.io when receive "REQ_DATA" by MOSTLink-protocol
    if (lora.parsePacket() == CMD_REQ_DATA)
    {
      // TODO: fill the collected values by sensors, then send to Vinduino.io (with api-key, and 8 fields)
      lora.sendPacketVinduino(loraApiKey, 11, 22, 33, 44, 55, 66, 77, 88);
    }
  }
  delay(100);     // wait a moment for next loop
}


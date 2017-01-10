//////////////////////////////////////////////////////
// Vinduino project base on MOSTLink protocol
// upload sensors to cloud server
// http://vinduino.io/
//
// Note:
//    If you use Vinduino board, check MOSTLora library folder:
//    "Arduino/libraries/MOSTLink/MostLora_def.h" header file,
//    you must unmark "#define USE_VINDUINO".
//
//    Then MOST Link library is ready for use.
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"       // MOSTLora library 

#ifdef USE_VINDUINO
MOSTLora lora(3, 4, A7);  // Arduino Pro mini borad: D3 for P1, D4 for P2, A7 for BZ
#else
MOSTLora lora;
#endif

const char *loraApiKey = "0GFUGE371WNPMMJE";    // API-key for vinduino.io

// callback for REQ_DATA
void funcCustomPacketReqData(unsigned char *data, int szData)
{
#ifdef DEBUG_LORA
  debugSerial.println(F("ReqData, sendPacketVinduino= "));
#endif // DEBUG_LORA

  // send data to Vinduino.io when received REQ_DATA from gateway
  // you should custom the value by sensors' result.
  lora.sendPacketVinduino(loraApiKey, 11, 22, 33, 44, 55, 66, 77, 88);
}

void setup() {
#ifdef DEBUG_LORA
  Serial.begin(9600);  // use serial port for log monitor
  Serial.println(F("*** lora_05_vinduino ***"));  // app title
#endif // DEBUG_LORA

  /////////////////////
  // MOSTLink lora lib
  /////////////////////
  lora.begin();         // lora library init
  
  // config setting: frequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);    // module mode: power-saving

  // test data to Vinduino.io
  lora.sendPacketVinduino(loraApiKey, 10, 20, 30, 40, 50, 60, 70, 80);

  // custom callback
  lora.setCallbackPacketReqData(funcCustomPacketReqData);  
}

void loop() {
  lora.run();
  delay(100);     // wait a moment for next loop
}


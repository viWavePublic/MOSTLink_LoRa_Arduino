# MOSTLink LoRa library
=========
Getting Started Using the MOSTLink LoRa library


Requirements
----
* MOSTLink LoRa shield dock on arduino
* Arduino Uno connected to computer
* Arduino IDE 1.6.x installed in computer

LoRa Shield Switch(S1): occupied pins
---- 
* S1 on LinkIt: D0(RX), D1(TX), D7(P1), D6(P2), D5(BZ)
* S1 on Arduino: D10(RX), D11(TX), D7(P1), D6(P2), D5(BZ) 

Install MOSTLora to Arduino IDE
----
[MOSTLora]: https://github.com/viWavePublic/MOSTLink_LoRa_Arduino  "MOSTLora lib"
* here [GitHub][MOSTLora]: [Clone or download] -> [Download ZIP]
* Arduino IDE install library: [Sketch] -> [Include Library] -> [Add .ZIP Library] 

Crypto library to encrypt/decrypt, then support security transmission
----
* in "MOSTLora_def.h"
* USE_LIB_CRYPTO_AES128: AES/CBC/NoPadding encrypt/decrypt
* USE_LIB_CRYPTO_HMAC: Challenge-response to check HMAC result

Building and using the example by Arduino IDE 
----
[File] -> [Examples] -> [installed MOSTLora lib] as examples folder
1.  Humidity & Temperature Sensor  
        [lora_ex1]  
    
2.  Chat test between gateway and node  
        [lora_chat] 

3.  Vinduino project 
        [lora_vinduino]      
        header file: MOSTLora_def.h, unmark "#define USE_VINDUINO", 
        Digit pin for RX/TX, and P1/P2 would be specified correctly.
        D0(RX), D1(TX), D3(P1), D4(P2), A7(BZ)

4.  ThingSpeak project  
        [lora_thingspeak]           

5.  Challenge-Response by crypto lib
        [lora_fake] as fake node simulate
        [lora_challenge] as challenge-response using HMAC, AES

6.  GPS test (for Linkit One)
        [lora_gps]

7.  MediaTek Cloud Sandbox (MCS)
        [lora_mcs]

8.  debug configure (for GlobalSat debug):  
        [lora_debug]      



Reference
----
* Arduino: https://www.arduino.cc
* MOSTLink: http://www.mostlink.io
* ThingSpeak: https://thingspeak.com
* Vinduino: https://github.com/ReiniervdL/Vinduino
* Cryptographic Library: http://rweather.github.io/arduinolibs/crypto.html
* MCS: https://mcs.mediatek.com

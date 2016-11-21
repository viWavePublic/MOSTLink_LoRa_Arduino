# MOSTLink LoRa library
=========
Getting Started Using the MOSTLink LoRa library


Requirements
----
* MOSTLink LoRa shield dock on arduino
* Arduino Uno connected to computer
* Arduino IDE 1.6.x installed in computer


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
1.  Humidity & Temperature Sensor  
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_ex1]  
    
2.  Vinduino project 
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_vinduino]      
        header file: MOSTLora_def.h, unmark "#define USE_VINDUINO", 
        Digit pin for RX/TX, and P1/P2 would be specified correctly.
        D0(RX), D1(TX), D3(P1), D4(P2), A7(BZ)

3.  ThingSpeak project  
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_thingspeak]      

4.  chat  
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_chat]      

5.  fake node simulate
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_fake]

6.  challenge-response using HMAC
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_challenge]

7.  GPS test (for Linkit One)
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_gps]      

8.  debug configure (for GlobalSat debug):  
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_debug]      

Reference
----
* Arduino: https://www.arduino.cc
* MOSTLink: http://mostlink.viwave.com
* Cryptographic Library: http://rweather.github.io/arduinolibs/crypto.html

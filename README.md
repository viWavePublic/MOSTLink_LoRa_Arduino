# MOSTLink Lora library
=========

Getting Started Using the MOSTLink LoRa library

Requirements
----

* MOSTLink LoRa shield for arduino
* Arduino Uno
* Arduino IDE 1.6.x installed

Install MOSTLora to Arduino IDE
----
[MOSTLora]: https://github.com/MOSTLinkDev/arduino_MOSTLora  "MOSTLora lib"
* from [GitHub][MOSTLora]: [Clone or download] -> [Download ZIP]
* Arduino IDE: [Sketch] -> [Include Library] -> [Add .ZIP Library] 

Building and using the example by Arduino IDE
----
1.  Humidity & Temperature Sensor example:  
        [File] -> [Examples] -> [your MOSTLora lib] -> [lora_ex1]  
    
2.  Vinduino project example:  
        [File] -> [Examples] -> [your MOSTLora lib] -> [lora_vinduino]      

        Remember to unmark "#define USE_VINDUINO" in header "MOSTLora.h"  
        Digit pin for RX/TX, and P1/P2 would be specified correctly.  

Reference
----
* Arduino: https://www.arduino.cc
* MOSTLink: http://mostlink.viwave.com

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
[MOSTLora]: https://github.com/MOSTLinkDev/arduino_MOSTLora  "MOSTLora lib"
* here [GitHub][MOSTLora]: [Clone or download] -> [Download ZIP]
* Arduino IDE install library: [Sketch] -> [Include Library] -> [Add .ZIP Library] 

Building and using the example by Arduino IDE
----
1.  Humidity & Temperature Sensor example:  
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_ex1]  
    
2.  Vinduino project example:  
        [File] -> [Examples] -> [installed MOSTLora lib] -> [lora_vinduino]      
        Remember to unmark "#define USE_VINDUINO" in header "MOSTLora.h"  
        Digit pin for RX/TX, and P1/P2 would be specified correctly.  


Reference
----
* Arduino: https://www.arduino.cc
* MOSTLink: http://mostlink.viwave.com

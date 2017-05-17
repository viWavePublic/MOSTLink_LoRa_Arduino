#include "MLutility.h"

#define PIN_LED 13
#define PIN_PH    A0    // PO <---> A0

// temperature not working yet, no effect
#define PIN_TEMP  A1    // T1 <---> A1
#define PIN_TEMP2 A2    // T2 <---> A2

int szBuf = 0;
byte buf[100] = {0};

void setup() {
  // put your setup code here, to run once:
  debugSerial.begin(9600);  // use serial port for log monitor
  debugSerial.print("=== Test PH Sensor ===");

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  pinMode(PIN_TEMP, INPUT);
  pinMode(PIN_TEMP2, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  // command to send (for debug)
  if (debugSerial.available())
    inputBySerial();

  delay(3000);
  
  int temp = analogRead(PIN_TEMP);
  int temp2 = analogRead(PIN_TEMP2);

  debugSerial.print("\nTemp = ");
  debugSerial.print(temp);
  debugSerial.print(" ,");
  debugSerial.println(temp2);

  float fPH = MLutility::measurePH(PIN_PH);

}

/////////////////////////////////////////////
// test by input 
void inputBySerial()
{
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 1) {
    buf[countBuf] = 0;
    if (buf[0] == '/')
    {
      if (buf[1] == '1') {
        digitalWrite(PIN_LED, LOW);
      }
      else if (buf[1] == '2') {
        digitalWrite(PIN_LED, HIGH);
      }
    }
  }  
}


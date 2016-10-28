
const int pinLedRece = 13;

#include "MOSTLora.h"
//#include <LDHT.h>
#include "DHT.h"
#include "MLPacketParser.h"
#include "MLutility.h"

int count = 0;
const int pinTouch = A0;  // 12;
bool bPressTouch = false;
//Grove_LED_Bar bar(7, 6, false);

//MOSTLora lora(13,12,A2);    // LM-230 (old)
MOSTLora lora(7, 6, 5);       // LM-130 (new)
DHT dht(2, DHT22);

float fTemperature, fHumidity;
int szBuf = 0;
byte buf[256] = {0};
int tsLast;
int tsIdle = 0;

long tsCurr = millis();
long countRun = 0;
boolean bLedRun = HIGH;

String strHi = ""; 

void setup() {
  // put your setup code here, to run once:
  pinMode(pinTouch, INPUT);
  pinMode(pinLedRece, OUTPUT);
  
  digitalWrite(pinLedRece, LOW);
  digitalWrite(pinTouch, LOW);

  // Serial for log monitor
  Serial.begin(9600);  // use serial port
  
  Serial.println("--- App: lora_ex1 ---");  // use serial port

  lora.begin();
  lora.setReceiverID("FFFFFFFF00112233");

//  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_WAKEUP);
  
  #if defined(__LINKIT_ONE__)
    strHi += "[LinkitOne]";
  #else
    strHi += "[ArduinoUno]";
  #endif

  Serial.println(strHi);  // use serial port

  int szHi = strHi.length();
  delay(3000);
  lora.waitUntilReady(5000);
//  lora.sendData((unsigned char*)strHello, szHello);
  lora.sendData((byte*)strHi.c_str(), szHi);

  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 20) {
    bReadDHT = readSensorDHT(fHumidity, fTemperature);
    delay(1000);
    i++;
  }
}


void loop() {
  long tsNow = millis();

  if (lora.available()) {
    szBuf = lora.receData();
    if (szBuf >= 2) {
      Serial.print(szBuf);  // use serial port
      Serial.println(") Parse rece <<<");  // use serial port
      if (lora.parsePacket() == CMD_REQ_DATA) {  // REQ_DATA
        readSensorDHT(fHumidity, fTemperature);
//        lora.sendPacketResData(fHumidity, fTemperature);
        lora.sendPacketResData2(fHumidity, fTemperature);
//        lora.sendPacketVinduino2("0GFUGE371WNPMMJE", 0, 0, 0, 0, 0, 0, 0, 0);
      }
    }
  }
  lora.isBusy();
  delay(2);

  // led builtin
  countRun++;
  if (countRun > 2000) {
//    lora.sendPacketResData2(fHumidity, fTemperature);
    bLedRun = !bLedRun;
//    digitalWrite(pinLedRece, bLedRun);
    countRun = 0;
  }

  // command to send
  if (Serial.available())
    inputBySerial();

  checkTouch();
  //digitalWrite(pinLedRece, bPressTouch);   // turn the LED on (HIGH is the voltage level)
}

boolean readSensorDHT(float &h, float &t)
{
    boolean bRet = false;
    if (dht.read()) {
        h = dht.readHumidity();
        t = dht.readTemperature();    
    }
    else {
      h = 0;
      t = 0;
    }
    if (h > 0)
      bRet = true;
#ifdef DEBUG_LORA
    if (bRet) {
        Serial.print("Humidity: "); 
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: "); 
        Serial.print(t);
        Serial.println(" *C");      
    }
    else {
        Serial.println("DHT Read Fail.");    
    }
#endif // DEBUG_LORA
    return bRet;
}

void inputBySerial()
{
  // serial command to send
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 0) {
    buf[countBuf] = 0;
    Serial.print("Command> ");
    Serial.println((char*)buf);
    if (!parseCommand((char*)buf)) {
//      lora.sendData(buf, countBuf);
      lora.sendData((char*)buf);
    }
  }  
}

boolean parseCommand(char *strCmd)
{
  if (strCmd[0] != '/' || strlen(strCmd) < 2) {
    lora.setMode(E_LORA_WAKEUP);
    return false;
  }

  int nCmd = strCmd[1];
  switch (nCmd) {
    case '0':
      lora.writeConfig(915000, 0, 0, 7, 5);
      break;
    case '1':
      lora.writeConfig(915000, 0, 1, 7, 5);
      break;
    case '2':
      lora.writeConfig(915000, 0, 2, 7, 5);
      break;
    case '3':
      lora.writeConfig(868000, 0, 0, 7, 5);
      break;
    case '4':
      lora.writeConfig(868000, 0, 2, 7, 5);
      break;
    case 'R':
    case 'r':
      lora.readConfig();
      break;
    case 's':
    case 'S':
      readSensorDHT(fHumidity, fTemperature);
      break;
    case 'i':
    case 'I':
      lora.printInfo();
      break;
    case 'm':
    case 'M':
      {
        int nMode = atoi(strCmd + 2);
        lora.setMode(nMode);
        lora.printInfo();
      }
      break;
    case 't':
      lora.writeConfig(913000, 0, 0, 7, 5);
      break;
    case 'f':
      {
        int i = 0;
        int datarate = 0;
        long freq = 915000;
        char *strOut = strtok(strCmd + 2, ",");
        while (strOut != NULL)
        {
          if (0 == i) {
            Serial.print("frequency: ");
            freq = atol(strOut);
            Serial.print(freq);
          }
          else if (1 == i) {
            Serial.print(", data rate: ");
            datarate = atoi(strOut);
            Serial.print(datarate);
            Serial.print(" --- END\n");
          }
          else {
            Serial.print("#");
            Serial.print(i);
            Serial.print(" = ");
            Serial.println(strOut);
          }
          strOut = strtok (NULL, ",");
          i++;
        }        
        lora.writeConfig(freq, 0, datarate, 7, 5);
      }
      break;
    default:
      lora.writeConfig(913000, 0, 0, 7, 5);
      break;
  }
  
  return true;
}


void checkTouch()
{
//  const int nTouchVal = digitalRead(pinTouch);
  const int nTouchVal = analogRead(pinTouch);

  if (!bPressTouch) {
    if (nTouchVal > 512) {   // key-down
      Serial.println("touch-DOWN");
      bPressTouch = true;
      count = (count + 1) % 11;
//      bar.setLevel(count);

      const char *strData = "ABC...123";
//      const char *strData = "Hello Hello Hello 0123456789!!!";
//      lora.sendData((byte*)strData, strlen(strData));
      lora.sendPacketResData2(99.9, -10.1);
    }
  }
  else {
    if (nTouchVal == 0) {  // key-up
      Serial.println("touch-UP");
      bPressTouch = false; 
    }   
  }  
}



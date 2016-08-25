
const int pinLedRece = 11;

#include "MOSTLora.h"
#include "MLpacket.h"
#include "DHT.h"

int count = 0;
const int pinTouch = 8;  // 12;
bool bPressTouch = false;
//Grove_LED_Bar bar(7, 6, false);

MOSTLora lora;
DHT dht(2, DHT22);

float fTemperature, fHumidity;
int szBuf = 0;
byte buf[256] = {0};
int tsLast;
int tsIdle = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinTouch, INPUT);
  pinMode(pinLedRece, OUTPUT);

  digitalWrite(pinLedRece, LOW);
  digitalWrite(pinTouch, LOW);

  Serial.begin(9600);

  dht.begin();
  lora.begin();

  Serial.print(F_CPU);
  Serial.println(" clocks CPU");
  
  // init sensor for humidity & temperature
  readSensorDHT(fHumidity, fTemperature);

  char strHello[] = "Hello...";
  int szHello = strlen(strHello);
  delay(3000);
  lora.waitUntilReady(5000);
//  lora.sendData((unsigned char*)strHello, szHello);
  lora.sendData(strHello);
}

void loop() {
  if (lora.available()) {
    szBuf = lora.receData(buf, 255);
    if (szBuf >= 2) {
      if (lora.parsePacket(buf, szBuf) >= 0) {
        readSensorDHT(fHumidity, fTemperature);
        responseSensorData(fHumidity, fTemperature);
      }
    }
  }
//  lora.isBusy();
  delay(10);
//  readSensorDHT();

  // command to send
  if (Serial.available())
    inputBySerial();

//  checkTouch();
  //digitalWrite(pinLedRece, bPressTouch);   // turn the LED on (HIGH is the voltage level)
}

boolean readSensorDHT(float &h, float &t)
{
    boolean bRet = false;
    h = dht.readHumidity();
    t = dht.readTemperature();
//dht.readHT(&t, &h)
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(t) || isnan(h)) 
    {
        Serial.println("Failed to read from DHT");
    } 
    else 
    {
        bRet = true;            
        Serial.print("Humidity: "); 
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: "); 
        Serial.print(t);
        Serial.println(" *C");
    }
    delay(2000);
    return bRet;
}

void responseSensorData(float h, float t)
{
    /////////////////////////
    // RES_DATA packet
    /////////////////////////   
    MLUplink headUplink;
    headUplink.length = 22 + 15;
    memcpy(headUplink.sender_id, lora.getMacAddress(), 8);
    // prepare uplink header
    memcpy(buf, &headUplink, 22);
    
    // prapare payload chunk
    byte payload[15], *ptr;
    payload[0] = 0x0A;    // version
    payload[1] = 0x02;    payload[2] = 0x02;  // 0x0202 RES_DATA command
    payload[3] = 0;       // error code: 0 - success
    payload[4] = 8;       // data length
    // humidity (4 bytes)
    ptr = (byte*)&h;
    payload[5] = ptr[3];
    payload[6] = ptr[2];
    payload[7] = ptr[1];
    payload[8] = ptr[0];
    // temperature (4 bytes)
    ptr = (byte*)&t;
    payload[9] = ptr[3];
    payload[10] = ptr[2];
    payload[11] = ptr[1];
    payload[12] = ptr[0];
    payload[13] = 0;      // option flag
    payload[14] = 0;      // payload CRC

    int nModeBackup = lora.getMode();
    lora.setMode(E_LORA_WAKEUP);    
    /////////////////////
    // send data is ready
    memcpy(buf + 22, payload, 15);
    lora.sendData(buf, 37);
    
    lora.setMode(nModeBackup);    
}

void inputBySerial()
{
  int countBuf = 0;
  while (Serial.available()) {
    int c = Serial.read();
    buf[countBuf] = c;

    countBuf++;    
    delay(20);
  }
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
            freq = atoi(strOut);
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
  const int nSensorTouch = digitalRead(pinTouch);
  if (!bPressTouch) {
    if (nSensorTouch > 0) {   // key-down
      Serial.println("touch-DOWN");
      bPressTouch = true;
      count = (count + 1) % 11;
//      bar.setLevel(count);

      const char *strData = "ABC...123";
//      const char *strData = "Hello Hello Hello 0123456789!!!";
      lora.sendData((byte*)strData, strlen(strData));
    }
  }
  else {
    if (nSensorTouch == 0) {  // key-up
      Serial.println("touch-UP");
      bPressTouch = false; 
    }   
  }  
}



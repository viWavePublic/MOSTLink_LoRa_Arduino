#include "MOSTLora.h"
#include "MLutility.h"

#define LED_PIN 13

MOSTLora lora;

DataLora loraConfig;

// TODO: extract constant to .h file
const unsigned char CMD_START = 0xAE;
const unsigned char CMD_HELLO = 0x01;
const unsigned char CMD_HELLO_ACK = 0x02;
const int CMD_LENGTH = 10;


enum E_TEST_PHASE {
  PHASE_FAILURE = -3,
  PHASE_UNKNOWN = -2,
  PHASE_INPROGRESS,
  PHASE_SETUP = 0,
  PHASE_HELLO,
  PHASE_SET_MODE,
  PHASE_SUCCESS
};

const int TIMEOUT = 30 * 1000;
int mCurrentPhase = PHASE_UNKNOWN;
int mHandleTime;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("setup begin");
  mHandleTime = millis();

  mCurrentPhase = PHASE_SETUP;
  turnLedOn(); // set led to in progress
  lora.begin();
//  lora.writeConfig(915000, 0, 0, 7, 4);
  lora.setCallbackReceData(receConfig);
  lora.readConfig();  
  lora.setMode(E_LORA_NORMAL);    // module mode: power-saving
  lora.setCallbackReceData(receData);
  Serial.println("setup done");
}

void receConfig(unsigned char* buf, int bufSize) {
  Serial.println("receConfig");
  Serial.print("bufSize: ");
  Serial.println(bufSize);
  Serial.print("buf: ");
  Serial.println((char*)buf);

  int szData = sizeof(DataLora);
  memcpy(&loraConfig, buf, szData);
  if (loraConfig.isValid()) {
    Serial.println("Valid DataLora");
    MLutility::printBinary(loraConfig.mac_addr, 8);
    mCurrentPhase = PHASE_HELLO;
  } else {
    Serial.println("Failed to receive lora config");
    mCurrentPhase = PHASE_FAILURE;
  }
}

boolean equalArrayValues(char *arr1, char *arr2, int szArr) {
  boolean result = true;
  for (int i=0; i< szArr; i++) {
    if (arr1[i] != arr2[i]) {
      result = false;
      break;
    }
  }
  return result;
}

void receData(unsigned char* data, int szData) {
  Serial.print("receData >");
  Serial.println((char*)data);
  Serial.print("bufSize: ");
  Serial.println(szData);
  Serial.println("receiver - receData");
  
  if (CMD_START == data[0]) {
    char cmd = data[1];
    if (CMD_HELLO_ACK == cmd) {
      Serial.println("Get Hello ack");  
      char mac[8];
      memcpy(mac, data+2, 8);
      if (equalArrayValues(mac, loraConfig.mac_addr, 8)) {
        Serial.println("Yes! It's my hello ack");
        if (PHASE_FAILURE != mCurrentPhase) {
          mCurrentPhase = PHASE_SET_MODE;  
        } else {
          Serial.println("Already timeout! don't perform set mode test");
        }
        
      } else {
        Serial.print("Not my response! mac: ");
        MLutility::printBinary(mac, 8);
        Serial.print(", mine: ");
        MLutility::printBinary(loraConfig.mac_addr, 8);
      }
    }
  } else {
    Serial.println("It's a noise");
  }
  
}

void loop() {
  lora.run();
  switch(mCurrentPhase) {
    case PHASE_HELLO:
      mCurrentPhase = PHASE_INPROGRESS;
      Serial.println("perform hello phase");
      char cmdHello[11];
      cmdcpy(cmdHello, CMD_HELLO, loraConfig.mac_addr);
      lora.sendData((char *)cmdHello, CMD_LENGTH);
      mHandleTime = millis();
      break;
    case PHASE_SET_MODE:
      if (performSetModeTest()) {
        mCurrentPhase = PHASE_SUCCESS;    
      } else {
        mCurrentPhase = PHASE_FAILURE;
      }
      break;
    case PHASE_INPROGRESS:
      if (millis() - mHandleTime > TIMEOUT) {
        mCurrentPhase = PHASE_FAILURE;
      }
      break;
    case PHASE_SUCCESS:
      turnLedBlink();
      break;
    case PHASE_SETUP:
      // Error if we reach PHASE_SETUP in loop. phase should be transited to other phase before entering loop.
      Serial.println("failed to setup");
      mCurrentPhase = PHASE_FAILURE;
      break;
    case PHASE_FAILURE:
    case PHASE_UNKNOWN:
      turnLedOff();
      break;
  }
  delay(100);
}

boolean performSetModeTest() {
  boolean modeResult = true;
  do {
    lora.setMode(E_LORA_POWERSAVING);
    if (E_LORA_POWERSAVING != lora.getMode()) {
      Serial.println("set pwr-save mode failed");
      modeResult = false;
      break;
    }
    delay(500);
    lora.setMode(E_LORA_SETUP);
    if (E_LORA_SETUP != lora.getMode()) {
      Serial.println("set setup mode failed");
      modeResult = false;
      break;
    }
    delay(500);
    lora.setMode(E_LORA_NORMAL);
    if (E_LORA_NORMAL != lora.getMode()) {
      Serial.println("set normal mode failed");
      modeResult = false;
      break;
    }
    delay(500);
    lora.setMode(E_LORA_WAKEUP);
    if (E_LORA_WAKEUP != lora.getMode()) {
      Serial.println("set wakeup mode failed");
      modeResult = false;
      break;
    }        
  } while(false);  
  Serial.print("set mode test: ");
  Serial.println(modeResult);
  return modeResult;
}
void turnLedOn() {
  digitalWrite(LED_PIN, HIGH);
}

void turnLedOff() {
  digitalWrite(LED_PIN, LOW);
}

// Callback method when getting request packet from a station gateway
void turnLedBlink() {
    for (int i=0; i<5;i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }  
    delay(500);
}

// TODO: extract common utils to other file
void cmdcpy(char *cmdData, char *cmdKey, char* mac) {
    cmdData[0] = CMD_START;
    cmdData[1] = cmdKey;
    memcpy(cmdData+2, mac, 8);  
}


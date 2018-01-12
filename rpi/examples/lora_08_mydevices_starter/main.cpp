//////////////////////////////////////////////////////
// Starter on MOSTLink protocol with myDevices Cayenne
// 
// Sensor connect to these pins on Raspberry Pi:
//     1. humidity-temperature sensor (3)
//
// myDevices Cayenne
//     1. sign in http://mydevices.com/cayenne/signin/
//     2. [Add New...]->[Device/Widget]->CAYENNE API(Bring Your Own Thing)
//     3. copy/paste: MQTT USERNAME, PASSWORD, CLIENT ID
//     4. Vertify and Upload this code to Raspberry Pi
//
// Command in Serial monitor
//     "/1" relogin myDevices Cayenne
//     "/2" send uplink DHT to myDevices
//////////////////////////////////////////////////////

#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <string>
#include "MOSTLora_def.h"
#include "MOSTLora.h"
#include "MLutility.h"
#include "DHT.h"


MOSTLora lora;
/////////////////////////////////////////////
// DHT sensor related:
#include "DHT.h"
DHT dht(3, DHT22);      // DHT22, DHT11
/////////////////////////////////////////////
// read DHT sensor: Temperature and Humidity
bool readSensorDHT(float &fHumi, float &fTemp, bool bShowResult)
{
    bool bRet = true;


    bRet = dht.readSensor(fHumi, fTemp);

	if (bShowResult) {
        printf("[%s][dbg] ts(%d), ", __FUNCTION__, millis());
        if (bRet) {
            printf("Humidity: %f %, Temperature: %f *C\n", fHumi, fTemp);
        }
        else {
            printf("DHT Fail.\n");
        }
    }
	
    return bRet;
}
/////////////////////////////////////////////

int szBuf = 0;
byte buf[100] = {0};
char strTmp[32] = {0};
float fTemperature, fHumidity;
unsigned long tsSensor = millis();

// ****** NOTICE ******
// TODO: Before you run your application, you need a myDevices Cayenne account.
const char *MQTTUsername = "522dbae0-a20e-11e7-bba6-6918eb39b85e";
const char *MQTTPassword = "5d2b97e0a195561dac11654d6c7a5457e330def4";
const char *ClientID =     "cdcb2060-a20f-11e7-a9b2-a5d7f5484bce";


/////////////////////////////////////////////

void sendUplinkDHT() {
  readSensorDHT(fHumidity, fTemperature, true);

  String strCmd = "2,temp,f,";
  int nVal = dht.convertCtoF(fTemperature);
  sprintf(strTmp, "%d", nVal);
  strCmd += strTmp;
  lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
  
  strCmd = "3,rel_hum,p,";
  sprintf(strTmp, "%d", (int)fHumidity);
  strCmd += strTmp;
  lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
}

/////////////////////////////////////////////
// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  MLutility::blinkSOS(50);
  memcpy(buf, data, szData);  
  buf[szData] = 0;
  printf("[%s] ReqData= %s\n", __FUNCTION__, buf);

  sendUplinkDHT();
}

/////////////////////////////////////////////
void setup() {

  printf("*** lora_08_mydevices_starter ***\n");

  lora.begin();
  // custom LoRa config by your environment setting
  // config setting: frequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);         // E_LORA_POWERSAVING

  delay(1000);

  // custom callback
  lora.setCallbackPacketReqData(funcPacketReqData);

  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 8) {
    delay(700);
    bReadDHT = dht.readSensor(fHumidity, fTemperature);
    i++;
  }

  // login myDevices
  lora.sendPacketReqLoginMydevices(MQTTUsername, MQTTPassword, ClientID);
}

/////////////////////////////////////////////

void *loop(void *arg) 
{
	unsigned long tsCurr;
	
	
	tsSensor = millis();	
	while(1) {
  		lora.run();
  		delay(100);
   
  		tsCurr = millis();
  		if (tsCurr > tsSensor + 5000) {
    		tsSensor = tsCurr;
    		readSensorDHT(fHumidity, fTemperature, true);
    		fTemperature = dht.convertCtoF(fTemperature);

    		printf("[%s] Temperature: %f *F, humidity: %f\n", __FUNCTION__, fTemperature, fHumidity);
  		}
	}
}




int main(void)
{
	pthread_t threadId;
	
	
	printf("[%s][dbg] invoke lora.begin\n", __FUNCTION__);
	setup();
	
	if (pthread_create(&threadId, NULL, loop, NULL) != 0)
		printf("[%s][dbg] fail to create loop thread\n", __FUNCTION__);
		
	while (1) {
		scanf("%s", buf);
		if (buf[0] == '/') {
      		if (buf[1] == '1') {
        		// login myDevices
        		lora.sendPacketReqLoginMydevices(MQTTUsername, MQTTPassword, ClientID);
      		}
      		else if (buf[1] == '2') {
        		sendUplinkDHT();
      		}
      		else if (buf[1] == '3') {
      		}
      		else if (buf[1] == '4') {
      		}
      		else if (buf[1] == 's') {
      		}
    	}
 	}
	
	return 1;
}

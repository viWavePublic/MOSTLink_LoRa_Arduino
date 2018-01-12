//////////////////////////////////////////////////////
// ShareCourse on MOSTLink protocol with MCS
// 
// Starter: uplink to MCS
// 1. DHT22 sensor for humidity and temperature
//
//////////////////////////////////////////////////////
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <string>
#include "MOSTLora_def.h"
#include "MOSTLora.h"
#include "MLutility.h"




using namespace std;




MOSTLora lora;

int szBuf = 0;
byte buf[100] = {0};
char *strTmp = (char*)buf;
float fTemperature, fHumidity;

unsigned long tsSensor = millis();

// TODO: fill your own deviceKey, deviceID from MCS
const char *strDevice = "DNqBP1Mq,vHX8ArP0tFivfPDo";  // usgmostlink

const char *strDispTemperature = "DISP_T";
const char *strDispHumidity = "DISP_H";


/////////////////////////////////////////////
// DHT sensor related:
#include "DHT.h"
DHT dht(3, DHT22);      // DHT22, DHT11




/////////////////////////////////////////////
// read DHT sensor: Temperature and Humidity
bool readSensorDHT(float &fHumi, float &fTemp, bool bShowResult)
{
    bool bRet = true;
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    bRet = dht.readSensor(fHumi, fTemp);

    if (bShowResult) {
    	printf("[%s][dbg] ts(%d), ", __FUNCTION__, millis());
        if (bRet) {
            printf("Humidity: %f %, Temperature: %f *C \n", fHumi, fTemp);
        }
        else {
        	printf("DHT Fail.\n");
        }
    }
    
    return bRet;
}




void sendUplinkDHT() {
 	string strCmd = strDevice;
  	strCmd += ",";
  	strCmd += MLutility::generateChannelData(strDispHumidity, fHumidity);
  	strCmd += "\n";
  	strCmd += MLutility::generateChannelData(strDispTemperature, fTemperature);

  	lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(500);

  	printf("[%s][dbg] %d count chars\n", __FUNCTION__, strCmd.length());
}




// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  	MLutility::blinkSOS(50);
  	memcpy(buf, data, szData);  
  	buf[szData] = 0;
	printf("[%s][dbg](%d) ReqData= %s\n", __FUNCTION__, szData, buf);

  	sendUplinkDHT();
}




// send uplink command to MCS
void sendUplink(const char *strID, const char *strValue)
{
  String strCmd = strDevice;
  strCmd += ",";
  strCmd += MLutility::generateChannelData(strID, strValue);
  
  lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
}




void setup() {
  	printf("[dbg] *** lora_07_mcs_starter ***\n");
  	
  	// custom LoRa config by your environment setting
  	// config setting: frequency, group, data-rate, power, wakeup-time
  	lora.writeConfig(915000, 0, 0, 7, 5);

  	delay(1000);
  	lora.setMode(E_LORA_WAKEUP);         // E_LORA_POWERSAVING
  	lora.sendData("MCS_starter");
  	lora.setMode(E_LORA_POWERSAVING);         // E_LORA_POWERSAVING

  	// custom callback
  	lora.setCallbackPacketReqData(funcPacketReqData);

  	// init sensor for humidity & temperature
  	dht.begin();
  	int i = 0;
  	boolean bReadDHT = false;
  	while (!bReadDHT && i < 8) {
    	delay(700);
    	bReadDHT = readSensorDHT(fHumidity, fTemperature, true);
    	i++;
  	}
  		
}




void *loop(void *arg) 
{
	unsigned long tsCurr;
	static unsigned long tsLive = 0;
	
	
	tsSensor = millis();
	while(1) {
  		lora.run();
  		delay(100);
  		
  		tsCurr = millis();
  		if (tsCurr > tsSensor + 6000) {
    		tsSensor = tsCurr;
    		readSensorDHT(fHumidity, fTemperature, true);
  		}
  
  		if (tsCurr > tsLive + 2000) {
    		tsLive = tsCurr;
    		MLutility::blinkLed(3, 30);
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
        		lora.sendPacketReqLoginMCS((uint8_t*) strDevice, strlen(strDevice));
      		}
      		else if (buf[1] == '2') {
        		sendUplinkDHT();
      		}
      		else if (buf[1] == '3') {
      		}
      		else if (buf[1] == '4') {
      		}
      		else if (buf[1] == 's') {
        		readSensorDHT(fHumidity, fTemperature, true);
      		}
      		else if (buf[1] == '9') {
        		lora.sendPacketSendMCSCommand((buf + 2), strlen((char*) buf + 2));
      		}
    	}
 	}
	
	return 1;
}





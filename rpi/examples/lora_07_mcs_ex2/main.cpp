//////////////////////////////////////////////////////
// ShareCourse on MOSTLink protocol with MCS
// 
// Green House system v2 by LoRa and MOSTLink
// 1. DHT22 sensor for humidity and temperature
// 2. Led for control humidity
// 3. Fan for cool down
// 4. Pump water by duration
//
//////////////////////////////////////////////////////

#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <string>
#include <stdlib.h> // atoi
#include "MOSTLora_def.h"
#include "MOSTLora.h"
#include "DHT.h"
#include "MLutility.h"




using namespace std;




MOSTLora lora;

#define PIN_LED_CONTROL  29
#define PIN_FAN_CONTROL  8
#define PIN_FAN_CONTROL2 9
#define PIN_PUMP_CONTROL 24

int szBuf = 0;
byte buf[100] = {0};
char *strTmp = (char*)buf;
float fTemperature, fHumidity;

#define TAG_SO    0x1235
struct DataSo {
  int nTag;     // 0x1235: stored correct value
  int soHot;
  int soWet;
  int timePump;   // pump active duration (unit: second)
};
DataSo dataSo;

unsigned long tsSensor = millis();
unsigned long tsStartPump = 0;


// TODO: fill your own deviceKey, deviceID from MCS
const char *strDevice = "DYE8T6Go,cB7NkS3rQzOSA0lV";  // usgmostlink

const char *strCtrlFan = "CTRL_FAN";
const char *strCtrlLed = "CTRL_LED";
const char *strCtrlPump = "CTRL_PUMP";
const char *strCtrlUpdate = "CTRL_UPDATE";        // refresh sensor
const char *strCtrlSoHot = "CTRL_SO_HOT";   // so hot degree
const char *strCtrlSoWet = "CTRL_SO_WET";   // so wet
const char *strCtrlPumpTime = "CTRL_PUMP_TIME";
const char *strDispTemperature = "DISP_TEMPERATURE";
const char *strDispHumidity = "DISP_HUMIDITY";
const char *strDispLog = "DISP_LOG";


/////////////////////////////////////////////
// DHT sensor related:
#include "DHT.h"
DHT dht(3, DHT22);      // DHT22, DHT11




// read config file
int readConfigFile(void)
{
	char buffer[32] = {0};
	int para;
	FILE *fp;	

	
	fp = fopen("./ex7ShareCourseConf2", "r");
	if (fp == NULL) {
		printf("[%s] can not open config file\n", __FUNCTION__);
		return 0;
	}
	
	while (!feof(fp)) {
		memset(buffer, 0, sizeof(buffer));
		fgets(buffer, sizeof(buffer), fp);

		if (strstr(buffer, "#") != 0) continue;
		printf("[%s] len: %d, buf: %s\n", __FUNCTION__, strlen(buffer), buffer);

		if (strstr(buffer, "soHot") != 0) {					
			sscanf(buffer, "soHot=%d", &para);
			dataSo.soHot = para;
			printf("[%s] soHot: %d\n", __FUNCTION__, dataSo.soHot); 
		
		} else if (strstr(buffer, "soWet") != 0) {					
			sscanf(buffer, "soWet=%d", &para);
			dataSo.soWet = para;
			printf("[%s] soWet: %d\n", __FUNCTION__, dataSo.soWet); 
		
		} else if (strstr(buffer, "timePump") != 0) {					
			sscanf(buffer, "timePump=%d", &para);
			dataSo.timePump = para;
			printf("[%s] timePump: %d\n", __FUNCTION__, dataSo.timePump); 
			
		} else if (strstr(buffer, "nTag") != 0) {					
			sscanf(buffer, "nTag=%04X", &para);
			dataSo.nTag = para;
			printf("[%s] nTag: 0x%04X\n", __FUNCTION__, dataSo.nTag); 
		} 
		
	}	
	
	fclose(fp);
	
	return 1;
}



		
// write config file
int writeConfigFile(void)
{
	FILE *fp;	


	fp = fopen("./ex7ShareCourseConf2", "w+");
	if (fp == NULL) {
		printf("[%s] can not open config file\n", __FUNCTION__);
		return 0;
	}
 
	fprintf(fp, "soHot=%d\n", dataSo.soHot); 
	fprintf(fp, "soWet=%d\n", dataSo.soWet); 
	fprintf(fp, "timePump=%d\n", dataSo.timePump);
	fprintf(fp, "nTag=%04X\n", dataSo.nTag); 
	
	fclose(fp);
		
	return 1;
}




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
  	delay(600);

  	printf("[%s][dbg] %d count chars ", __FUNCTION__, strCmd.length());
}





// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  	memcpy(buf, data, szData);  
  	buf[szData] = 0;
	printf("[%s][dbg](%d) ReqData= %s\n", __FUNCTION__, szData, buf);
}




// parse downlink command from MCS
boolean parseDownlink(const char *strToken, int &nVal) 
{
	boolean bRet = false;
  	const char *strBuf = (const char *)buf;
  
  
  	if (strstr(strBuf, strToken) == strBuf) {
    	const char *strVal = strBuf + strlen(strToken) + 1;
    	nVal = atoi(strVal);
    	bRet = true;
  	}

  	//printf("[%s][dbg] strToken: %s, ret: %d\n", __FUNCTION__, strToken, bRet);
  	
	return bRet;
}




// send uplink command to MCS
void sendUplink(const char *strID, const char *strValue)
{
  	string strCmd = strDevice;
  	strCmd += ",";
  	strCmd += MLutility::generateChannelData(strID, strValue);
  
  	lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(500);
}




void refreshControlState(void) 
{
  	string strCmd = strDevice;
  	strCmd += ",";
  	strCmd += MLutility::generateChannelData(strCtrlSoHot, dataSo.soHot);
  	strCmd += "\n";
  	strCmd += MLutility::generateChannelData(strCtrlSoWet, dataSo.soWet);

  	lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(800);

  	strCmd = strDevice;
  	strCmd += ",";
  	strCmd += MLutility::generateChannelData(strCtrlFan, digitalRead(PIN_FAN_CONTROL));
  	strCmd += "\n";
  	strCmd += MLutility::generateChannelData(strCtrlLed, digitalRead(PIN_LED_CONTROL));
  	strCmd += "\n";
  	strCmd += MLutility::generateChannelData(strCtrlUpdate, "0");
  
  	lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(800);

  	// for pump
  	strCmd = strDevice;
  	strCmd += ",";
  	strCmd += MLutility::generateChannelData(strCtrlPump, digitalRead(PIN_PUMP_CONTROL));
  	strCmd += "\n";
  	strCmd += MLutility::generateChannelData(strCtrlPumpTime, dataSo.timePump);
  
  	lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(800);
}




void funcPacketNotifyMcsCommand(unsigned char *data, int szData)
{
	memcpy(buf, data, szData);  
  	buf[szData] = 0;
  
  	printf("[%s][dbg](%d) NotifyMcsCommand= %s\n", __FUNCTION__, szData, buf);

  	int nVal = 0;
  	string strValue;
  	const char *strCtrl = NULL;
  
  
  	if (parseDownlink(strCtrlFan, nVal)) {
    	digitalWrite(PIN_FAN_CONTROL, nVal);
    	strValue = strCtrlFan;
    	if (0 == nVal)
      		strValue += " off";
    	else
      		strValue += " on";
  	}
  	else if (parseDownlink(strCtrlLed, nVal)) {
    	digitalWrite(PIN_LED_CONTROL, nVal);
    	strValue = strCtrlLed;
    	if (0 == nVal)
      		strValue += " off";
    	else
      		strValue += " on";    
  	}
  	else if (parseDownlink(strCtrlUpdate, nVal)) {
    	readSensorDHT(fHumidity, fTemperature, true);

    	sendUplinkDHT();
    	refreshControlState(); 
  	}
  	else if (parseDownlink(strCtrlSoHot, nVal)) {
    	dataSo.soHot = nVal;
    	// write config file
    	writeConfigFile();    
    	sprintf(strTmp, "(Hot)T > %d", nVal);
    	strValue = strTmp;
  	}
  	else if (parseDownlink(strCtrlSoWet, nVal)) {
    	dataSo.soWet = nVal;
    	// write config file
    	writeConfigFile();
    	sprintf(strTmp, "(Wet)H > %d", nVal);    
    	strValue = strTmp;
  	}
  	else if (parseDownlink(strCtrlPumpTime, nVal)) {
    	printf("[%s] get pump time: %d\n", __FUNCTION__, nVal);
    	dataSo.timePump = nVal;
    	// write config file
    	writeConfigFile();
    	sprintf(strTmp, "Pump time: %d sec", nVal);
    	strValue = strTmp;
  	}
  	else if (parseDownlink(strCtrlPump, nVal)) {
    	digitalWrite(PIN_PUMP_CONTROL, nVal);
    	if (1 == nVal) {
      		tsStartPump = millis();
    	}
  	}  

  	// send log to MCS (as LoRa ack)
  	if (strValue.length() > 0) {
    	// add timestamp
    	unsigned long tsCurr = millis();
    	sprintf(strTmp, " (@%d)", tsCurr);
    	strValue += strTmp;
    	sendUplink(strDispLog, strValue.c_str());
    	printf("[%s] send log: %s\n", __FUNCTION__, strValue.c_str());  
  	}
}




void setup() {
  	printf("[dbg] *** lora_07_mcs_ex2 ***\n");

	// read config file
  	readConfigFile();
  	if (dataSo.nTag != TAG_SO) {
    	printf("[%s][dbg] === init so hot/wet!\n", __FUNCTION__);
    	dataSo.nTag = TAG_SO;
    	dataSo.soHot = 30;
    	dataSo.soWet = 65;
    	dataSo.timePump = 3;
    	// write config file
    	writeConfigFile();
  	}
  	else {
  		printf("[%s][dbg] === load: hot> %d, wet> %d, pump sec: %d\n", __FUNCTION__, dataSo.soHot, dataSo.soWet, dataSo.timePump);
  	}
  	
  	pinMode(PIN_LED_CONTROL, OUTPUT);
  	pinMode(PIN_FAN_CONTROL, OUTPUT);
  	pinMode(PIN_FAN_CONTROL2, OUTPUT);
  	pinMode(PIN_PUMP_CONTROL, OUTPUT);
  	digitalWrite(PIN_LED_CONTROL, LOW);
  	digitalWrite(PIN_FAN_CONTROL, HIGH);
  	digitalWrite(PIN_FAN_CONTROL2, LOW);
  	digitalWrite(PIN_PUMP_CONTROL, LOW);
    
  	lora.begin();
  	// custom LoRa config by your environment setting
  	// config setting: frequency, group, data-rate, power, wakeup-time
  	lora.writeConfig(915222, 0, 0, 7, 5);
  	lora.setMode(E_LORA_POWERSAVING);         // E_LORA_POWERSAVING

  	delay(1000);

  	// custom callback
  	lora.setCallbackPacketReqData(funcPacketReqData);
 	lora.setCallbackPacketNotifyMcsCommand(funcPacketNotifyMcsCommand);

  	// init sensor for humidity & temperature
  	dht.begin();
  	int i = 0;
  	boolean bReadDHT = false;
  	while (!bReadDHT && i < 8) {
    	delay(700);
    	bReadDHT = readSensorDHT(fHumidity, fTemperature, true);
    	i++;
  	}

  	// login MCS
  	lora.sendPacketReqLoginMCS((uint8_t*)strDevice, strlen(strDevice));

  	// init MCS control state:
  	refreshControlState();
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
  		 if (tsCurr > tsSensor + 5000) {
    		tsSensor = tsCurr;
    		readSensorDHT(fHumidity, fTemperature, true);
    		if (fTemperature > dataSo.soHot && 0 == digitalRead(PIN_FAN_CONTROL)) {
      			digitalWrite(PIN_FAN_CONTROL, 1);
      			sendUplink(strCtrlFan, "1");
      			sendUplinkDHT();     
      			printf("[%s] (dbg) temperature > %d, active fan.\n", __FUNCTION__, dataSo.soHot); 
    		}
    		
    		if (fHumidity > dataSo.soWet && 0 == digitalRead(PIN_LED_CONTROL)) {
      			digitalWrite(PIN_LED_CONTROL, 1);
      			sendUplinkDHT();      
      			sendUplink(strCtrlLed, "1");
      			
      			printf("[%s] (dbg) humidity > %d, active led.\n", __FUNCTION__, dataSo.soWet);
    		}
  		}
  			
  		if (tsStartPump > 0 && tsCurr - tsStartPump > dataSo.timePump * 1000)
  		{
    		tsStartPump = 0;
    		digitalWrite(PIN_PUMP_CONTROL, 0);
    		sendUplink(strCtrlPump, "0");
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





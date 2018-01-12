//////////////////////////////////////////////////////
// USG(CES) on MOSTLink protocol with MCS
// 
// Sensor connect to these pins (wiringPi pin) on Raspberry Pi:
//     1. blue-led(1): show light-sensor value from MCP3008
//     2. red-led(28): show reed-sensor(7) state
//     3. green-led(27): control by MCS
//     4. humidity-temperature sensor (3)
//
// MediaTek Cloud Sandbox(MCS) web-server
//     1. sign in https://mcs.mediatek.com
//     2. Development->Prototype->Create Prototype->import from JSON file
//     3. select "MCS_USG.json" in "Arduinolibraries/MOSTLinkLib/examples/lora_07_mcs_usg" folder
//     4. Create test device, MCS will generate DeviceId,DeviceKey.
//     5. modify strDevice as your "DeviceId,DeviceKey"
//     6. select test device in MCS
//     7. Vertify and Upload this code to Raspberry Pi
//
//////////////////////////////////////////////////////
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <string>
#include <stdlib.h> // for atoi
#include <wiringPiSPI.h>
#include "MOSTLora_def.h"
#include "MOSTLora.h"
#include "MLutility.h"
#include "DHT.h"




using namespace std;




MOSTLora lora;
DHT dht(3, DHT11);       

#define PIN_LED_CONTROL   27

#define PIN_SENSOR_REED   7
#define PIN_REED_LED      28

#define PIN_LIT_LED       1




int szBuf = 0;
byte buf[100] = {0};
char strTmp[32] = {0};
float fTemperature, fHumidity;
unsigned long tsSensor = millis();
int myFd;


const char *strDevice = "DqADKUf1,oE8PFZDqfV9CgpQW";    // LoRa Node #1
const char *strCtrlLed = "CTRL_LED";
const char *strCtrlUpdate = "CTRL_UPDATE";        // refresh sensor
const char *strDispTemperature = "DISP_TEMPERATURE";
const char *strDispHumidity = "DISP_HUMIDITY";
const char *strDispLog = "DISP_LOG";
const char *strDispReed = "DISP_REED";
const char *strDispLight = "DISP_LIGHT";
 
 
 
 
void spiSetup (int spiChannel)
{
    if ((myFd = wiringPiSPISetup (spiChannel, 1000000)) < 0)
    {
        printf ("[%s] can't open the SPI bus\n", __FUNCTION__) ;
        exit (EXIT_FAILURE) ;
    }
}
 
 
 
 

 int myAnalogRead(int spiChannel, int channelConfig, int analogChannel)
{
	int spireturn, data;
   	unsigned char spidata[3];
   	
   	
   	spidata[0]= 1; 
   	spidata[1]= (0x80 | analogChannel << 4)  ;
   	spidata[2]= 0;
   	spireturn = wiringPiSPIDataRW(0, spidata , 3);
   	data = ((spidata[1] & 3) << 8) + spidata[2];
 
 	return(data);	
}




// callback for rece data
void funcPacketReqData(unsigned char *data, int szData)
{
 	memcpy(buf, data, szData);  
  	buf[szData] = 0;
  	printf("[%s][dbg](%d) ReqData= %s\n", __FUNCTION__, szData, buf);
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
  	strCmd += MLutility::generateChannelData(strCtrlLed, digitalRead(PIN_LED_CONTROL));
    
  	int nReedState = digitalRead(PIN_SENSOR_REED);
  	digitalWrite(PIN_REED_LED, nReedState);  
  	strCmd += "\n";
  	strCmd += MLutility::generateChannelData(strDispReed, nReedState);
  
  	int nLit = myAnalogRead(0, 8, 7);
  	strCmd += "\n";
  	strCmd += MLutility::generateChannelData(strDispLight, nLit);
  	lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(500);

  	sendUplink(strCtrlUpdate, "0");
}




void sendUplinkDHT(void) 
{
  	//dht.readSensor(fHumidity, fTemperature, false);
  	dht.readSensor(fHumidity, fTemperature);
  	fTemperature = dht.convertCtoF(fTemperature);
  
  	string strCmd = strDevice;
  	strCmd += ",";
  	strCmd += MLutility::generateChannelData(strDispHumidity, fHumidity);
  	strCmd += "\n";
  	strCmd += MLutility::generateChannelData(strDispTemperature, fTemperature);

  	lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(500);

  	printf("[%s][dbg] %d count chars\n", __FUNCTION__, strCmd.length());
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
  	
  	return bRet;
}




void funcPacketNotifyMcsCommand(unsigned char *data, int szData)
{
	memcpy(buf, data, szData);  
  	buf[szData] = 0;
  	printf("[%s][dbg](%d) NotifyMcsCommand= %s\n", __FUNCTION__, szData, buf);

  	int nVal = 0;
  	string strValue;
  	const char *strCtrl = NULL;
  
  	if (parseDownlink(strCtrlLed, nVal)) {
    	digitalWrite(PIN_LED_CONTROL, nVal);
    	strValue = strCtrlLed;
    	if (0 == nVal)
      		strValue += " off";
    	else
      		strValue += " on";    
  	}
  	else if (parseDownlink(strCtrlUpdate, nVal)) {
    	sendUplinkDHT();
    	refreshControlState();
  	}

  	// send log to MCS (as LoRa ack)
  	if (strValue.length() > 0) {
    	// add timestamp
    	unsigned long tsCurr = millis();
    	sprintf(strTmp, " (@%d)", tsCurr);
    	strValue += strTmp;
    	sendUplink(strDispLog, strValue.c_str());    
  	}
}




void setup() {
  	pinMode(PIN_SENSOR_REED, INPUT);
  	pinMode(PIN_REED_LED, OUTPUT);
  
  	pinMode(PIN_LIT_LED, PWM_OUTPUT);
  
  	pinMode(PIN_LED_CONTROL, OUTPUT);

  	digitalWrite(PIN_LED_CONTROL, HIGH);
  	digitalWrite(PIN_REED_LED, HIGH);
  	pwmWrite(PIN_LIT_LED, 1000);
    
    wiringPiSetup();
    spiSetup(0);
    int i, nLit;
    
    for (i = 0; i < 5; i++) {
    	nLit = myAnalogRead(0, 8, 7);
		printf("[%s] light sensor: %d\n", __FUNCTION__, nLit);
		delay(1000);
	}
    
  	lora.begin();
  	// custom LoRa config by your environment setting
  	lora.writeConfig(915555, 0, 0, 7, 5);
  	lora.setMode(E_LORA_POWERSAVING);         // E_LORA_POWERSAVING

  	delay(1000);

  	// custom callback
  	lora.setCallbackPacketReqData(funcPacketReqData);
  	lora.setCallbackPacketNotifyMcsCommand(funcPacketNotifyMcsCommand);

  	// init sensor for humidity & temperature
  	dht.begin();
  	boolean bReadDHT = false;
  	while (!bReadDHT && i < 8) {
    	delay(700);
    	bReadDHT = dht.readSensor(fHumidity, fTemperature);
    	i++;
  	}
  	
  	// login MCS
  	lora.sendPacketReqLoginMCS((uint8_t*)strDevice, strlen(strDevice));

  	// init MCS control state:
  	refreshControlState();	
}




void *loop(void *arg) 
{
	int nReedState, nLit, nLitLed;
	unsigned long tsCurr, tsCurr2, tsSensor2;
	
	
	tsSensor  = millis();
	tsSensor2 = millis();
	while(1) {
  		lora.run();
  		delay(100);
  		
  		tsCurr2 = millis();
  		if (tsCurr2 > tsSensor2 + 1000) {
  			tsSensor2 = tsCurr2;
  			
  			// reed sensor
  			nReedState = digitalRead(PIN_SENSOR_REED);
  			digitalWrite(PIN_REED_LED, nReedState);
  		
  			nLit = myAnalogRead(0, 8, 7);
    		printf("[%s] light sensor: %d\n", __FUNCTION__, nLit);    
  			pwmWrite(PIN_LIT_LED, nLit);
  		}
  		
  		tsCurr = millis();
  		if (tsCurr > tsSensor + 5000) {
    		tsSensor = tsCurr;

    		dht.readSensor(fHumidity, fTemperature);
    		fTemperature = dht.convertCtoF(fTemperature);
    		
			printf("[%s] [dbg] temperature: %f *F, humidity: %f, timestamp: %d\n", __FUNCTION__, fTemperature, fHumidity, tsCurr);
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
      			refreshControlState();
      		}
      		else if (buf[1] == 's') {
        		dht.readSensor(fHumidity, fTemperature);
      		}
      		else if (buf[1] == '9') {
        		lora.sendPacketSendMCSCommand((buf + 2), strlen((char*) buf + 2));
      		}
    	}
 	}
 	
	return 1;
}





//////////////////////////////////////////////////////
// USG(CES) on MOSTLink protocol with myDevices Cayenne
// 
// Sensor connect to these pins (wiringPi pin) on Raspberry Pi:
//     1. blue-led(1): show light-sensor value from MCP3008
//     2. red-led(28): show reed-sensor(7) state
//     3. green-led(27): control by myDevices Cayenne
//     4. humidity-temperature sensor (3)
//
// myDevices Cayenne
//     1. sign in http://mydevices.com/cayenne/signin/
//     2. [Add New...]->[Device/Widget]->CAYENNE API(Bring Your Own Thing)
//     3. copy/paste: MQTT USERNAME, PASSWORD, CLIENT ID
//     4. Vertify and Upload this code to Raspberry Pi
//
// Command
//     "/1" relogin myDevices Cayenne
//     "/2" send uplink DHT to myDevices
//     "/4" send uplink control-status to myDevices
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

#define ID_UPDATE         15

#define PIN_LIT_LED       1

int szBuf = 0;
char buf[100] = {0};
char strTmp[32] = {0};
float fTemperature, fHumidity;
unsigned long tsSensor = millis();
int myFd;


// ****** NOTICE ******
// TODO: Before you run your application, you need a myDevices Cayenne account.
const char *MQTTUsername = "522dbae0-a20e-11e7-bba6-6918eb39b85e";
const char *MQTTPassword = "5d2b97e0a195561dac11654d6c7a5457e330def4";
const char *ClientID =     "eb709610-a5b4-11e7-a9b2-a5d7f5484bce";
 
 
 
 
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




void refreshControlState(void) 
{
	String strCmd;
  	int nVal = digitalRead(PIN_LED_CONTROL);
  	sprintf(strTmp, "%d", PIN_LED_CONTROL);
  	strCmd = strTmp;
  	strCmd += ",null,null,";
  	sprintf(strTmp, "%d", nVal);
  	strCmd += strTmp;
  	lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(500);
  
  	nVal = digitalRead(PIN_SENSOR_REED);
  	sprintf(strTmp, "%d", PIN_SENSOR_REED);
  	strCmd = strTmp;
  	strCmd += ",null,null,";
  	sprintf(strTmp, "%d", nVal);
  	strCmd += strTmp;
  	lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(500);
  
  	int nLit = myAnalogRead(0, 8, 7);
  	strCmd = "20,lum,lux,";
  	sprintf(strTmp, "%d", nLit);
  	strCmd += strTmp;
  	lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(500);

  	sprintf(strTmp, "%d", ID_UPDATE);
  	strCmd = strTmp;
  	strCmd += ",null,null,0";
  	lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  	delay(500);
}




void sendUplinkDHT(void) 
{
	dht.readSensor(fHumidity, fTemperature);

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
  
  	printf("[%s][dbg] %d count chars\n", __FUNCTION__, strCmd.length());
}




// callback for rece packet: notify myDevices Command
void funcPacketNotifyMydevicesCommand(unsigned char *data, int szData)
{
 	memcpy(buf, data, szData);  
  	buf[szData] = 0;
  	
  	printf("[%s][dbg] NotifyMydevicesCommand= %s\n", __FUNCTION__, buf);

  	const char *strCommon = strstr(buf, ",");
  	int nChannel = 0;
  	int nVal;
  	
  	if (strCommon != NULL) {
    	nChannel = atoi(buf);
    	nVal = atoi(strCommon + 1);
    
    	printf("[%s][dbg] #%d= %d\n", __FUNCTION__, nChannel, nVal);

    	if (nChannel == PIN_LED_CONTROL) {
      		digitalWrite(PIN_LED_CONTROL, nVal);
    	}
    	else if (nChannel == ID_UPDATE) {
      		sendUplinkDHT();
      		refreshControlState();      
    	}
  	}
}




void setup() {
	int i, nLit;
	
	
  	pinMode(PIN_SENSOR_REED, INPUT);
  	pinMode(PIN_REED_LED, OUTPUT);
  
  	pinMode(PIN_LIT_LED, PWM_OUTPUT);
  
  	pinMode(PIN_LED_CONTROL, OUTPUT);

  	digitalWrite(PIN_LED_CONTROL, HIGH);
  	digitalWrite(PIN_REED_LED, HIGH);
  	pwmWrite(PIN_LIT_LED, 1000);
    
    wiringPiSetup() ;
    spiSetup(0);
    
    for (i = 0; i < 5; i++) {
    	nLit = myAnalogRead(0, 8, 7);
		printf("[%s] light sensor: %d\n", __FUNCTION__, nLit);
		delay(1000);
	}
    
  	lora.begin();
  	// custom LoRa config by your environment setting
  	lora.writeConfig(915000, 0, 0, 7, 5);
  	lora.setMode(E_LORA_POWERSAVING);         // E_LORA_POWERSAVING

  	delay(1000);

  	// custom callback
  	lora.setCallbackPacketNotifyMydevicesCommand(funcPacketNotifyMydevicesCommand);

  	// init sensor for humidity & temperature
  	dht.begin();
  	i = 0;
  	boolean bReadDHT = false;
  	while (!bReadDHT && i < 8) {
    	delay(700);
    	bReadDHT = dht.readSensor(fHumidity, fTemperature);
    	i++;
  	}

  	// login myDevices
  	lora.sendPacketReqLoginMydevices(MQTTUsername, MQTTPassword, ClientID);	
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
  		
  			// light sensor
  			nLit = myAnalogRead(0, 8, 7);
    		printf("[%s] light sensor: %d\n", __FUNCTION__, nLit);    
  			pwmWrite(PIN_LIT_LED, nLit);
  		}
  		
  		tsCurr = millis();
  		if (tsCurr > tsSensor + 5000) {
    		tsSensor = tsCurr;
    		dht.readSensor(fHumidity, fTemperature);
    		fTemperature = dht.convertCtoF(fTemperature);
    		
			printf("[%s][dbg] temperature: %f *F, humidity: %f, timestamp: %d\n", __FUNCTION__, fTemperature, fHumidity, tsCurr);
  		}		
  	}
 
}




int main(void)
{
	//char buf[256];
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
      			refreshControlState();
      		}
      		else if (buf[1] == 's') {
      		}
    	}
 	}
 	
	return 1;
}





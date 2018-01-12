//////////////////////////////////////////////////////
// ThingSpeak project base on MOSTLink protocol
// upload sensors to cloud server
// https://thingspeak.com/login
//
// DHT dht(2, DHT22);         // for DHT22
// DHT dht(2, DHT11);         // for DHT11
//////////////////////////////////////////////////////

#include <stdio.h>
#include "MOSTLora_def.h"
#include "MOSTLora.h"


/////////////////////////////////////////////
// DHT sensor related:
#include "DHT.h"
DHT dht(3, DHT22); // DHT22, DHT11




MOSTLora lora;
float fTemperature, fHumidity;
const char *thingSpeakApiKey = "V9RD7HWSYAY6FDJX";    // ThingSpeak API-key


/////////////////////////////////////////////
// read DHT sensor: Temperature and Humidity
bool readSensorDHT(float &fHumi, float &fTemp, bool bShowResult)
{
    bool bRet = true;


    bRet = dht.readSensor(fHumi, fTemp);

	if (bShowResult) {
        printf("[%s] ts(%d)", __FUNCTION__, millis());
        if (bRet) {
            printf(", Humidity: %f %, Temperature: %f *C\n", fHumi, fTemp);
        }
        else {
            printf(", DHT Fail.\n");
        }
    }
	
    return bRet;
}




/////////////////////////////////////////////
// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
	printf("[%s] ReqData, sendPacketThingSpeak=  \n", __FUNCTION__);
	
  	readSensorDHT(fHumidity, fTemperature, true);
  	lora.sendPacketThingSpeak(thingSpeakApiKey, fHumidity, fTemperature, 33, 46, 59, 60, 70, 85);
}




void setup() {
	// put your setup code here, to run once:
  	printf("*** lora_04_thingspeak ***\n");
  
 	lora.begin();
  
  	// config setting: frequency, group, data-rate, power, wakeup-time
  	lora.writeConfig(915000, 0, 0, 7, 5);
  	lora.setMode(E_LORA_WAKEUP);

  	delay(1000);
  
  	// init sensor for humidity & temperature
  	dht.begin();
  	int i = 0;
  	boolean bReadDHT = false;
  	while (!bReadDHT && i < 8) {
    	bReadDHT = readSensorDHT(fHumidity, fTemperature, true);
    	delay(600);
    	i++;
  	}

  	// custom callback
  	lora.setCallbackPacketReqData(funcPacketReqData);
}




void loop() {
	while(1) {
  		lora.run();
  		delay(100);
  	}
}




int main(void)
{
	printf("[%s] invoke lora.begin\n", __FUNCTION__);
	setup();
	loop();
	
	return 1;
}





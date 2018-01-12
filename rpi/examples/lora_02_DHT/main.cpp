//////////////////////////////////////////////////////
// MOSTLink LoRa Raspberry Pi sample code
//
// Gateway request data to node, then node response it's 
// temperature and humidity to gateway
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




MOSTLora lora(6, 5, 4);
float fTemperature, fHumidity;




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
	printf("[%s] ReqData, answer= \n", __FUNCTION__);
	
  	readSensorDHT(fHumidity, fTemperature, true);
  	lora.sendPacketAnsData(fHumidity, fTemperature);
}




void setup() {
	// put your setup code here, to run once:
  	printf("*** lora_02_DHT ***\n");
  
  	lora.begin(); 
  
  	// config setting: frequency, group, data-rate, power, wakeup-time
  	lora.writeConfig(915000, 0, 0, 7, 5);
  	lora.setMode(E_LORA_POWERSAVING);
  
  	// set polling request callback
  	lora.setCallbackPacketReqData(funcPacketReqData);

  	// init sensor for humidity & temperature
  	dht.begin();
  	int i = 0;
  	boolean bReadDHT = false;
  
  	while (!bReadDHT && i < 8) {
    	bReadDHT = readSensorDHT(fHumidity, fTemperature, true);
    	delay(600);
    	i++;
  	}

  	lora.sendPacketAnsData(fHumidity, fTemperature);
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





//////////////////////////////////////////////////////
// Simple chat sample between Gateway <-> Lora
//
//    Gateway: chat by Send TEXT(HEX)
//    Lora Node: chat by application
//////////////////////////////////////////////////////
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include "MOSTLora_def.h"
#include "MOSTLora.h"




MOSTLora lora;
byte buf[100] = {0};


// callback for rece data
void funcCustomRece(unsigned char *data, int szData)
{ 
	printf("[%s][dbg] funcCustomRece\n", __FUNCTION__);
	
	int i;
	
	printf("[%s][dbg] rx: ", __FUNCTION__);
	for (i = 0; i < szData; i++)
		printf("%02X ", *(data + i));
	printf("\n");
}




// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
 	memcpy(buf, data, szData);  
  	buf[szData] = 0;
  	printf("[%s][dbg](%d) rx: %s\n", __FUNCTION__, szData, buf);
}




void setup() {
  	printf("[dbg] *** lora_03_chat ***\n");
  	
 	lora.begin();
  
  	// config setting: frequency, group, data-rate, power, wakeup-time
  	lora.writeConfig(915000, 0, 0, 7, 5);
  	lora.setMode(E_LORA_WAKEUP);         // E_LORA_WAKEUP

  	delay(1000);
  	lora.sendData("Hi, how are you.");

  	// custom callback
  	lora.setCallbackReceData(funcCustomRece);
  	lora.setCallbackPacketReqData(funcPacketReqData);
}




void *loop(void *arg) 
{
	while(1) {
  		lora.run();
  		delay(100);
  	}
}




int main(void)
{
	char buffer[256] = {0};
	pthread_t threadId;
	
	
	printf("[%s][dbg] invoke lora.begin\n", __FUNCTION__);
	setup();
	
	if (pthread_create(&threadId, NULL, loop, NULL) != 0)
		printf("[%s][dbg] fail to create loop thread\n", __FUNCTION__);
		
	while (1) {
		printf("\nchat>  ");
		//scanf("%[^\n]", buffer);
		gets(buffer);
		lora.sendData((char *) buffer);
 	}
	
	return 1;
}





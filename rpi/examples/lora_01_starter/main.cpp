#include "MOSTLora_def.h"
#include "MOSTLora.h"




#define LED_PIN   29
 
 
 
 
MOSTLora lora; 




// Callback method when getting request packet from a station gateway
void funcBlink(unsigned char *data, int szData) {
	delay(500);
	
	
	for (int i=0; i<10;i++) {
    	digitalWrite(LED_PIN, HIGH);
    	delay(200);
    	digitalWrite(LED_PIN, LOW);
    	delay(200);  
  	}
} 



 
void setup() { 
	pinMode(LED_PIN, OUTPUT);
  
	lora.begin(); 
  
	// config setting: frequency, group, data-rate, power, wakeup-time
	lora.writeConfig(915000, 0, 0, 7, 5);
	lora.setMode(E_LORA_POWERSAVING);
  
	// set polling request callback
	lora.setCallbackPacketReqData(funcBlink);
} 
  
  
  
  
void loop() {
	while (1) { 
		lora.run(); // lora handle input messages
		delay(100);
	} 
}   




int main(void)
{
	setup();
	loop();
	 
	return 1;
}





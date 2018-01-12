#include <stdio.h>	
#include <termios.h>
#include "rpiSerial.h"




RpiSerial rpiSerial;




RpiSerial::RpiSerial()
{
	if ((_fd = serialOpen(RPI_UART_TTY, RPI_UART_BAUDRATE)) < 0) {
    	printf("[%s] Unable to open serial device\n", __FUNCTION__) ;
		return;
  	}
  
  	printf("[%s] open raspberry pi serial device ok\n", __FUNCTION__);
  	
    if (wiringPiSetup() == -1) {
        printf("[%s] fail to invoke wiringPiSetup!\n", __FUNCTION__);
        return;
    }
    
    printf("[%s] invoke wiringPiSetup ok\n", __FUNCTION__);
}




void RpiSerial::begin(unsigned long baud)
{
	struct termios options;
	speed_t baudrate;


	printf("[%s]: set serial device baud rate: %d\n", __FUNCTION__, baud);
	
	if (baud == 0)           baudrate = B0;
	else if (baud == 50)     baudrate = B50;
	else if (baud == 75)     baudrate = B75;
	else if (baud == 110)    baudrate = B110;	
	else if (baud == 134)    baudrate = B134;
	else if (baud == 150)    baudrate = B150;
	else if (baud == 200)    baudrate = B200;
	else if (baud == 300)    baudrate = B300;
	else if (baud == 600)    baudrate = B600;
	else if (baud == 1200)   baudrate = B1200;
	else if (baud == 1800)   baudrate = B1800;
	else if (baud == 2400)   baudrate = B2400;
	else if (baud == 4800)   baudrate = B4800;
	else if (baud == 9600)   baudrate = B9600;
	else if (baud == 19200)  baudrate = B19200;
	else if (baud == 38400)  baudrate = B38400;
	else if (baud == 57600)  baudrate = B57600;
	else if (baud == 115200) baudrate = B115200;
	else                     baudrate = B0;
	
  	tcgetattr(_fd, &options) ;   // Read current options
  	//options.c_cflag &= ~CSIZE ;  // Mask out size
  	//options.c_cflag |= CS7 ;     // Or in 7-bits
  	//options.c_cflag |= PARENB ;  // Enable Parity - even by default
  	//tcsetattr(_fd, &options) ;   // Set new options	
  	
  	if (cfsetispeed(&options, baudrate) == -1)
		printf("[%s]: set input rate error!\n", __FUNCTION__);

	if (cfsetospeed(&options, baudrate) == -1)
		printf("[%s]: set output rate error!\n", __FUNCTION__);
		
	if (tcsetattr(_fd, TCSANOW, &options) == -1)
		printf("[%s]: set config error!\n", __FUNCTION__);

	printf("[%s]: set serial device config ok!\n", __FUNCTION__);
}




int RpiSerial::available(void)
{
	return serialDataAvail (_fd);
}




int RpiSerial::read(void)
{
	return serialGetchar(_fd);
}




int RpiSerial::write(unsigned char *cmd, int szCmd)
{
	int i;
	
	for (i = 0; i < szCmd; i++)
    	serialPutchar(_fd, cmd[i]);
    	
	return 1;
}
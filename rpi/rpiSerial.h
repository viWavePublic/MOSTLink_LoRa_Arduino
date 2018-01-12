#ifndef _RPISERIAL_H_
#define _RPISERIAL_H_




#include <wiringPi.h>
#include <wiringSerial.h>
//#include "MOSTLora_def.h"




#define RPI_UART_BAUDRATE    9600
#define RPI_UART_TTY         "/dev/ttyAMA0"




class RpiSerial
{
private:
	int _fd;
	
public:
    RpiSerial();
    void begin(unsigned long);
    
    //RpiSerial() {}
    //void begin(unsigned long baud) {}
    //void end() {}
    virtual int available(void);
    virtual int read(void);
    virtual int write(unsigned char *, int);
    
#if 0    
    virtual size_t write(uint8_t) {}
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    
    size_t print(const __FlashStringHelper *) { return 0; }
    size_t print(const String &) { return 0; }
    size_t print(const char[]) { return 0; }
    size_t print(char) { return 0; }
    size_t print(unsigned char, int = DEC) { return 0; }
    size_t print(int, int = DEC) { return 0; }
    size_t print(unsigned int, int = DEC) { return 0; }
    size_t print(long, int = DEC) { return 0; }
    size_t print(unsigned long, int = DEC) { return 0; }
    size_t print(double, int = 2) { return 0; }
    size_t print(const Printable&) { return 0; }
    
    size_t println(const __FlashStringHelper *) { return 0; }
    size_t println(const String &s) { return 0; }
    size_t println(const char[]) { return 0; }
    size_t println(char) { return 0; }
    size_t println(unsigned char, int = DEC) { return 0; }
    size_t println(int, int = DEC) { return 0; }
    size_t println(unsigned int, int = DEC) { return 0; }
    size_t println(long, int = DEC) { return 0; }
    size_t println(unsigned long, int = DEC) { return 0; }
    size_t println(double, int = 2) { return 0; }
    size_t println(const Printable&) { return 0; }
    size_t println(void) { return 0; }
#endif   
};




extern RpiSerial rpiSerial;




#endif  // _RSPISERIAL_H_



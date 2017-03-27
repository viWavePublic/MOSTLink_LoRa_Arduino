/*
  Library for MOSTLink LoRa Shield Library
 
  MOSTLink LoRa Shield with Semtech sx1276 for LoRa technology.
  Support device: Arduino UNO, Linkit One, Vinduino
 
  2016 Copyright (c) viWave Co. Ltd., All right reserved.
  http://mostlink.viwave.com/

  Original Author: Macbear Chen, Auguest 12, 2016.
  --------------------------------------------------------------

*/

#ifndef __MOSTLora_def_h
#define __MOSTLora_def_h

#include <Arduino.h>

///////////////////////
// crypto encrypt/decrypt for AES128/CBC/NoPadding, HMAC for challenge-response
///////////////////////
//#define USE_LIB_CRYPTO_AES128
//#define USE_LIB_CRYPTO_HMAC

///////////////////////
// Arduino board
///////////////////////
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)

#elif defined(ARDUINO_AVR_PRO)  // vinduino pro mini

#define USE_VINDUINO  // for Vinduino project (unmark the line)

#elif defined(ARDUINO_AVR_MINI) //

#elif defined(__LINKIT_ONE__)   // for LinkIt ONE

#else // other Arduino board

// unsupported board
#error "ARDUINO BOARD NOT SUPPORTED"

#endif // for Arduino board

///////////////////////
// debug for Serial monitor
///////////////////////
#ifndef USE_VINDUINO     // debug only for non-Vinduino project

#define DEBUG_LORA     // <--- debug by Serial Monitor, NOTICE: remark for RELEASE

#endif // USE_VINDUINO

// debug LoRa to serial-monitor
#ifdef DEBUG_LORA
#define debugSerial Serial
#else // DEBUG_LORA

#include "DummySerial.h"
#define debugSerial dummySerial

#endif // DEBUG_LORA

#endif // __MOSTLora_def_h


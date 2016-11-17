// MOSTLink LoRa
#include "MOSTLora.h"
#include "MLutility.h"

#if !defined(__LINKIT_ONE__)
#include <MemoryFree.h>
#endif // __LINKIT_ONE__

MOSTLora lora;

#include <Crypto.h>
#include <AES.h>
#include <Speck.h>
#include <SpeckTiny.h>
#include <GCM.h>
#include <avr/pgmspace.h>

#define KEY_SIZE 16
#define IV_SIZE 4
#define MAX_PLAINTEXT_LEN 64
#define TAG_SIZE 16

struct TestVector {
    byte key[KEY_SIZE];
    byte text[MAX_PLAINTEXT_LEN];
    byte iv[IV_SIZE];
    size_t datasize;
};

static TestVector const testVectorGCM PROGMEM = {
    .key         = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
                    0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08},
    .text        = {1, 2, 3, 4, 5, 6, 7, 8,
                    0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x77,
                    0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
                    0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
                    0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
                    0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
                    0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
                    0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55},
    .iv          = {0xca, 0xfe, 0xba, 0xbe},
    .datasize    = 16
};

TestVector testVector;

void setup()
{
    Serial.begin(9600);

    memcpy_P(&testVector, &testVectorGCM, sizeof(TestVector));
//    MLutility::encryptAES(testVector.text, testVector.datasize, testVector.key, testVector.iv);
//    MLutility::decryptAES(testVector.text, testVector.datasize, testVector.key, testVector.iv);

                char strKey[16] = "0123456789012345";
                char strIv[4] = "ABCD";
                char strDat[16] = "Hello, 01234567";

    Serial.println(F("REQ_AUTH_JOIN"));
    MLutility::encryptAES_CBC(testVector.text, testVector.datasize, testVector.key, testVector.iv);
    MLutility::decryptAES_CBC(testVector.text, testVector.datasize, testVector.key, testVector.iv);

//    MLutility::encryptAES_CBC(strDat, 16, strKey, strIv);
//    MLutility::decryptAES_CBC(strDat, 16, strKey, strIv);

    
    lora.begin();
  // custom LoRa config by your environment setting
//  lora.writeConfig(915555, 0, 0, 7, 5);
//    Serial.println(F("REQ_AUTH_JOIN"));
    lora.sendPacketReqAuthJoin();

//    Serial.print(gcmaes128->keySize());
#if !defined(__LINKIT_ONE__)
    Serial.print(F(" Free mem:"));
    Serial.println(freeMemory());

#endif // __LINKIT_ONE__

}

void loop()
{
  lora.run();
}

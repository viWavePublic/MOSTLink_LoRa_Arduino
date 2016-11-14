// MOSTLink LoRa
#include "MOSTLora.h"
#include "MLutility.h"
//#include <MemoryFree.h>

MOSTLora lora;

#include <Crypto.h>
#include <AES.h>
#include <Speck.h>
#include <SpeckTiny.h>
#include <GCM.h>
#include <avr/pgmspace.h>

#define KEY_SIZE 32
#define IV_SIZE 4
#define MAX_PLAINTEXT_LEN 64
#define TAG_SIZE 16
/*
void printBytes(byte b[], int len) {
 int i;
 for (i=0; i<len; i++){
   Serial.print("0x");
   if (b[i] <= 16) Serial.print("0");
   Serial.print(b[i], 16);
   Serial.print(", ");
 }
 Serial.println();
}
*/
struct TestVector {
    byte key[KEY_SIZE];
    byte text[MAX_PLAINTEXT_LEN];
    byte iv[IV_SIZE];
    size_t datasize;
};

static TestVector const testVectorGCM PROGMEM = {
    .key         = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
                    0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08},
    .text        = {0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
                    0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
                    0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
                    0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
                    0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
                    0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
                    0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
                    0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55},
    .iv          = {0xca, 0xfe, 0xba, 0xbe},
    .datasize    = 64
};

static TestVector const testDecryptVectorGCM PROGMEM = {
    .key         = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
                    0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08},
    .text        = {0xbc, 0x74, 0x8b, 0x61, 0xfb, 0xca, 0x0c, 0x95,
                    0x64, 0x3e, 0x9a, 0xb2, 0x52, 0x9c, 0xcb, 0x88,
                    0xc3, 0xe0, 0xad, 0xe7, 0xaa, 0x3f, 0x73, 0xa2,
                    0xac, 0xb2, 0x14, 0x21, 0x01, 0x14, 0xd8, 0x8f,
                    0x20, 0x2e, 0x54, 0x35, 0xff, 0x26, 0x66, 0x79,
                    0x74, 0x3f, 0x2a, 0x0f, 0xe8, 0xaf, 0x82, 0x5e,
                    0x87, 0xff, 0x7d, 0x1d, 0x9c, 0x4d, 0xa6, 0xbb,
                    0x4c, 0xfb, 0xcb, 0x0d, 0x7d, 0xfc, 0x69, 0x37},
    .iv          = {0xca, 0xfe, 0xba, 0xbe},
    .datasize    = 64
};

TestVector testVector;

GCM<AES128> *gcmaes128 = 0;

byte buffer[MAX_PLAINTEXT_LEN];
byte res_tag[TAG_SIZE];

void crypt(AuthenticatedCipher *cipher, const struct TestVector *test, int control) {
    memcpy_P(&testVector, test, sizeof(TestVector));
    test = &testVector;
    cipher->clear();
    cipher->setKey(test->key, cipher->keySize());
    cipher->setIV(test->iv, IV_SIZE);
    memset(buffer, 0xBA, sizeof(buffer));
    if (control == 1) {
        cipher->encrypt(buffer, test->text, test->datasize);
        cipher->computeTag(res_tag, sizeof(res_tag));
//        printBytes(buffer,test->datasize);
//        printBytes(res_tag,TAG_SIZE);
    }
    if (control == 2) {
        cipher->setKey(test->key, cipher->keySize());
        cipher->setIV(test->iv, IV_SIZE);
        cipher->decrypt(buffer, test->text, test->datasize);
//        printBytes(buffer,test->datasize);
    }
}

void setup()
{
    Serial.begin(9600);
    gcmaes128 = new GCM<AES128>();
    crypt(gcmaes128, &testVectorGCM, 1);
    crypt(gcmaes128, &testDecryptVectorGCM, 2);

    lora.begin();
  // custom LoRa config by your environment setting
//  lora.writeConfig(915555, 0, 0, 7, 5);
//    Serial.println(F("REQ_AUTH_JOIN"));
    lora.sendPacketReqAuthJoin();

//    Serial.println(freeMemory());
}

void loop()
{
  lora.run();
}

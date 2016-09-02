#include <stdio.h>
#include "../MLPacketParser.h"

const uint8_t packet[] = {
0xFB,0xFC,0x0A,0x4B,0x08,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x01,0x9B,0x66,0x7C,
0x0E,0x0B,0x00,0x04,0x80,0xB0,0x0A,0x10,0x02,0x30,0x47,0x46,0x55,0x47,0x45,0x33,
0x37,0x31,0x57,0x4E,0x50,0x4D,0x4D,0x4A,0x45,0x45,0xCF,0x08,0x00,0x46,0xD9,0x8A,
0x99,0x46,0x27,0x3C,0x00,0x46,0x1A,0xF0,0x00,0x40,0x86,0x14,0x7B,0x42,0x9A,0x70,
0xA4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x57 };

const uint8_t REQ_DATA_PK[] = {
0xFB,0xFC,0x0A,0x1A,0x01,0x9B,0x66,0x7C,0x0F,0x17,0x00,0x03,0x00,0x8C,0x0A,0x00,
0x04,0x00,0x0A,0x04,0x74,0x65,0x73,0x74,0x00,0x16 };


const uint8_t RES_DATA_PK[] = {
0xFB,0xFC,0x0A,0x25,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x9B,0x66,0x7C,
0x11,0x2C,0x00,0x26,0x80,0x00,0x0A,0x02,0x02,0x00,0x08,0x42,0x10,0x00,0x00,0x41,
0xD6,0x66,0x66,0x00,0x00 };

int main()
{
    MLPacketCtx pkctx;
    MLPacketParser pkParser;

    pkParser.mostloraPacketParse(&pkctx, packet);
    pkParser.mostloraPacketParse(&pkctx, REQ_DATA_PK);
    pkParser.mostloraPacketParse(&pkctx, RES_DATA_PK);

    return 0;
}

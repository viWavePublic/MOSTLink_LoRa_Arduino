/*
  MOSTLora.h - Library for MOST Link Lora
  MOST-Link protocol

  Created by Macbear Chen, Auguest 12, 2016.
  Released into the public domain.
*/

#ifndef __MLpacket_h
#define __MLpacket_h

// downlink packet header for MOST Link (14 bytes)
struct MLDownlink {

    unsigned char preamble[2];      // 0xFB FC
    unsigned char version;
    unsigned char length;
    // --------
    unsigned char flag;             // 101H
    unsigned char receiver_id[8];
    unsigned char headerCrc;
};

// uplink packet header for MOST Link (22 bytes)
struct MLUplink {
    unsigned char preamble[2];      // 0xFB FC
    unsigned char version;
    unsigned char length;
    // --------
    unsigned char flag;             // 101H
    unsigned char receiver_id[8];
    unsigned char sender_id[8];
    unsigned char packetCrc;
};

#endif // __MLpacket_h


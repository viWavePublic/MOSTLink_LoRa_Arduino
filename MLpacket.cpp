/* viWave LoRa Module by GlobalSat

	MOST-Link protocol
	
	
 */

#include <Arduino.h>
#include "MLpacket.h"
#include "MLPacketComm.h"

MLDownlink::MLDownlink(unsigned char ver, unsigned char len, unsigned char flg, unsigned char *r_id)
{
    preamble[0] = 0xFB;
    preamble[1] = 0xFC;
    version = ver;
    length = len;
    flag = flg;
    memcpy(receiver_id, r_id, 8);

    headerCrc = getCrc((byte*)this, sizeof(MLDownlink) - 1);      // header CRC
}

MLUplink::MLUplink(unsigned char ver, unsigned char len, unsigned char flg, unsigned char *s_id, unsigned char *r_id)
{
    preamble[0] = 0xFB;
    preamble[1] = 0xFC;
    version = ver;
    length = len;
    flag = flg;
    memcpy(sender_id, s_id, 8);
    if (r_id != NULL)
        memcpy(receiver_id, r_id, 8);
    else
        memset(receiver_id, 0xFF, 8);
    
    headerCrc = getCrc((byte*)this, sizeof(MLUplink) - 1);      // header CRC
}

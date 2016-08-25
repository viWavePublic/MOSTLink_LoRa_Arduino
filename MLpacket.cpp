/* viWave LoRa Module by GlobalSat

	MOST-Link protocol
	
	
 */

#include "MLpacket.h"

MLDownlink::MLDownlink() {
    preamble[0] = 0xFB;
    preamble[1] = 0xFC;
    version = 0x0A;
    length = 14;
    flag = 0;
    int i;
    for (i = 0; i < 8; i++) {
        receiver_id[i] = 0xFF;
    }
    headerCrc = 0;
}

MLUplink::MLUplink()
{
    preamble[0] = 0xFB;
    preamble[1] = 0xFC;
    version = 0x0A;
    length = 22;
    flag = 0;
    int i;
    for (i = 0; i < 8; i++) {
        sender_id[i] = 0xFF;
        receiver_id[i] = 0xFF;
    }
    headerCrc = 0;
}

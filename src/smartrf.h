
#ifndef _SMARTRF_H_
#define _SMARTRF_H_

/* Deviation = 20.629883 */
/* Base frequency = 867.999939 */
/* Carrier frequency = 867.999939 */
/* Channel number = 0 */
/* Carrier frequency = 867.999939 */
/* Modulated = true */
/* Modulation format = 2-GFSK */
/* Manchester enable = false */
/* Sync word qualifier mode = 30/32 sync word bits detected */
/* Preamble count = 4 */
/* Channel spacing = 199.951172 */
/* Carrier frequency = 867.999939 */
/* Data rate = 38.3835 */
/* RX filter BW = 101.562500 */
/* Data format = Normal mode */
/* CRC enable = true */
/* Whitening = false */
/* Device address = 0 */
/* Address config = No address check */
/* CRC autoflush = false */
/* PA ramping = false */
/* TX power = 0 */

#define SMARTRF_IOCFG2     0x29
#define SMARTRF_IOCFG1     0x2E
#define SMARTRF_IOCFG0     0x06
#define SMARTRF_FIFOTHR    0x47
#define SMARTRF_SYNC1      0xD3
#define SMARTRF_SYNC0      0x91
#define SMARTRF_PKTLEN     0x0A /* Changed to 0x0A. Def = 0xFF */
#define SMARTRF_PKTCTRL1   0x04
#define SMARTRF_PKTCTRL0   0x05
#define SMARTRF_ADDR       0x00
#define SMARTRF_CHANNR     0x00
#define SMARTRF_FSCTRL1    0x06
#define SMARTRF_FSCTRL0    0x00
#define SMARTRF_FREQ2      0x21
#define SMARTRF_FREQ1      0x62
#define SMARTRF_FREQ0      0x76
#define SMARTRF_MDMCFG4    0xCA
#define SMARTRF_MDMCFG3    0x83
#define SMARTRF_MDMCFG2    0x13
#define SMARTRF_MDMCFG1    0x22
#define SMARTRF_MDMCFG0    0xF8
#define SMARTRF_DEVIATN    0x35
#define SMARTRF_MCSM2      0x07
#define SMARTRF_MCSM1      0x3C /* Changed to 0x3C. When a packet has been
                                   received: Stay in RX. Def = 0x30 */
#define SMARTRF_MCSM0      0x10
#define SMARTRF_FOCCFG     0x16
#define SMARTRF_BSCFG      0x6C
#define SMARTRF_AGCCTRL2   0x43
#define SMARTRF_AGCCTRL1   0x40
#define SMARTRF_AGCCTRL0   0x91
#define SMARTRF_WOREVT1    0x87
#define SMARTRF_WOREVT0    0x6B
#define SMARTRF_WORCTRL    0xFB
#define SMARTRF_FREND1     0x56
#define SMARTRF_FREND0     0x10
#define SMARTRF_FSCAL3     0xE9
#define SMARTRF_FSCAL2     0x2A
#define SMARTRF_FSCAL1     0x00
#define SMARTRF_FSCAL0     0x1F
#define SMARTRF_FSTEST     0x59
#define SMARTRF_PTEST      0x7F
#define SMARTRF_AGCTEST    0x3F
#define SMARTRF_TEST2      0x81
#define SMARTRF_TEST1      0x35
#define SMARTRF_TEST0      0x09

#endif /* _SMARTRF_H_ */
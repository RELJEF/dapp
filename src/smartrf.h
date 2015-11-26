
#ifndef _SMARTRF_H_
#define _SMARTRF_H_

/* Data rate = 38.3835 */

#define SMARTRF_IOCFG2     0x29
#define SMARTRF_IOCFG1     0x2E
#define SMARTRF_IOCFG0     0x06
#define SMARTRF_FIFOTHR    0x47 /* Maximum bytes in TX FIFO: 33; Maximum bytes
                                 * in RX FIFO: 32 */
#define SMARTRF_SYNC1      0xD3
#define SMARTRF_SYNC0      0x91
#define SMARTRF_PKTLEN     0x3C /* 60 */
#define SMARTRF_PKTCTRL1   0x04 /* PQT = 0 (sync word always accepted); CRC
                                 * autoflush: disabled; Append status bytes in
                                 * RX packet: enabled; Address check:
                                 * disabled */
#define SMARTRF_PKTCTRL0   0x44 /* Data whitening: enabled; Packet length:
                                 * fixed; Packet format: normal (use RX and TX
                                 * FIFOs); CRC: enabled */
#define SMARTRF_ADDR       0x00 /* Device address: 0 */
#define SMARTRF_CHANNR     0x00 /* Channel number: 0 */
#define SMARTRF_FSCTRL1    0x08 /* IF frequency: 204 kHz */
#define SMARTRF_FSCTRL0    0x00 /* Frequency offset: 0 */
#define SMARTRF_FREQ2      0x21 /* Carrier frequency: 867.999939 MHz */
#define SMARTRF_FREQ1      0x62
#define SMARTRF_FREQ0      0x76
#define SMARTRF_MDMCFG4    0xCA /* RX filter bandwidth: 101.562500 kHz */
#define SMARTRF_MDMCFG3    0x83 /* Mantissa of the user specified symbol rate */
#define SMARTRF_MDMCFG2    0x93 /* Digital DC blocking filter: disabled;
                                 * Modulation: 2-GFSK; Manchester coding:
                                 * disabled; Sync mode: 30/32 sync word bits
                                 * detected */
#define SMARTRF_MDMCFG1    0x22 /* Minimum preamble bytes to be sent: 4;
                                 * Channel spacing exponent: 2 */
#define SMARTRF_MDMCFG0    0xF8 /* Channel spacing mantissa */
#define SMARTRF_DEVIATN    0x34 /* Deviation: 19 kHz */
#define SMARTRF_MCSM2      0x07 /* Timeout for sync word search in RX:
                                 * disabled */
#define SMARTRF_MCSM1      0x3C /* Clear channel indication: if RSSI is below
                                 * threshold unless currently receiving packet;
                                 * When a packet has been received stay in RX;
                                 * When a packet has been transmitted go to
                                 * IDLE */
#define SMARTRF_MCSM0      0x10 /* Automatically calibrate when going from
                                 * from IDLE to RX or TX (or FSTXON) */
#define SMARTRF_FOCCFG     0x16 /* Frequency compensation loop gain before sync
                                 * word is detected: 3K; Saturation point for
                                 * the frequency offset compensation algorithm:
                                 * +/- BANDWIDTH/8 */
#define SMARTRF_BSCFG      0x6C /* Clock recovery loop integral gain before sync
                                 * word: 2Ki; Clock recovery loop proportional
                                 * gain before sync word: 3Kp; Clock recovery
                                 * loop integral gain before after word: Ki/2;
                                 * Clock recovery loop proportional ain after
                                 * sync word: 3Kp */
#define SMARTRF_AGCCTRL2   0x43
#define SMARTRF_AGCCTRL1   0x40
#define SMARTRF_AGCCTRL0   0x91
#define SMARTRF_WOREVT1    0x87
#define SMARTRF_WOREVT0    0x6B
#define SMARTRF_WORCTRL    0xFB /* ACLK input to the WOR timer: disabled;
                                 * Event1 timeout: 1.465 ms; Maximum WOR and
                                 * regular RX operation timeout: 18.2 hours */
#define SMARTRF_FREND1     0x56
#define SMARTRF_FREND0     0x10 /* RF output power: 0 dBm */
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
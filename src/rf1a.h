
#ifndef _RF1A_H_
#define _RF1A_H_

typedef struct {
    unsigned char fsctrl1;   /* Frequency synthesizer control. */
    unsigned char fsctrl0;   /* Frequency synthesizer control. */
    unsigned char freq2;     /* Frequency control word, high byte. */
    unsigned char freq1;     /* Frequency control word, middle byte. */
    unsigned char freq0;     /* Frequency control word, low byte. */
    unsigned char mdmcfg4;   /* Modem configuration. */
    unsigned char mdmcfg3;   /* Modem configuration. */
    unsigned char mdmcfg2;   /* Modem configuration. */
    unsigned char mdmcfg1;   /* Modem configuration. */
    unsigned char mdmcfg0;   /* Modem configuration. */
    unsigned char channr;    /* Channel number. */
    unsigned char deviatn;   /* Modem deviation setting (when FSK modulation
                                is enabled). */
    unsigned char frend1;    /* Front end RX configuration. */
    unsigned char frend0;    /* Front end RX configuration. */
    unsigned char mcsm0;     /* Main Radio Control State Machine
                                configuration. */
    unsigned char foccfg;    /* Frequency Offset Compensation Configuration. */
    unsigned char bscfg;     /* Bit synchronization Configuration. */
    unsigned char agcctrl2;  /* AGC control. */
    unsigned char agcctrl1;  /* AGC control. */
    unsigned char agcctrl0;  /* AGC control. */
    unsigned char fscal3;    /* Frequency synthesizer calibration. */
    unsigned char fscal2;    /* Frequency synthesizer calibration. */
    unsigned char fscal1;    /* Frequency synthesizer calibration. */
    unsigned char fscal0;    /* Frequency synthesizer calibration. */
    unsigned char fstest;    /* Frequency synthesizer calibration control. */
    unsigned char test2;     /* Various test settings. */
    unsigned char test1;     /* Various test settings. */
    unsigned char test0;     /* Various test settings. */
    unsigned char fifothr;   /* RXFIFO and TXFIFO thresholds. */
    unsigned char iocfg2;    /* GDO2 output pin configuration. */
    unsigned char iocfg0;    /* GDO0 output pin configuration. */
    unsigned char pktctrl1;  /* Packet automation control. */
    unsigned char pktctrl0;  /* Packet automation control. */
    unsigned char addr;      /* Device address. */
    unsigned char pktlen;    /* Packet length. */
} rf_settings_t;



unsigned char   read_single_reg( unsigned char addr );
void            read_burst_reg( unsigned char addr,
                                unsigned char* buffer,
                                unsigned char count );
void            write_single_reg( unsigned char addr, unsigned char value );
void            write_burst_reg( unsigned char addr,
                                 unsigned char* buffer,
                                 unsigned char count );
unsigned char   strobe( unsigned char s );
void            reset_radio_core( void );
void            write_rf_settings( rf_settings_t* rf_settings );
void            write_single_pa_table( unsigned char value );
void            write_burst_pa_table( unsigned char* buffer,
                                      unsigned char count );

#endif /* _RF1A_H_ */

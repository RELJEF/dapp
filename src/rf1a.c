
#include "main.h"


const uint8_t rf1a_regs_config[ RF1A_REGS_CONF_SIZE ] =
{
    SMARTRF_IOCFG2,     /* IOCFG2: GDO2 signals on RF_RDYn */
    SMARTRF_IOCFG1,     /* IOCFG1: GDO1 signals on RSSI_VALID */
    SMARTRF_IOCFG0,     /* IOCFG0: GDO0 signals on PA power down signal to
                         * control RX/TX switch */
    SMARTRF_FIFOTHR,    /* FIFOTHR: RX/TX FIFO threshold: 33 bytes in TX, 32
                         * bytes in RX */
    SMARTRF_SYNC1,      /* SYNC1: High byte of sync word */
    SMARTRF_SYNC0,      /* SYNC0: Low byte of sync word */
    SMARTRF_PKTLEN,     /* PKTLEN: Packet length in fixed mode, maximum length
                         * in variable-length mode */
    SMARTRF_PKTCTRL1,   /* PKTCTRL1: No status bytes appended to the packet */
    SMARTRF_PKTCTRL0,   /* PKTCTRL0: Fixed-Length Mode, No CRC */
    SMARTRF_ADDR,       /* ADDR: Address for packet filtration */
    SMARTRF_CHANNR,     /* CHANNR: 8-bit channel number,
                         * freq = base freq + CHANNR * channel spacing */
    SMARTRF_FSCTRL1,    /* FSCTRL1: Frequency Synthesizer Control */
    SMARTRF_FSCTRL0,    /* FSCTRL0: Frequency Synthesizer Control */
    SMARTRF_FREQ2,      /* FREQ2: Base frequency, high byte */
    SMARTRF_FREQ1,      /* FREQ1: Base frequency, middle byte */
    SMARTRF_FREQ0,      /* FREQ0: Base frequency, low byte */
    SMARTRF_MDMCFG4,    /* MDMCFG4: Modem configuration */
    SMARTRF_MDMCFG3,    /* MDMCFG3:                "                      " */
    SMARTRF_MDMCFG2,    /* MDMCFG2:                "                      " */
    SMARTRF_MDMCFG1,    /* MDMCFG1:                "                      " */
    SMARTRF_MDMCFG0,    /* MDMCFG0:                "                      " */
    SMARTRF_DEVIATN,    /* DEVIATN: Modem deviation setting */
    SMARTRF_MCSM2,      /* MCSM2: Main radio control state machine
                         * conf. : timeout for sync word search disabled */
    SMARTRF_MCSM1,      /* MCSM1: CCA signals when RSSI below threshold, stay
                         * in RX after packet has been received */
    SMARTRF_MCSM0,      /* MCSM0: Auto-calibrate when going from IDLE to RX
                         * or TX (or FSTXON ) */
    SMARTRF_FOCCFG,     /* FOCCFG: Frequency offset compensation conf. */
    SMARTRF_BSCFG,      /* BSCFG: Bit synchronization conf. */
    SMARTRF_AGCCTRL2,   /* AGCCTRL2: AGC Control */
    SMARTRF_AGCCTRL1,   /* AGCCTRL1:     " */
    SMARTRF_AGCCTRL0,   /* AGCCTRL0:     " */
    SMARTRF_WOREVT1,    /* WOREVT1: High byte Event0 timeout */
    SMARTRF_WOREVT0,    /* WOREVT0: High byte Event0 timeout */
    SMARTRF_WORCTRL,    /* WORCTL: Wake on radio control ****Feature
                         * unavailable in PG0.6**** */
    SMARTRF_FREND1,     /* FREND1: Front end RX conf. */
    SMARTRF_FREND0,     /* FREND0: Front end TX conf. */
    SMARTRF_FSCAL3,     /* FSCAL3: Frequency synthesizer calibration */
    SMARTRF_FSCAL2,     /* FSCAL2:              " */
    SMARTRF_FSCAL1,     /* FSCAL1:              " */
    SMARTRF_FSCAL0,     /* FSCAL0:              " */
    0x00,               /* Reserved *read as 0* */
    0x00,               /* Reserved *read as 0* */
    SMARTRF_FSTEST,     /* FSTEST: Irrelevant for normal use case */
    SMARTRF_PTEST,      /* PTEST: Irrelevant for normal use case */
    SMARTRF_AGCTEST,    /* AGCTEST: Irrelevant for normal use case */
    SMARTRF_TEST2,      /* TEST2: Irrelevant for normal use case */
    SMARTRF_TEST1,      /* TEST1: Irrelevant for normal use case */
    SMARTRF_TEST0       /* TEST0: Irrelevant for normal use case */
};


/* Read a single byte from the radio register. */
uint8_t read_single_reg( uint8_t addr )
{
    uint8_t data_out;

    /* Check for valid configuration register address, 0x3E refers to PATABLE */
    if( ( addr <= 0x2E ) || ( addr == 0x3E ) ) {
        /* Send address + instruction + 1 dummy byte (auto-read) */
        RF1AINSTR1B = ( addr | RF_SNGLREGRD );    
    } else {
        /* Send address + instruction + 1 dummy byte (auto-read) */
        RF1AINSTR1B = ( addr | RF_STATREGRD );
    }

    while( !( RF1AIFCTL1 & RFDOUTIFG ) ) {
        ;
    }
    
    data_out = RF1ADOUTB; /* Read data and clear the RFDOUTIFG. */

    return data_out;
}

/* Read multiple bytes to the radio registers. */
void read_burst_reg( uint8_t addr, uint8_t* buffer, uint8_t count )
{
    uint8_t i;
    
    if( count > 0 ) {
        /* Wait for INSTRIFG. */
        while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
            ;
        }
        
        /* Send addr of first conf. reg. to be read
           and the burst-register read instruction. */
        RF1AINSTR1B = ( addr | RF_REGRD );  

        for( i = 0; i < ( count - 1 ); i++ ) {
            /* Wait for the Radio Core to update the RF1ADOUTB reg. */
            while( !( RFDOUTIFG & RF1AIFCTL1 ) ) {
                ;
            }
            
            /* Reads DOUT from radio core + clears RFDOUTIFG.
             * Also initiates auto-read for next DOUT byte. */
            buffer[i] = RF1ADOUT1B;
        }
        
        /* Store the last DOUT from radio core. */
        buffer[count - 1] = RF1ADOUT0B; 
    }
}

/* Write a single byte to a radio register. */
void write_single_reg( uint8_t addr, uint8_t value )
{
    /* Wait for the Radio to be ready for next instruction. */
    while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
        ;
    }
    
    RF1AINSTRB = ( addr | RF_SNGLREGWR );   /* Send address + instruction. */
    RF1ADINB = value;                       /* Write data in. */

    _NOP( ); 
}

/* Write multiple bytes to the radio registers. */
void write_burst_reg( uint8_t addr, const uint8_t* buffer, uint8_t count )
{  
    uint8_t i;

    if( count > 0 ) {
        /* Wait for the Radio to be ready for next instruction. */
        while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
            ;
        }
        
        /* Send address + instruction. */
        RF1AINSTRW = ( ( addr | RF_REGWR ) << 8 ) + buffer[0];

        for( i = 1; i < count; i++ ) {
            RF1ADINB = buffer[i]; /* Send data. */
            
            while( !( RFDINIFG & RF1AIFCTL1 ) ) { /* Wait for TX to finish. */
                ;
            }
        }
        
        i = RF1ADOUTB; /* Reset RFDOUTIFG flag which contains status byte. */ 
    }
}

/* Send a command strobe to the radio. Includes workaround for RF1A7. */
uint8_t strobe( uint8_t cmd )
{
    uint8_t status_byte;
    uint8_t gdo_state;
    
    status_byte = 0;
  
    /* Check for valid strobe command. */
    if( ( cmd == 0xBD ) || ( ( cmd >= RF_SRES ) && ( cmd <= RF_SNOP ) ) ) {
        /* Clear the Status read flag. */
        RF1AIFCTL1 &= ~( RFSTATIFG );    

        /* Wait for radio to be ready for next instruction. */
        while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
            ;
        }

        /* Write the strobe instruction. */
        if( ( cmd > RF_SRES ) && ( cmd < RF_SNOP ) ) {
            gdo_state = read_single_reg( IOCFG2 ); /* Buffer IOCFG2 state */
            write_single_reg( IOCFG2, 0x29 );      /* Chip-ready to GDO2 */

            RF1AINSTRB = cmd;
             
            if( ( RF1AIN & 0x04 ) == 0x04 ) {      /* Chip at sleep mode */
                if( ( cmd == RF_SXOFF ) ||
                    ( cmd == RF_SPWD ) ||
                    ( cmd == RF_SWOR ) ) {
                    
                } else {
                    while( ( RF1AIN & 0x04 ) == 0x04 ) { /* Chip-ready? */
                        ;
                    }
                    
                    /* Delay for ~810usec at 1.05MHz CPU clock,
                     * see erratum RF1A7. */
                    __delay_cycles( 850 );
                }
            }
            
            write_single_reg( IOCFG2, gdo_state ); /* Restore IOCFG2 setting */

            while( !( RF1AIFCTL1 & RFSTATIFG ) ) {
                ;
            }
        } else { /* Chip active mode (SRES) */
            RF1AINSTRB = cmd; 	   
        }
        
        status_byte = RF1ASTATB;
    }
    
    return status_byte;
}

/* Reset the radio core using RF_SRES command. */
void reset_radio_core( void )
{
    strobe( RF_SRES ); /* Reset the radio core. */
    strobe( RF_SNOP ); /* Reset radio pointer. */
}

/* Write data to power table. */
void write_single_pa_table( uint8_t value )
{
    while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
        ;
    }
    
    RF1AINSTRW = 0x3E00 + value;    /* PA Table single write. */

    while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
        ;
    }
    
    RF1AINSTRB = RF_SNOP;           /* Reset PA_Table pointer. */
}

/* Write to multiple locations in power table. */
void write_burst_pa_table( const uint8_t* buffer, uint8_t count )
{
    volatile uint8_t i;

    i = 0;

    while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
        ;
    }
    
    RF1AINSTRW = 0x7E00 + buffer[i];            /* PA Table burst write. */

    for( i = 1; i < count; i++ ) {
        RF1ADINB = buffer[i];                   /* Send data. */
        
        while( !( RFDINIFG & RF1AIFCTL1 ) ) {   /* Wait for TX to finish. */
            ;
        }
    }
    
    i = RF1ADOUTB; /* Reset RFDOUTIFG flag which contains status byte. */

    while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
        ;
    }
    
    RF1AINSTRB = RF_SNOP;                       /* Reset PA Table pointer. */
}

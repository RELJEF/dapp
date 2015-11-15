
#include "rf1a.h"
#include "cc430x513x.h"


/* Read a single byte from the radio register. */
unsigned char read_single_reg( unsigned char addr )
{
    unsigned char data_out;


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
void read_burst_reg( unsigned char addr,
                     unsigned char* buffer,
                     unsigned char count )
{
    unsigned int i;
    
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
            
            /* Reads DOUT from Radio Core + clears RFDOUTIFG.
               Also initiates auo-read for next DOUT byte. */
            buffer[i] = RF1ADOUT1B;
        }
        
        /* Store the last DOUT from Radio Core. */
        buffer[count - 1] = RF1ADOUT0B; 
    }
}

/* Write a single byte to a radio register. */
void write_single_reg( unsigned char addr, unsigned char value )
{
    /* Wait for the Radio to be ready for next instruction. */
    while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
        ;
    }
    
    RF1AINSTRB = ( addr | RF_SNGLREGWR );   /* Send address + instruction. */
    RF1ADINB = value;                       /* Write data in. */

    __no_operation( ); 
}

/* Write multiple bytes to the radio registers. */
void write_burst_reg( unsigned char addr,
                      unsigned char* buffer,
                      unsigned char count )
{  
    unsigned char i;

    if( count > 0 ) {
        /* Wait for the Radio to be ready for next instruction. */
        while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
            ;
        }
        
        /* Send address + instruction. */
        RF1AINSTRW = ( (addr | RF_REGWR ) << 8 ) + buffer[0];

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
unsigned char strobe( unsigned char s )
{
    unsigned char status_byte;
    unsigned int  gdo_state;
    
    
    status_byte = 0;
  
    /* Check for valid strobe command. */
    if( ( s == 0xBD ) || ( ( s >= RF_SRES ) && ( s <= RF_SNOP ) ) ) {
        /* Clear the Status read flag. */
        RF1AIFCTL1 &= ~( RFSTATIFG );    

        /* Wait for radio to be ready for next instruction. */
        while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
            ;
        }

        /* Write the strobe instruction. */
        if( ( s > RF_SRES ) && ( s < RF_SNOP ) ) {
            gdo_state = read_single_reg( IOCFG2 ); /* buffer IOCFG2 state */
            write_single_reg( IOCFG2, 0x29 );      /* chip-ready to GDO2 */

            RF1AINSTRB = s;
             
            if( ( RF1AIN & 0x04 ) == 0x04 ) {      /* chip at sleep mod */
                if( ( s == RF_SXOFF ) ||
                    ( s == RF_SPWD ) ||
                    ( s == RF_SWOR ) ) {
                    
                } else {
                    while( ( RF1AIN & 0x04 ) == 0x04 ) { /* chip-ready? */
                        ;
                    }
                    
                    /* Delay for ~810usec at 1.05MHz CPU clock,
                       see erratum RF1A7. */
                    __delay_cycles( 850 );	            
                }
            }
            
            write_single_reg( IOCFG2, gdo_state ); /* restore IOCFG2 setting */

            while( !( RF1AIFCTL1 & RFSTATIFG ) ) {
                ;
            }
        } else { /* chip active mode (SRES)	*/
            RF1AINSTRB = s; 	   
        }
        
        status_byte = RF1ASTATB;
    }
    
    return status_byte;
}

/* Reset the radio core using RF_SRES command. */
void reset_radio_core( void )
{
    strobe( RF_SRES ); /* Reset the Radio Core. */
    strobe( RF_SNOP ); /* Reset Radio Pointer. */
}

/* Write the minimum set of RF configuration register settings. */
void write_rf_settings( rf_settings_t* rf_settings )
{
    write_single_reg( FSCTRL1,  rf_settings->fsctrl1 );
    write_single_reg( FSCTRL0,  rf_settings->fsctrl0 );
    write_single_reg( FREQ2,    rf_settings->freq2 );
    write_single_reg( FREQ1,    rf_settings->freq1 );
    write_single_reg( FREQ0,    rf_settings->freq0 );
    write_single_reg( MDMCFG4,  rf_settings->mdmcfg4 );
    write_single_reg( MDMCFG3,  rf_settings->mdmcfg3 );
    write_single_reg( MDMCFG2,  rf_settings->mdmcfg2 );
    write_single_reg( MDMCFG1,  rf_settings->mdmcfg1 );
    write_single_reg( MDMCFG0,  rf_settings->mdmcfg0 );
    write_single_reg( CHANNR,   rf_settings->channr );
    write_single_reg( DEVIATN,  rf_settings->deviatn );
    write_single_reg( FREND1,   rf_settings->frend1 );
    write_single_reg( FREND0,   rf_settings->frend0 );
    write_single_reg( MCSM0 ,   rf_settings->mcsm0 );
    write_single_reg( FOCCFG,   rf_settings->foccfg );
    write_single_reg( BSCFG,    rf_settings->bscfg );
    write_single_reg( AGCCTRL2, rf_settings->agcctrl2 );
    write_single_reg( AGCCTRL1, rf_settings->agcctrl1 );
    write_single_reg( AGCCTRL0, rf_settings->agcctrl0 );
    write_single_reg( FSCAL3,   rf_settings->fscal3 );
    write_single_reg( FSCAL2,   rf_settings->fscal2 );
    write_single_reg( FSCAL1,   rf_settings->fscal1 );
    write_single_reg( FSCAL0,   rf_settings->fscal0 );
    write_single_reg( FSTEST,   rf_settings->fstest );
    write_single_reg( TEST2,    rf_settings->test2 );
    write_single_reg( TEST1,    rf_settings->test1 );
    write_single_reg( TEST0,    rf_settings->test0 );
    write_single_reg( FIFOTHR,  rf_settings->fifothr );
    write_single_reg( IOCFG2,   rf_settings->iocfg2 );
    write_single_reg( IOCFG0,   rf_settings->iocfg0 );
    write_single_reg( PKTCTRL1, rf_settings->pktctrl1 );
    write_single_reg( PKTCTRL0, rf_settings->pktctrl0 );
    write_single_reg( ADDR,     rf_settings->addr );
    write_single_reg( PKTLEN,   rf_settings->pktlen );
}

/* Write data to power table. */
void write_single_pa_table( unsigned char value )
{
    while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
        ;
    }
    
    RF1AINSTRW = 0x3E00 + value;        /* PA Table single write. */

    while( !( RF1AIFCTL1 & RFINSTRIFG ) ) {
        ;
    }
    
    RF1AINSTRB = RF_SNOP;               /* Reset PA_Table pointer. */
}

/* Write to multiple locations in power table. */
void write_burst_pa_table( unsigned char* buffer, unsigned char count )
{
    volatile char i = 0; 

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

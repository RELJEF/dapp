
#include "main.h"

void init_rf( void )
{
#if 0
    reset_radio_core( );
    write_burst_reg( IOCFG2, rf1a_regs_config, RF1A_REGS_CONF_SIZE );
#endif
}

static inline void rf1a_error_handler( void )
{
    switch( RF1AIFERRV ) {
        case RF1AIFERRV_LVERR:      /* Low core voltage error */
            P1OUT &= ~BIT0;         /* 00 = on LED's [D2,D1] */
            P3OUT &= ~BIT6; 
            _NOP( );
            break; 
        case RF1AIFERRV_OPERR:      /* Operand error */
            P1OUT |= BIT0;          /* 01 = on LED's [D2,D1] */
            P3OUT &= ~BIT6; 
            _NOP( );
            break;  
        case RF1AIFERRV_OUTERR:     /* Output data not available error */
            P1OUT &= ~BIT0;         /* 10 = on LED's [D2,D1] */
            P3OUT |= BIT6; 
            _NOP( );
            break;
        case RF1AIFERRV_OPOVERR:    /* Operand overwrite error */
            P1OUT |= BIT0;          /* 11 = on LED's [D2,D1] */
            P3OUT |= BIT6; 
            _NOP( );
            break;
        default:
            break;
    }
}

__interrupt( CC1101_VECTOR ) void cc1101_isr( void )
{
    switch( RF1AIV ) {
        case RF1AIV_NONE:               /* RF1A interrupt */
            if( RF1AIFIV == RFERRIFG ) {
                rf1a_error_handler( );
            }
            break;
        case RF1AIV_RFIFG4:             /* RX end of packet */
            break;
        case RF1AIV_RFIFG9:             /* TX end of packet */
            break;
        default:
            break;
    }

    _BIC_SR_IRQ( LPM3_bits );
}
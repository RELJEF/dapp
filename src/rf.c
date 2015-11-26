
#include "main.h"

#define PATABLE_VALUE   0x51                /* 0 dBm output */
#define PACKET_LEN      60
#define RSSI_INDEX      PACKET_LEN          /* Index of appended RSSI */
#define CRC_LQI_INDEX   ( PACKET_LEN + 1 )  /* Index of appended CRC and LQI */


typedef enum {
    RADIO_STATE_IDLE,
    RADIO_STATE_RECEIVING,
    RADIO_STATE_TRANSMITTING
} radio_state_t;

static uint8_t rx_buffer[PACKET_LEN + 2];
static uint8_t tx_buffer[PACKET_LEN];
static uint8_t radio_state;
static uint8_t rx_buffer_len;

void init_rf( void )
{
    radio_state = RADIO_STATE_IDLE;
    rx_buffer_len = 0;

    /* Increase PMMCOREV level to 2 for proper radio operation */
    set_vcore( 2 );
    reset_radio_core( );

    /* Set the High-Power Mode Request Enable bit so LPM3 can be entered
     * with active radio enabled */
    PMMCTL0_H = 0xA5;           /* PMM password */
    PMMCTL0_L |= PMMHPMRE_L;    /* Global high power module request enable */
    PMMCTL0_H = 0x00;

    write_burst_reg( IOCFG2, rf1a_regs_config, RF1A_REGS_CONF_SIZE );
    write_single_pa_table( PATABLE_VALUE );
}

void transmit( uint8_t* buffer, uint8_t length )
{
    radio_state = RADIO_STATE_TRANSMITTING;

    RF1AIES |= BIT9;                          
    RF1AIFG &= ~BIT9;   /* Clear pending interrupts */
    RF1AIE |= BIT9;     /* Enable TX end-of-packet interrupt */

    write_burst_reg( RF_TXFIFOWR, buffer, length );     

    strobe( RF_STX );
}

void receive_on( void )
{
    radio_state = RADIO_STATE_RECEIVING;

    RF1AIES |= BIT9;    /* Falling edge of RFIFG9 */
    RF1AIFG &= ~BIT9;   /* Clear a pending interrupt */
    RF1AIE  |= BIT9;    /* Enable the interrupt */

    /* Radio is in IDLE following a TX, so strobe SRX to enter receive mode */
    strobe( RF_SRX );
}

void receive_off( void )
{
    radio_state = RADIO_STATE_IDLE;

    RF1AIE &= ~BIT9;    /* Disable RX interrupts */
    RF1AIFG &= ~BIT9;   /* Clear pending IFG */

    /* It is possible that receive_off( ) is called while radio
     * is receiving a packet. Therefore, it is necessary to flush
     * the RX FIFO after issuing IDLE strobe such that the RX FIFO
     * is empty prior to receiving a packet. */
    strobe( RF_SIDLE );
    strobe( RF_SFRX );
}

uint8_t is_radio_idle( void )
{
    return ( radio_state == RADIO_STATE_IDLE );
}

__interrupt( CC1101_VECTOR ) void cc1101_isr( void )
{
    if( RF1AIV == RF1AIV_RFIFG9 ) {
        if( radio_state == RADIO_STATE_RECEIVING ) {
            rx_buffer_len = read_single_reg( RXBYTES );
            read_burst_reg( RF_RXFIFORD, rx_buffer, rx_buffer_len );
            _NOP( );

            if( rx_buffer[CRC_LQI_INDEX] & BIT7 ) { /* Check packet CRC */
                /* Process packet... */
            }
        } else if( radio_state == RADIO_STATE_TRANSMITTING ) {
            RF1AIE &= ~BIT9; /* Disable TX end of packet interrupt */
            radio_state = RADIO_STATE_IDLE;
        }
    }

    _BIC_SR_IRQ( LPM3_bits );
}
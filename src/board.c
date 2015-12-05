
#include "main.h"


static inline void init_ports( void )
{
    P1OUT = 0x00;
    P1DIR = 0x01; /* P1.0 - LED output, P1.1 - button input */
    P2OUT = 0x00;
    P2DIR = 0x00;
    P3OUT = 0x00;
    P3DIR = 0x00;
}

static inline void init_button_led( void )
{
    /* Enable button interrupt */
    P1DIR &= ~BIT1;
    P1REN |= BIT1;
    P1IES &= BIT1;
    P1IFG = 0;
    P1OUT |= BIT1;
    P1IE  |= BIT1; 

    /* Set up LED */
    P1OUT &= ~BIT0;
    P1DIR |= BIT0;
}

void board_init( void )
{
    _BIC_SR( GIE ); /* Disable interrupts during initialization. */

    WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */

    init_ports( );
    init_button_led( );
    init_rf( );

    _BIS_SR( GIE ); /* Enable interrupts after initialization. */
}
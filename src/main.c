
#include "main.h"


#define LED_ON      P1OUT |= 0x01;      P1DIR |= 0x01;
#define LED_OFF     P1OUT &= (~0x01);   P1DIR |= 0x01;
#define LED_TOGGLE  P1OUT ^= 0x01;      P1DIR |= 0x01;
#define LED_CHECK   (P1IN & 0x01)



static inline void init_ports( void )
{
    P1OUT = 0x00;
    P1DIR = 0x01; /* P1.0 - LED out, P1.1 - button in */
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

static inline void init_chip( void )
{
    _BIC_SR( GIE ); /* Disable interrupts during initialization. */

    init_ports( );
    init_button_led( );
    init_rf( );

    _BIS_SR( GIE ); /* Enable interrupts after initialization. */
}

int main( void )
{
    WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */
    
    init_chip( );
  
    while( 1 ) {
        _NOP( );
    }

    return 0;
}
 
__interrupt( UNMI_VECTOR ) void unmi_isr( void )
{
    if( UCSCTL7 & XT2OFFG ) {    /* XT2 oscillator fault flag */
        UCSCTL7 &= (~XT2OFFG);   /* Clear flag XT2OFFG */
    }

    if ( UCSCTL7 & XT1HFOFFG ) { /* XT1 oscillator fault flag (HF mode). */
        UCSCTL7 &= (~XT1HFOFFG); /* Clear flag XT1HFOFFG */
    }

    if( UCSCTL7 & XT1LFOFFG ) {  /* XT1 oscillator fault flag (LF mode) */
        UCSCTL7 &= (~XT1LFOFFG); /* Clear flag XT1LFOFFG */
    }

    if( UCSCTL7 & DCOFFG ) {     /* DCO fault flag. */
        UCSCTL7 &= (~DCOFFG);    /* Clear flag DCOFFG */
    }

    SFRIFG1 &= (~OFIFG);         /* Oscillator fault interrupt flag */
}

__interrupt( PORT1_VECTOR ) void port1_isr( void )
{
    switch( P1IV ) {
        case P1IV_NONE:   break;
        case P1IV_P1IFG0: break;
        case P1IV_P1IFG1:
            LED_TOGGLE;
            break;
        case P1IV_P1IFG2: break;
        case P1IV_P1IFG3: break;
        case P1IV_P1IFG4: break;
        case P1IV_P1IFG5: break;
        case P1IV_P1IFG6: break;
        case P1IV_P1IFG7: break;
    }
}

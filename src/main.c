
#include "main.h"

#define LED_ON      P1OUT |= 0x01;      P1DIR |= 0x01;
#define LED_OFF     P1OUT &= (~0x01);   P1DIR |= 0x01;
#define LED_TOGGLE  P1OUT ^= 0x01;      P1DIR |= 0x01;
#define LED_CHECK   (P1IN & 0x01)


static volatile uint8_t button_pressed = 0;

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
        _BIS_SR( LPM3_bits + GIE );
        _NOP( );

        if( button_pressed ) {
            LED_TOGGLE;
            button_pressed = 0;
            P1IFG = 0;

            __delay_cycles( 100000 );

            P1IE |= BIT1;   /* Enable button press interrupt. */
        }
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
    if( P1IV == P1IV_P1IFG1 ) {     /* Change on P1.1 = button press. */
        P1IE = 0;                   /* Debounce by disabling button. */
        button_pressed = 1;
        _BIC_SR_IRQ( LPM3_bits );   /* Exit active mode. */
    }
}

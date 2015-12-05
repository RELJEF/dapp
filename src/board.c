
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

static inline void init_timer1( void )
{
    TA1CTL = TACLR;                 /* Timer1_A clear */
    TA1CCTL0 = CCIE;                /* Capture/compare 0 interrupt enable */
#if 0
    TA1CCR0 = 0x0004;               /* 32768 Hz / 4 = 8000 Hz */
#endif
    TA1CCR0 = 0x4000;               /* 2 Hz for testing */
    TA1CTL = TASSEL0 | ID_0 | MC_1; /* Clock: ACLK; divider: 0; mode: up */
}

void board_init( void )
{
    _BIC_SR( GIE ); /* Disable interrupts during initialization. */

    WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */

    init_ports( );
    init_button_led( );
    init_timer1( );
    init_rf( );

    _BIS_SR( GIE ); /* Enable interrupts after initialization. */
}

__interrupt( TIMER1_A0_VECTOR ) void timer1_a0_cc_isr( void )
{
    TA1CCTL0 &= ~CCIFG; /* Clear interrupt flag */

    /* Start ADC */

    LED_TOGGLE;
}
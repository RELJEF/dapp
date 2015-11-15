
#include <cc430x513x.h>
#include <iomacros.h>
#include "rf.h"


#define LED_ON    P1OUT |= 0x01; P1DIR |= 0x01;
#define LED_OFF   P1OUT &= (~0x01); P1DIR |= 0x01;
#define LED_TOGG  P1OUT ^= 0x01; P1DIR |= 0x01;
#define LED_CHK   (P1IN & 0x01)



void delay( volatile unsigned long p )
{
    while( p ) {
        p--;
    }
}

void ports_init( void )
{
    P1OUT = 0x00;
    P1DIR = 0x01; /* Led out */
    P2OUT = 0x00;
    P2DIR = 0x00;
    P3OUT = 0x00;
    P3DIR = 0x00;
}

void init_button_led( void )
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

void rtc_clock_init( void )
{
    P5OUT = 0x00;
    /*P5DIR = 0x02;*/   /* If this is uncommented RTC doesn't work.
                           P5DIR register must be set to 0. */
    P5SEL = 0x03;       /* Enabled alternative functions */
  
    _BIC_SR( OSCOFF );  /* Enable the LFXT1 crystal oscillator */
    UCSCTL6 &= (~XTS);  /* Select Low-frequency mode. XCAP bits define the
                           capacitance at the XIN and XOUT pins. */
    UCSCTL6 |= (XCAP0); /* Set XCAP0 */
    UCSCTL6 |= (XCAP1); /* Set XCAP1 */

    /* Then Qrystal Load Capacitance is:  
      (XCAP1=0),(XCAP0=0) -> 2pF     (XCAP1=0),(XCAP0=1) -> 5.5pF
      (XCAP1=1),(XCAP0=0) -> 8.5pF   (XCAP1=1),(XCAP0=1) -> 12pF */

    UCSCTL6 &= (~XT1OFF); /* Turns on the XT1. */
  
    /* Loop until XT1,XT2 & DCO stabilizes */
    do {
        /* Clear XT2,XT1,DCO fault flags */
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
        SFRIFG1 &= ~OFIFG;      /* Clear fault flags */
    } while( SFRIFG1 & OFIFG ); /* Test oscillator fault flag */
  
    UCSCTL4 = SELA__XT1CLK;   /* ACLK source: XT1CLK */
    SFRIFG1 &= (~OFIFG);      /* Oscillator fault interrupt flag */
    SFRIE1 |= (OFIE);         /* Oscillator fault interrupt enable */
}

void timer1_a3_init( void )
{
    TA1CTL = 0x0004;   /* Timer1_A3 clear. */
    TA1CCTL0 = 0x0010; /* Timer1_A3 Capture/compare 0 interrupt enable. */
    TA1CCR0 = 0x4000;  /* Set TACCR0 value */
    TA1CTL = 0x0110;   /* Selected: ACLK, divider:0, Up mode. */
}

void init_devices( void )
{
    _BIC_SR( GIE ); /* Disable interrupts during initialization process. */

    /* Init system clock */
    /* By defaut the FLL stabilizes MCLK and
       SMCLK to 1.048576 MHz and fDCO = 2.097152 MHz. */
 
    /* Init peripherals */
    /*rtc_clock_init( );*/
    ports_init( );
    /*timer1_a3_init( );*/
    init_button_led( );

    _BIS_SR( GIE ); /* Enable interrupts after initialization. */
}

int main( void )
{
    WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */
    
    init_devices( );
    init_rf( );
  
    while( 1 ) {
        asm( "NOP" );
    }

    return 0;
}

__interrupt( TIMER1_A0_VECTOR ) void timer1_a0_capture_compare_isr( void )
{
    /*LED_TOGG;*/
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
    switch( __even_in_range( P1IV, 4 ) ) {
        case  0: break;
        case  2: break; /* P1.0 IFG */
        case  4:
            LED_TOGG;
            break;      /* P1.1 IFG */
        case  6: break; /* P1.2 IFG */
        case  8: break; /* P1.3 IFG */
        case 10: break; /* P1.4 IFG */
        case 12: break; /* P1.5 IFG */
        case 14: break; /* P1.6 IFG */
        case 16: break; /* P1.7 IFG */
    }
}

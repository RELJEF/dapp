
#include "main.h"

#if 0
#define SMPL_PER    132     /* Audio sampling period.
                               1.048576 MHz (SMCLK) / 8 kHz = 132 */
#endif

/* For debugging purposes */
#define SMPL_PER    524     /* Sampling period counter at 2 kHz */


volatile uint8_t    adc_in[128];
volatile uint16_t   adc_count = 0;


static inline void init_ports( void )
{
    P1OUT = 0x00;
    P1DIR = 0x01; /* P1.0: LED output; P1.1: button input */
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
    TA1CTL = TASSEL_2 | ID_0 | MC_1;    /* Clock: SMCLK; divider: 0; mode: up */
    TA1R = 0;                           /* Count = 0 */
    TA1CCR0 = SMPL_PER;                 /* Counter for given sampling period */
    TA1CCTL0 = CCIE;                    /* Capture/compare 0 interrupt enable */
}

void test_adc12( void )
{
    uint16_t i;

    adc_count = 0;

    for( i = 0; i < 128; i++ ) {
        adc_in[i] = 0x33;   /* For debugging purposes */
    }

    P2DIR = 0x00;           /* Input */
    P2REN = 0xFF;           /* Enable pullups/pulldowns */
    P2OUT = 0xFF;           /* Pullups */
    P2SEL |= BIT5;          /* Select ADC12 A5 */

    PMAPPWD = PMAPPW;       /* Get write access to port mapping registers */
    PMAPCTL |= PMAPRECFG;   /* Unlock port settings */
    P2MAP5 = PM_ANALOG;     /* P2.5 is analog pin */
    PMAPPWD = 0;            /* Disable write access to port mapping registers */

#if 0
    /* DEBUG */
    if( ( P2IN >> 5 ) & 0x01 ) {
        LED_ON;
    }
#endif

    /* S&H time: 192 ADC12CLK cycles; ADC12: on */
    ADC12CTL0 = ADC12SHT0_7 | ADC12SHT1_7 | ADC12ON;

    /* S&H source: ADC12SC bit; Single channel, single conversion;
       Clock divider: 0; Clock: ADCOSC */
    ADC12CTL1 = ADC12SHS_0 | ADC12CONSEQ_0 | ADC12DIV_0 | ADC12SSEL_0;

    /* Resolution: 8 bits; Data format: binary unsigned */
    ADC12CTL2 = ADC12RES_0;

    /* Referent voltages: AVcc (3.3V) and AVss (0V); Input channel: A5 (P2.5) */
    ADC12MCTL0 = ADC12SREF_0 | ADC12INCH_5;

    ADC12IFG = 0;           /* Clear ADC12 interrupt flag register */
    ADC12IE |= ADC12IE0;    /* Enable interrupt for memory location 0 */
    ADC12CTL0 |= ADC12ENC;  /* Enable conversion */

    /* When using internal reference, wait for it to stabilize here */

    init_timer1( );

    /* Enable global interrupts and enter LPM0 mode */
    _BIS_SR( LPM0_bits | GIE );

    /* When ADC12 is finished with sampling, CPU wakes up and execution
       flow lands here. */
    _BIC_SR( GIE );

    /* Disable ADC12 conversion */
    ADC12CTL0 &= ~( ADC12ENC | ADC12SC | ADC12SHT0_7 | ADC12SHT1_7 );
    ADC12CTL0 &= ~ADC12ON;  /* Switch off ADC12 */
    TA1CTL = 0;             /* Disable timer */

    LED_ON;
}

void board_init( void )
{
    _BIC_SR( GIE ); /* Disable interrupts during initialization. */

    WDTCTL = WDTPW | WDTHOLD; /* Stop WDT */

    init_ports( );
    init_button_led( );
    init_rf( );

    _BIS_SR( GIE ); /* Enable interrupts after initialization. */
}

__interrupt( TIMER1_A0_VECTOR ) void timer1_a0_cc_isr( void )
{
    TA1CCTL0 &= ~CCIFG; /* Clear interrupt flag */

    /* Start conversion */
    ADC12CTL0 |= ADC12SC;
    ADC12CTL0 &= ~ADC12SC;
}

__interrupt( ADC12_VECTOR ) void adc12_isr( void )
{
    adc_in[adc_count] = ADC12MEM0;

    adc_count++;

    if( adc_count >= 128 ) {
        ADC12CTL0 = 0;
        _BIC_SR_IRQ( LPM0_bits ); /* Enter active mode */
    }
}
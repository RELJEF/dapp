
#include "main.h"

static volatile bool_t button_pressed = FALSE;


int main( void )
{
    board_init( );

    //receive_on( );
  
    while( 1 ) {
        _BIS_SR( LPM3_bits + GIE ); /* Enter LPM3 mode, interrupts enabled */
        _NOP( );

        if( button_pressed ) {
            LED_TOGGLE;

            button_pressed = FALSE;
            P1IFG = 0;

            P1IE |= BIT1;   /* Enable button press interrupt. */
        }
    }

    return 0;
}

__interrupt( PORT1_VECTOR ) void port1_isr( void )
{
    if( P1IV == P1IV_P1IFG1 ) {     /* Change on P1.1 = button press. */
        P1IE = 0;                   /* Debounce by disabling button. */
        button_pressed = TRUE;
        _BIC_SR_IRQ( LPM3_bits );   /* Exit active mode. */
    }
}

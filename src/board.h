
#ifndef _BOARD_H_
#define _BOARD_H_

#define LED_ON      P1OUT |= 0x01;      P1DIR |= 0x01;
#define LED_OFF     P1OUT &= (~0x01);   P1DIR |= 0x01;
#define LED_TOGGLE  P1OUT ^= 0x01;      P1DIR |= 0x01;
#define LED_CHECK   (P1IN & 0x01)


void board_init( void );
void test_adc12( void );

#endif /* _BOARD_H_ */
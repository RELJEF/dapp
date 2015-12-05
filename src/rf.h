
#ifndef _RF_H_
#define _RF_H_

void    init_rf( void );
void    transmit( uint8_t* buffer, uint8_t length );
void    receive_on( void );
void    receive_off( void );
bool_t  is_radio_idle( void );

#endif /* _RF_H_ */

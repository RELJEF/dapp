
#ifndef _RF1A_H_
#define _RF1A_H_

#define RF1A_REGS_CONF_SIZE  47

extern const uint8_t rf1a_regs_config[ RF1A_REGS_CONF_SIZE ];


uint8_t read_single_reg( uint8_t addr );
void    read_burst_reg( uint8_t addr, uint8_t* buffer, uint8_t count );
void    write_single_reg( uint8_t addr, uint8_t value );
void    write_burst_reg( uint8_t addr, const uint8_t* buffer, uint8_t count );
uint8_t strobe( uint8_t cmd );
void    reset_radio_core( void );
void    write_single_pa_table( uint8_t value );
void    write_burst_pa_table( const uint8_t* buffer, uint8_t count );

#endif /* _RF1A_H_ */

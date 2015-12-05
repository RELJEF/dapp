
#ifndef _ADPCM_H_
#define _ADPCM_H_

uint16_t adpcm_encode( uint8_t* dst, const uint8_t* src, uint16_t src_size );
uint16_t adpcm_decode( uint8_t* dst, const uint8_t* src, uint16_t src_size );

#endif /* _ADPCM_H_ */
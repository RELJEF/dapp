
#include "main.h"

/*
================================================================================

IMA Adaptive Differential Pulse Code Modulation algorithm implementation.

================================================================================
*/

static const uint16_t step_table[89] = {
        7,     8,     9,    10,    11,    12,    13,    14,
       16,    17,    19,    21,    23,    25,    28,    31,
       34,    37,    41,    45,    50,    55,    60,    66,
       73,    80,    88,    97,   107,   118,   130,   143,
      157,   173,   190,   209,   230,   253,   279,   307,
      337,   371,   408,   449,   494,   544,   598,   658,
      724,   796,   876,   963,  1060,  1166,  1282,  1411,
     1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,
     3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,
     7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767
};

static const int8_t index_table[8] = {
    -1, -1, -1, -1, 2, 4, 6, 8
};

typedef struct {
    int16_t predicted_value;
    uint8_t step_index;
} ima_adpcm_state_t;



/*
================================================================================
adpcm_encode_sample
 
Encodes one 16 bit PCM sample into lower 4 bits of the return value.
================================================================================
*/
static uint8_t adpcm_encode_sample( ima_adpcm_state_t* s, int16_t pcm16 )
{
    int16_t     predicted;
    int8_t      step_idx;
    uint8_t     value;
    int16_t     delta;
    uint16_t    step;
    uint16_t    diff;

    predicted = s->predicted_value;
    step_idx = s->step_index;
    delta = pcm16 - predicted;

    if( delta >= 0 ) {
        value = 0;
    } else {
        value = 8;
        delta = -delta;
    }

    step = step_table[ step_idx ];
    diff = step >> 3;

    if( delta > step ) {
        value |= 4;
        delta -= step;
        diff += step;
    }

    step >>= 1;

    if( delta > step ) {
        value |= 2;
        delta -= step;
        diff += step;
    }

    step >>= 1;

    if( delta > step ) {
        value |= 1;
        diff += step;
    }

    if( value & 8 ) {
        predicted -= diff;
    } else {
        predicted += diff;
    }

    s->predicted_value = predicted;

    step_idx += index_table[ value & 7 ];

    if( step_idx < 0 ) {
        step_idx = 0;
    } else if( step_idx > 88 ) {
        step_idx = 88;
    }

    s->step_index = step_idx;

    return value;
}

/*
================================================================================
adpcm_dencode_sample
 
Decodes lower 4 bits of input byte 'adpcm' into one 16 bit PCM sample.
Upper 4 bits of the input byte are ignored.
================================================================================
*/
static int16_t adpcm_decode_sample( ima_adpcm_state_t* s, uint8_t adpcm )
{
    int16_t     predicted;
    int8_t      step_idx;
    uint16_t    step;
    uint16_t    diff;

    predicted = s->predicted_value;
    step_idx = s->step_index;
    step = step_table[ step_idx ];
    step_idx += index_table[ adpcm & 7 ];

    if( step_idx < 0 ) {
        step_idx = 0;
    } else if( step_idx > 88 ) {
        step_idx = 88;
    }

    s->step_index = step_idx;
    diff = step >> 3;

    if( adpcm & 4 ) {
        diff += step;
    }

    if( adpcm & 2 ) {
        diff += step >> 1;
    }

    if( adpcm & 1 ) {
        diff += step >> 2;
    }

    if( adpcm & 8 ) {
        predicted -= diff;
    } else {
        predicted += diff;
    }

    s->predicted_value = predicted;

    return predicted;
}

uint16_t adpcm_encode( uint8_t* dst, const uint8_t* src, uint16_t src_size )
{
    ima_adpcm_state_t   s;
    uint8_t             adpcm;
    int16_t             i;
    uint8_t             tmp;

    src_size &= ~1;

    s.predicted_value = 0;
    s.step_index = 0;

    for( i = 0; i < src_size; i++ ) {
        adpcm = adpcm_encode_sample( &s, (int16_t)*src++ );

        if( !( i & 1 ) ) {
            tmp = adpcm;
        } else {
            *dst = ( adpcm << 4 ) | tmp;
            dst++;
        }
    }

    return src_size >> 1;
}

uint16_t adpcm_decode( uint8_t* dst, const uint8_t* src, uint16_t src_size )
{
    ima_adpcm_state_t   s;
    uint8_t             adpcm;
    uint16_t            i;

    s.predicted_value = 0;
    s.step_index = 0;

    src_size <<= 1;

    for( i = 0; i < src_size; i++ ) {
        adpcm = *src;

        if( i & 1 ) {
            adpcm >>= 4;
            src++;
        }

        *dst = (uint8_t)adpcm_decode_sample( &s, adpcm );
        dst++;
    }

    return src_size;
}
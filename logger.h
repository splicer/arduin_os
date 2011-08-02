#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

#define FILE_KERNEL     0x00
#define FILE_SERIAL     0x01
// 0x1f is the largest possible file ID

// 15:11 - 5-bit file ID (limit of 32 file IDs)
// 10:0  - 11-bit line number (limit of 2048 lines per file)
#define PRINT_ID (((uint16_t)(FILE_ID) << 11) | \
                  (uint16_t)(0x7ff & (__LINE__ - 1)))

#define PRINT0( str ) \
    print0( PRINT_ID )
#define PRINT1( str, a ) \
    print1( PRINT_ID, (uint16_t)(a) )
#define PRINT2( str, a, b ) \
    print2( PRINT_ID, (uint16_t)(a), (uint16_t)(b) )
#define PRINT3( str, a, b, c ) \
    print3( PRINT_ID, (uint16_t)(a), (uint16_t)(b), (uint16_t)(c) )
#define PRINT4( str, a, b, c, d ) \
    print4( PRINT_ID, (uint16_t)(a), (uint16_t)(b), (uint16_t)(c), \
            (uint16_t)(d) )
#define PRINT5( str, a, b, c, d, e ) \
    print5( PRINT_ID, (uint16_t)(a), (uint16_t)(b), (uint16_t)(c), \
            (uint16_t)(d), (uint16_t)(e) )
#define PRINT6( str, a, b, c, d, e, f ) \
    print6( PRINT_ID, (uint16_t)(a), (uint16_t)(b), (uint16_t)(c), \
            (uint16_t)(d), (uint16_t)(e), (uint16_t)(f) )

void logger_init( volatile uint32_t *ticks_since_boot );
void print_timestamp();
void print0( uint16_t id );
void print1( uint16_t id, uint16_t a );
void print2( uint16_t id, uint16_t a, uint16_t b );
void print3( uint16_t id, uint16_t a, uint16_t b, uint16_t c );
void print4( uint16_t id, uint16_t a, uint16_t b, uint16_t c, uint16_t d );
void print5( uint16_t id,
             uint16_t a,
             uint16_t b,
             uint16_t c,
             uint16_t d,
             uint16_t e );
void print6( uint16_t id,
             uint16_t a,
             uint16_t b,
             uint16_t c,
             uint16_t d,
             uint16_t e,
             uint16_t f );

#endif

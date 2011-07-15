#include "logger.h"
#include "serial.h"
#include <avr/io.h>

#define CYCLES_PER_TICK (125)

static volatile uint32_t *ticks_since_boot;


static void enc_serial_send( uint8_t b )
{
    switch( b ) {
        case 0xfc:
            serial_send( 0xfc );
            serial_send( 0xfc );
            break;
        case 0xfd:
            serial_send( 0xfc );
            serial_send( 0xfd );
            break;
        case 0xfe:
            serial_send( 0xfd );
            serial_send( 0xfc );
            break;
        default:
            serial_send( b );
    }
}


void logger_init( volatile uint32_t * _ticks_since_boot )
{
    ticks_since_boot = _ticks_since_boot;
    serial_init();
}


void print_timestamp()
{
    uint32_t abs_time = *ticks_since_boot;

    serial_send( 0xfe ); // start packet

    serial_send( 7 ); // timestamp symbol (not num args)

    serial_send( (uint8_t)(abs_time >> 24) );
    serial_send( (uint8_t)(abs_time >> 16) );
    serial_send( (uint8_t)(abs_time >> 8) );
    serial_send( (uint8_t)abs_time );
}


void print0( uint16_t id )
{
    uint8_t microtick = TCNT0 + CYCLES_PER_TICK;
    uint8_t tick = (uint8_t)*ticks_since_boot;

    serial_send( 0xfe ); // start packet

    serial_send( 0 ); // num args
    enc_serial_send( (uint8_t)(id >> 8) );
    enc_serial_send( (uint8_t)id );
    enc_serial_send( tick );
    enc_serial_send( microtick );
}


void print1( uint16_t id, uint16_t a )
{
    uint8_t microtick = TCNT0 + CYCLES_PER_TICK;
    uint8_t tick = (uint8_t)*ticks_since_boot;

    serial_send( 0xfe ); // start packet

    serial_send( 1 ); // num args
    enc_serial_send( (uint8_t)(id >> 8) );
    enc_serial_send( (uint8_t)id );
    enc_serial_send( tick );
    enc_serial_send( microtick );

    enc_serial_send( (uint8_t)(a >> 8) );
    enc_serial_send( (uint8_t)a );
}


void print2( uint16_t id, uint16_t a, uint16_t b )
{
    uint8_t microtick = TCNT0 + CYCLES_PER_TICK;
    uint8_t tick = (uint8_t)*ticks_since_boot;

    serial_send( 0xfe ); // start packet

    serial_send( 2 ); // num args
    enc_serial_send( (uint8_t)(id >> 8) );
    enc_serial_send( (uint8_t)id );
    enc_serial_send( tick );
    enc_serial_send( microtick );

    enc_serial_send( (uint8_t)(a >> 8) );
    enc_serial_send( (uint8_t)a );

    enc_serial_send( (uint8_t)(b >> 8) );
    enc_serial_send( (uint8_t)b );
}


void print3( uint16_t id, uint16_t a, uint16_t b, uint16_t c )
{
    uint8_t microtick = TCNT0 + CYCLES_PER_TICK;
    uint8_t tick = (uint8_t)*ticks_since_boot;

    serial_send( 0xfe ); // start packet

    serial_send( 3 ); // num args
    enc_serial_send( (uint8_t)(id >> 8) );
    enc_serial_send( (uint8_t)id );
    enc_serial_send( tick );
    enc_serial_send( microtick );

    enc_serial_send( (uint8_t)(a >> 8) );
    enc_serial_send( (uint8_t)a );

    enc_serial_send( (uint8_t)(b >> 8) );
    enc_serial_send( (uint8_t)b );

    enc_serial_send( (uint8_t)(c >> 8) );
    enc_serial_send( (uint8_t)c );
}


void print4( uint16_t id, uint16_t a, uint16_t b, uint16_t c, uint16_t d )
{
    uint8_t microtick = TCNT0 + CYCLES_PER_TICK;
    uint8_t tick = (uint8_t)*ticks_since_boot;

    serial_send( 0xfe ); // start packet

    serial_send( 4 ); // num args
    enc_serial_send( (uint8_t)(id >> 8) );
    enc_serial_send( (uint8_t)id );
    enc_serial_send( tick );
    enc_serial_send( microtick );

    enc_serial_send( (uint8_t)(a >> 8) );
    enc_serial_send( (uint8_t)a );

    enc_serial_send( (uint8_t)(b >> 8) );
    enc_serial_send( (uint8_t)b );

    enc_serial_send( (uint8_t)(c >> 8) );
    enc_serial_send( (uint8_t)c );

    enc_serial_send( (uint8_t)(d >> 8) );
    enc_serial_send( (uint8_t)d );
}


void print5( uint16_t id,
             uint16_t a,
             uint16_t b,
             uint16_t c,
             uint16_t d,
             uint16_t e )
{
    uint8_t microtick = TCNT0 + CYCLES_PER_TICK;
    uint8_t tick = (uint8_t)*ticks_since_boot;

    serial_send( 0xfe ); // start packet

    serial_send( 5 ); // num args
    enc_serial_send( (uint8_t)(id >> 8) );
    enc_serial_send( (uint8_t)id );
    enc_serial_send( tick );
    enc_serial_send( microtick );

    enc_serial_send( (uint8_t)(a >> 8) );
    enc_serial_send( (uint8_t)a );

    enc_serial_send( (uint8_t)(b >> 8) );
    enc_serial_send( (uint8_t)b );

    enc_serial_send( (uint8_t)(c >> 8) );
    enc_serial_send( (uint8_t)c );

    enc_serial_send( (uint8_t)(d >> 8) );
    enc_serial_send( (uint8_t)d );

    enc_serial_send( (uint8_t)(e >> 8) );
    enc_serial_send( (uint8_t)e );
}


void print6( uint16_t id,
             uint16_t a,
             uint16_t b,
             uint16_t c,
             uint16_t d,
             uint16_t e,
             uint16_t f )
{
    uint8_t microtick = TCNT0 + CYCLES_PER_TICK;
    uint8_t tick = (uint8_t)*ticks_since_boot;

    serial_send( 0xfe ); // start packet

    serial_send( 6 ); // num args
    enc_serial_send( (uint8_t)(id >> 8) );
    enc_serial_send( (uint8_t)id );
    enc_serial_send( tick );
    enc_serial_send( microtick );

    enc_serial_send( (uint8_t)(a >> 8) );
    enc_serial_send( (uint8_t)a );

    enc_serial_send( (uint8_t)(b >> 8) );
    enc_serial_send( (uint8_t)b );

    enc_serial_send( (uint8_t)(c >> 8) );
    enc_serial_send( (uint8_t)c );

    enc_serial_send( (uint8_t)(d >> 8) );
    enc_serial_send( (uint8_t)d );

    enc_serial_send( (uint8_t)(e >> 8) );
    enc_serial_send( (uint8_t)e );

    enc_serial_send( (uint8_t)(f >> 8) );
    enc_serial_send( (uint8_t)f );
}

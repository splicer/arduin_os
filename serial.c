#include "serial.h"
#include <avr/io.h>
#include <avr/interrupt.h>


void serial_init()
{
    // Set up the UART using U2X
#define UBRR_SETTING (F_CPU / 4 / (BAUD - 1) / 2)
#if defined( UBRRH )
    UBRRH = (uint8_t)(UBRR_SETTING >> 8);
    UBRRL = (uint8_t)UBRR_SETTING;

    UCSRA = _BV(U2X);

    // Enable the receiver and transmitter
    UCSRB = _BV(RXEN) | _BV(TXEN);
#else
    UBRR0H = (uint8_t)(UBRR_SETTING >> 8);
    UBRR0L = (uint8_t)UBRR_SETTING;

    UCSR0A = _BV(U2X0);

    // Enable the receiver and transmitter
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
#endif
}


void serial_send( uint8_t c )
{
    uint8_t old_sreg;

    old_sreg = SREG;
    cli();
#if defined( UBRRH )
    // Wait for empty transmit buffer
    while( !( UCSRA & (1<<UDRE) ) )
        ;
    // Put data into buffer, sends the data
    UDR = c;
#else
    // Wait for empty transmit buffer
    while( !( UCSR0A & (1<<UDRE0) ) )
        ;
    // Put data into buffer, sends the data
    UDR0 = c;
#endif
    SREG = old_sreg;
}

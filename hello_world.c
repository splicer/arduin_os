#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>


int main()
{
    bool state = true;

    // set the mode of the LED pins to output
    DDRB |= _BV( PIN5 );
    for( ;; state = !state ) {
        if( state ) {
            PORTB |= _BV( PIN5 );
        } else {
            PORTB &= ~_BV( PIN5 );
        }
        _delay_ms( 500 );
    }

    return 0;
}

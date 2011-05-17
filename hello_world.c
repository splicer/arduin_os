#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>


// given that timer0 will be prescaled by 1024,
// let 1 tick equal exactly 8 msec
#define CYCLES_PER_TICK (125)
#define MS_TO_TICKS( ms ) ( (ms) >> 3 )
#define TICKS_TO_MS( ticks ) ( (ticks) << 3 )

static volatile uint32_t ticks_since_boot = 0;


int main()
{
    DDRB |= _BV( PIN5 ); // set digital pin 13 to output mode

#if defined( CLKPR )
    // disable system clock prescaling
    CLKPR = _BV( CLKPCE );
    CLKPR = 0;
#endif

#if defined( TIMSK0 )
    // enable TIMER0_OVF_vect
    TIMSK0 = _BV( TOIE0 );

    // normal (counter) mode, prescaled by 1024
    TCCR0A = 0;
    TCCR0B = _BV( CS02 ) | _BV( CS00 );
#else
    // enable TIMER0_OVF_vect
    TIMSK = _BV( TOIE0 );

    // prescale by 1024
    TCCR0 = _BV( CS02 ) | _BV( CS00 );
#endif

    // set timer to fire after 1 tick
    TCNT0 = 0xFF - CYCLES_PER_TICK + 1;

    // enable interrupts
    sei();

    for( ;; ); // loop forever
    return 0;
}


ISR( TIMER0_OVF_vect )
{
    // set timer to fire again after 1 tick
    TCNT0 = 0xFF - CYCLES_PER_TICK + 1;

    ticks_since_boot++;

    // toggle LED every second
    if( !(ticks_since_boot % MS_TO_TICKS( 1000 )) ) {
        PORTB ^= _BV( PIN5 ); // flip LED (Arduino's PIN9) state
    }
}

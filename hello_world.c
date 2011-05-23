// arduino pin 0  -> port D, pin 0 ?
// arduino pin 1  -> port D, pin 1 ?
// arduino pin 2  -> port D, pin 2 ?
// arduino pin 3  -> port D, pin 3 ?
// arduino pin 4  -> port D, pin 4 ?
// arduino pin 5  -> port D, pin 5 ?
// arduino pin 6  -> port D, pin 6 ?
// arduino pin 7  -> port D, pin 7 ?
// arduino pin 8  -> port B, pin 0
// arduino pin 9  -> port B, pin 1
// arduino pin 10 -> port B, pin 2
// arduino pin 11 -> port B, pin 3
// arduino pin 12 -> port B, pin 4
// arduino pin 13 -> port B, pin 5

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

// given that timer0 will be prescaled by 1024,
// let 1 tick equal exactly 8 msec
#define CYCLES_PER_TICK (125)
#define MS_TO_TICKS( ms ) ( (ms) >> 3 )
#define TICKS_TO_MS( ticks ) ( (ticks) << 3 )

#define LEAVE_CRUMB( id ) PORTB = ( (id) & 0x0F ) | ( PORTB & 0xF0 )

#define PUSH_CONTEXT() asm volatile(\
    "push   r31\n\t"\
    "in     r31, __SREG__\n\t"\
    "push   r31\n\t"\
    "push   r30\n\t"\
    "push   r29\n\t"\
    "push   r28\n\t"\
    "push   r27\n\t"\
    "push   r26\n\t"\
    "push   r25\n\t"\
    "push   r24\n\t"\
    "push   r23\n\t"\
    "push   r22\n\t"\
    "push   r21\n\t"\
    "push   r20\n\t"\
    "push   r19\n\t"\
    "push   r18\n\t"\
    "push   r17\n\t"\
    "push   r16\n\t"\
    "push   r15\n\t"\
    "push   r14\n\t"\
    "push   r13\n\t"\
    "push   r12\n\t"\
    "push   r11\n\t"\
    "push   r10\n\t"\
    "push   r9\n\t"\
    "push   r8\n\t"\
    "push   r7\n\t"\
    "push   r6\n\t"\
    "push   r5\n\t"\
    "push   r4\n\t"\
    "push   r3\n\t"\
    "push   r2\n\t"\
    "push   r1\n\t"\
    "push   r0\n\t"\
    "clr    __zero_reg__" ) // gcc assumes __zero_reg__ (r1) is zero after a context switch

#define POP_CONTEXT() asm volatile(\
    "pop    r0\n\t"\
    "pop    r1\n\t"\
    "pop    r2\n\t"\
    "pop    r3\n\t"\
    "pop    r4\n\t"\
    "pop    r5\n\t"\
    "pop    r6\n\t"\
    "pop    r7\n\t"\
    "pop    r8\n\t"\
    "pop    r9\n\t"\
    "pop    r10\n\t"\
    "pop    r11\n\t"\
    "pop    r12\n\t"\
    "pop    r13\n\t"\
    "pop    r14\n\t"\
    "pop    r15\n\t"\
    "pop    r16\n\t"\
    "pop    r17\n\t"\
    "pop    r18\n\t"\
    "pop    r19\n\t"\
    "pop    r20\n\t"\
    "pop    r21\n\t"\
    "pop    r22\n\t"\
    "pop    r23\n\t"\
    "pop    r24\n\t"\
    "pop    r25\n\t"\
    "pop    r26\n\t"\
    "pop    r27\n\t"\
    "pop    r28\n\t"\
    "pop    r29\n\t"\
    "pop    r30\n\t"\
    "pop    r31\n\t"\
    "out    __SREG__, r31\n\t"\
    "pop    r31\n\t"\
    "ret" )

static volatile uint32_t ticks_since_boot = 0;
static uint8_t task1_stack[256];
static volatile uint8_t *task1_sp;
static volatile uint8_t *cur_task_sp;
static volatile uint8_t *kernel_sp;

static void init_task1();
static void task1() __attribute__((noinline));
static void run_scheduler();
static void exit_kernel() __attribute__((naked, noinline));





void serial_init(void)
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
    // Wait for empty transmit buffer
    while( !( UCSR0A & (1<<UDRE0) ) )
        ;
    // Put data into buffer, sends the data
    UDR0 = c;
}


// this will eventually become a generic task creation function
static void init_task1()
{
    task1_sp = &task1_stack[sizeof( task1_stack ) - 35];
    task1_sp[2] = 0; // __zero_reg__ (r1)
    task1_sp[32] = _BV( SREG_I ); // SREG
    task1_sp[34] = (uint8_t)((uint16_t)&task1 >> 8);
    task1_sp[35] = (uint8_t)(uint16_t)&task1;
}


static void task1()
{
    for( ;; ) {
        if( (ticks_since_boot % 0x100) <= 0x7f) {
            PORTB |= _BV( PIN5 );
        } else {
            PORTB &= ~_BV( PIN5 );
        }
        if( (ticks_since_boot % 0x100) == 0) {
            serial_send('a');
        }
    }
}


static void run_scheduler()
{
    cur_task_sp = task1_sp;
}


static void exit_kernel()
{
    PUSH_CONTEXT();
    kernel_sp = (uint8_t *)SP;
    SP = (uint16_t)cur_task_sp;
    POP_CONTEXT();
}


int main()
{
    // set Arduino's pin 13 to output mode
    DDRB |= _BV( PIN5 );

#if defined( CLKPR )
    // disable system clock prescaling
    CLKPR = _BV( CLKPCE );
    CLKPR = 0;
#endif

    // Setup serial driver
    serial_init();

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

    // TODO: use a generic task creation function instead
    init_task1();

    // set timer to fire after 1 tick
    TCNT0 = 0xFF - CYCLES_PER_TICK + 1;

    // main kernel loop
    for( ;; ) {
        run_scheduler();
        exit_kernel();
        // TODO handle kernel request
    }

    return 0;
}


ISR( TIMER0_OVF_vect, ISR_NAKED )
{
    // push current context to stack
    asm volatile(
        "push   r31\n\t"

        // set timer to fire again after 1 tick
        "ldi    r31, %[cycles]\n\t"
        "out    %[counter], r31\n\t"

        // save a copy of SREG to the stack, with its I-bit enabled because we
        // know interrupts were enabled before this ISR fired
        "in     r31, __SREG__\n\t"
        "ori    r31, %[sreg_i_mask]\n\t"
        "push   r31\n\t"

        "push   r30\n\t"
        "push   r29\n\t"
        "push   r28\n\t"
        "push   r27\n\t"
        "push   r26\n\t"
        "push   r25\n\t"
        "push   r24\n\t"
        "push   r23\n\t"
        "push   r22\n\t"
        "push   r21\n\t"
        "push   r20\n\t"
        "push   r19\n\t"
        "push   r18\n\t"
        "push   r17\n\t"
        "push   r16\n\t"
        "push   r15\n\t"
        "push   r14\n\t"
        "push   r13\n\t"
        "push   r12\n\t"
        "push   r11\n\t"
        "push   r10\n\t"
        "push   r9\n\t"
        "push   r8\n\t"
        "push   r7\n\t"
        "push   r6\n\t"
        "push   r5\n\t"
        "push   r4\n\t"
        "push   r3\n\t"
        "push   r2\n\t"
        "push   r1\n\t"
        "push   r0\n\t"
        "clr    __zero_reg__" // gcc assumes __zero_reg__ (r1) is zero after a context switch
        :
        : [cycles]      "M" (0xFF - CYCLES_PER_TICK + 1)
        , [counter]     "I" (_SFR_IO_ADDR( TCNT0 ))
        , [sreg_i_mask] "M" (_BV( SREG_I )) );

    ticks_since_boot++;

    SP = (uint16_t)kernel_sp;

    // pop kernel's context from stack
    POP_CONTEXT();
}

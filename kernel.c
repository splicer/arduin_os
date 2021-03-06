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

#include "logger.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#define FILE_ID FILE_KERNEL

// given that timer0 will be prescaled by 1024,
// let 1 tick equal exactly 8 msec
#define CYCLES_PER_TICK (125)
#define MS_TO_TICKS( ms ) ( (ms) >> 3 )
#define TICKS_TO_MS( ticks ) ( (ticks) << 3 )

#define MAX_THREADS (32)

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

struct Thread {
    volatile uint16_t sp;
};

static volatile uint32_t ticks_since_boot = 0;
static struct Thread threads[MAX_THREADS];
static volatile uint8_t free_thread_ids[MAX_THREADS];
static volatile uint8_t num_free_threads;
static volatile uint8_t cur_thread_id;
static volatile uint16_t kernel_sp;

static uint8_t thread1_stack[256];
static uint8_t thread2_stack[256];

static void thread1() __attribute__((noinline));
static void thread2() __attribute__((noinline));

static void run_scheduler();
static void exit_kernel() __attribute__((naked, noinline));
static void create_thread( void (* func)(), uint8_t *stack, uint16_t size );
static void thread_self_destruct() __attribute__((noinline));


static void create_thread( void (* func)(), uint8_t *stack, uint16_t size )
{
    struct Thread *t;
    uint8_t *stack_bottom;

    if( size < 38 ) {
        // don't create the thread
        // TODO return an error
        return;
    }

    if( num_free_threads == 0 ) {
        // no more free threads
        // TODO return an error
        return;
    }

    t = &threads[free_thread_ids[--num_free_threads]];

    stack_bottom = &stack[size - 1];
    stack_bottom[0] = (uint8_t)(uint16_t)&thread_self_destruct;
    stack_bottom[-1] = (uint8_t)(((uint16_t)&thread_self_destruct) >> 8);
    stack_bottom[-2] = (uint8_t)(uint16_t)func;
    stack_bottom[-3] = (uint8_t)(((uint16_t)func) >> 8);
    // stack_bottom[-4] is r31
    stack_bottom[-5] = _BV( SREG_I ); // SREG
    // stack_bottom[-6] is r30
    // ...
    // stack_bottom[-34] is r2
    stack_bottom[-35] = 0; // __zero_reg__ (r1)
    // stack_bottom[-36] is r0
    t->sp = (uint16_t)&stack_bottom[-37];
}


static void thread_self_destruct()
{
    cli();
    free_thread_ids[num_free_threads++] = cur_thread_id;
    SP = kernel_sp;
    POP_CONTEXT();
}


static void thread1()
{
    for( ;; ) {
        if( (ticks_since_boot % MS_TO_TICKS( 400 )) <= MS_TO_TICKS( 300 ) ) {
            PORTB |= _BV( PIN4 );
        } else {
            PORTB &= ~_BV( PIN4 );
        }
        PRINT0( "thread1" );

        if( ticks_since_boot >= MS_TO_TICKS( 10000 ) ) break;
    }
}


static void thread2()
{
    for( ;; ) {
        if( (ticks_since_boot % MS_TO_TICKS( 300 )) <= MS_TO_TICKS( 10 ) ) {
            PORTB |= _BV( PIN5 );
        } else {
            PORTB &= ~_BV( PIN5 );
        }
        PRINT1( "thread %u", 2 );
    }
}


static void run_scheduler()
{
    // a horribly inefficient implementation of round-robin
    for( ;; ) {
        cur_thread_id = (cur_thread_id + 1) % MAX_THREADS;
        for( uint8_t i = 0; i < num_free_threads; i++ ) {
            if( cur_thread_id == free_thread_ids[i] ) {
                break;
            } else if( i == num_free_threads - 1 ) {
                return;
            }
        }
    }
}


static void exit_kernel()
{
    PUSH_CONTEXT();
    kernel_sp = SP;
    SP = threads[cur_thread_id].sp;
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

    for( uint8_t i = 0; i < MAX_THREADS; i++ ) {
        free_thread_ids[i] = i;
    }
    num_free_threads = MAX_THREADS;

    create_thread( thread1, thread1_stack, sizeof( thread1_stack ) );
    create_thread( thread2, thread2_stack, sizeof( thread2_stack ) );

    // set timer to fire after 1 tick
    TCNT0 = 0xFF - CYCLES_PER_TICK + 1;

    logger_init( &ticks_since_boot );

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

    threads[cur_thread_id].sp = SP;
    SP = kernel_sp;

    if( !(ticks_since_boot++ % 0x100) ) {
        print_timestamp();
    }

    // pop kernel's context from stack
    POP_CONTEXT();
}

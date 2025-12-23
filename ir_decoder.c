/**
 * @file ir_decoder.c
 * @brief Implementation of NEC IR decoding logic using State Machine.
 */

#include "config.h"
#include "ir_decoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

/* Hardware Configuration */
#define DATA_RECEIVER   PD2
#define TIMER_PRESCALER 1024UL

/* Macro to convert microseconds to Timer Ticks */
#define TIME_TICS(us)   ((unsigned long)((float)(us) * (F_CPU / 1000000.0) / (float)TIMER_PRESCALER))

/* * NEC Protocol Timing Definitions (with +/- 33% Tolerance) 
 * Format: [Base Time] -> [Min Time] ... [Max Time]
 */

// Header Mark (9ms)
#define TIME_9000US     TIME_TICS(9000)
#define TOL_9000US      ((TIME_9000US)/3)
#define MIN_9000US      (TIME_9000US - TOL_9000US)
#define MAX_9000US      (TIME_9000US + TOL_9000US)

// Header Space (4.5ms)
#define TIME_4500US     TIME_TICS(4500)
#define TOL_4500US      ((TIME_4500US)/3)
#define MIN_4500US      (TIME_4500US - TOL_4500US)
#define MAX_4500US      (TIME_4500US + TOL_4500US)

// Bit Mark (560us)
#define TIME_560US      TIME_TICS(560)
#define TOL_560US       ((TIME_560US)/3)
#define MIN_560US       (TIME_560US - TOL_560US)
#define MAX_560US       (TIME_560US + TOL_560US)

// Logic '1' Space (1690us)
#define TIME_1690US     TIME_TICS(1690)
#define TOL_1690US      ((TIME_1690US)/3)
#define MIN_1690US      (TIME_1690US - TOL_1690US)
#define MAX_1690US      (TIME_1690US + TOL_1690US)

/* Decoding State Machine */
typedef enum
{
    WAIT_FOR_9MS_HEADER = 0,
    WAIT_FOR_4MS_SPACE  = 1,
    WAIT_FOR_BIT_MARK   = 2,
    MEASURE_BIT_VALUE   = 3,
} IrState_t;

/* Internal State Variables */
static volatile IrState_t step = WAIT_FOR_9MS_HEADER;
static volatile uint8_t pulse_width = 0; // Previously 'czas'
static volatile uint32_t raw_data = 0;   // Previously 'data_32'
static volatile uint8_t bit_counter = 0; // Previously 'bit_number'

/* External Variables */
volatile uint8_t data_IR = 0;
volatile bool data_ready = false;

void ir_init(void)
{
    /* Configure GPIO */
    DDRD &= ~(1 << DATA_RECEIVER); // Set as Input
    PORTD |= (1 << DATA_RECEIVER); // Enable Pull-up resistor
    
    /* Configure External Interrupt (INT0) */
    EIMSK |= (1 << INT0);  // Enable INT0
    
    /* Set Trigger Source: Any Logical Change (Rising & Falling) */
    // ISC01 = 0, ISC00 = 1 -> Any Change
    EICRA &= ~(1 << ISC01); 
    EICRA |= (1 << ISC00); 
    
    /* Configure Timer0 for time measurement */
    // Prescaler 1024
    TCCR0B = (1 << CS02) | (1 << CS00); 
}

ISR(INT0_vect)
{
    /* Capture elapsed time and reset timer */
    pulse_width = TCNT0;
    TCNT0 = 0;

    switch(step)
    {
        case WAIT_FOR_9MS_HEADER:
            // Check for Rising Edge (End of 9ms Mark)
            if(PIND & (1 << DATA_RECEIVER))
            {
                if(pulse_width > MIN_9000US && pulse_width < MAX_9000US)
                {
                    step = WAIT_FOR_4MS_SPACE;
                    bit_counter = 0;
                    raw_data = 0;
                }
                else
                {
                    step = WAIT_FOR_9MS_HEADER; // Error, reset
                }
            }
            break;
        
        case WAIT_FOR_4MS_SPACE:
            // Check for Falling Edge (End of 4.5ms Space / Start of Bit Mark)
            if(!(PIND & (1 << DATA_RECEIVER)))
            {
                if(pulse_width > MIN_4500US && pulse_width < MAX_4500US)
                {
                    step = WAIT_FOR_BIT_MARK;
                }
                else
                {
                    step = WAIT_FOR_9MS_HEADER; // Error
                }
            }
            break;
        
        case WAIT_FOR_BIT_MARK:
            // Check for Rising Edge (End of 560us Mark)
            if(PIND & (1 << DATA_RECEIVER))
            {
                if(pulse_width > MIN_560US && pulse_width < MAX_560US)
                {
                    step = MEASURE_BIT_VALUE;
                }
                else
                {
                    step = WAIT_FOR_9MS_HEADER; // Error
                }
            }
            break;
        
        case MEASURE_BIT_VALUE:
            // Check for Falling Edge (End of Bit Space)
            if(!(PIND & (1 << DATA_RECEIVER)))
            {
                // Check if it is Logic '0' (Short space ~560us)
                if(pulse_width > MIN_560US && pulse_width < MAX_560US)
                {
                    raw_data &= ~(1UL << bit_counter); // Clear bit
                    bit_counter++;
                    step = WAIT_FOR_BIT_MARK;
                }
                // Check if it is Logic '1' (Long space ~1690us)
                else if(pulse_width > MIN_1690US && pulse_width < MAX_1690US)
                {
                    raw_data |= (1UL << bit_counter); // Set bit
                    bit_counter++;
                    step = WAIT_FOR_BIT_MARK;
                }
                else
                {
                    step = WAIT_FOR_9MS_HEADER; // Timing error
                }

                // Check for completion (32 bits)
                if(bit_counter == 32)
                {
                    step = WAIT_FOR_9MS_HEADER;
                    data_ready = true;
                    // Extract Command Byte (Bits 16-23 usually)
                    data_IR = (uint8_t)(raw_data >> 16);
                }
            }
            break;
        
        default:
            step = WAIT_FOR_9MS_HEADER;
            break;
    }
}
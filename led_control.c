/**
 * @file led_control.c
 * @brief Implementation of LED control sequences via Shift Register.
 */

#include "led_control.h"
#include "ir_decoder.h"

#include <avr/io.h>

/* Pin Definitions for SN74HC595 */
#define RCLK  PD3   // Latch Pin
#define SRCLK PD4   // Clock Pin
#define SRCLR PD5   // Clear Pin
#define OE    PD6   // Output Enable
#define SER   PD7   // Serial Data

/* Animation Delays */
#define DELAY_BINARY_COUNT_MS   30
#define DELAY_SEQUENCE_MS       100

/**
 * @brief Initializes GPIOs and Timer1 for sequencing.
 */
void led_init(void)
{
    /* Configure Shift Register GPIOs as Output */
    DDRD |= (1 << SER) | (1 << RCLK) | (1 << SRCLK) | (1 << SRCLR) | (1 << OE);
    
    /* Set initial state */
    PORTD |= (1 << SRCLR); // Disable Clear (Active Low)
    led_set_pattern(0);    // Turn off all LEDs

    /* Configure Timer1 for delay generation (CTC Mode) */
    // WGM12 = CTC Mode
    // CS11 + CS10 = Prescaler 64
    TCCR1B = (1 << CS10) | (1 << CS11) | (1 << WGM12);
    
    /* Set Compare Match for 1ms tick */
    // Formula: (16MHz / 64 / 1000Hz) - 1 = 249
    OCR1A = 249;
}

/**
 * @brief Blocking delay that checks for IR interruption.
 * @param time_ms Duration in milliseconds.
 * @return 1 if IR data received (break), 0 otherwise.
 */
static uint8_t check_wait(uint8_t time_ms)
{
    for(int j = 0; j < time_ms; j++)
    {
        if(data_ready)
        {
            return 1; // Break loop if new command arrived
        }
        
        // Reset timer and wait for 1ms flag
        TCNT1 = 0;
        TIFR1 |= (1 << OCF1A); // Clear flag by writing 1
        while(!(TIFR1 & (1 << OCF1A))); // Wait for match
    }
    return 0;
}

void led_set_pattern(uint8_t data)
{
    for(int i = 0; i <= 7; i++)
    {
        // Set Data Pin
        if(data & (1 << i))
        {
            PORTD |= (1 << SER);
        }
        else
        {
            PORTD &= ~(1 << SER);
        }
        
        // Pulse Shift Clock (SRCLK)
        PORTD |= (1 << SRCLK);
        PORTD &= ~(1 << SRCLK);
    }
    
    // Pulse Latch Clock (RCLK) to update outputs
    PORTD |= (1 << RCLK);
    PORTD &= ~(1 << RCLK);
}

void led_all_on(void)
{
    PORTD &= ~(1 << OE); // Enable Output
    led_set_pattern(0);  // Note: Depending on wiring, 0 might mean ON or OFF. Assuming logic matches HW.
}

void led_all_off(void)
{
    led_set_pattern(0);
    PORTD |= (1 << OE);  // Disable Output
}

void led_binary_counting(void)
{
    for(int i = 0; i < 255; i++)
    {
        led_set_pattern(i + 1);
        if(check_wait(DELAY_BINARY_COUNT_MS)) return;
    }
    
    if(check_wait(DELAY_BINARY_COUNT_MS)) return;
    led_set_pattern(0);
}

void led_sequence_left(void)
{
    for(int i = 1; i <= 128; i = i * 2)
    {
        led_set_pattern(i);
        if(check_wait(DELAY_SEQUENCE_MS)) return;
        led_set_pattern(0);
    }
}

void led_sequence_right(void)
{
    for(int i = 128; i >= 1; i = i / 2)
    {
        led_set_pattern(i);
        if(check_wait(DELAY_SEQUENCE_MS)) return;
    }
    led_set_pattern(0);
}
/**
 * Project:     NEC IR Decoder with SN74HC595 Shift Register LED Control (Bare Metal)
 * Microcontroller: ATmega328P / Arduino Uno (16 MHz)
 * Author:      Kamil Kupis
 * Date:        2025-12-18
 * Description: 
 * Main application entry point. Initializes the IR receiver subsystem
 * and handles processing of received data in the main loop.
 */

#include "config.h"
#include "led_control.h"
#include "ir_decoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>

/** * Remote Control Command Codes (NEC Protocol)
 * Device: Generic Mini IR Remote (Arduino Starter Kit type)
 * Note: Key codes are specific to this hardware model.
 */
#define IR_KEY_1      0x16
#define IR_KEY_2      0x19
#define IR_KEY_OK     0x40
#define IR_KEY_LEFT   0x44
#define IR_KEY_RIGHT  0x43

int main(void)
{
    /* Initialize Hardware Subsystems */
    ir_init();   // Initialize IR receiver subsystem (Ext. Int. + Timer0)
    led_init();  // Initialize LED driver subsystem (SN74HC595 + Timer1)
    
    /* Enable global interrupts to start processing */
    sei();       
    
    /* Main Event Loop */
    while(1)
    {
        if(data_ready)
        {
            data_ready = false; // Acknowledge data
            
            switch(data_IR)
            {
                case IR_KEY_1:
                    led_all_on();
                    break;
                
                case IR_KEY_2:
                    led_all_off();
                    break;
                
                case IR_KEY_OK:
                    led_binary_counting();
                    break;
                
                case IR_KEY_LEFT:
                    led_sequence_left();
                    break;
                
                case IR_KEY_RIGHT:
                    led_sequence_right();
                    break;
                    
                default:
                    // Optional: Handle unknown keys
                    break;
            }
        }
    }
}
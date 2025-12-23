/**
 * @file led_control.h
 * @brief Driver for SN74HC595 Shift Register and LED sequencing logic.
 */

#ifndef LED_CONTROL_H_
#define LED_CONTROL_H_

#include <avr/io.h>

/* Initialization */
void led_init(void);

/* Low-level control */
void led_set_pattern(uint8_t data); // Previously 'print_diod'
void led_all_on(void);              // Previously 'diod_on'
void led_all_off(void);             // Previously 'diod_off'

/* Animation sequences */
void led_binary_counting(void);
void led_sequence_left(void);       // Previously 'left_side'
void led_sequence_right(void);      // Previously 'right_side'

#endif /* LED_CONTROL_H_ */
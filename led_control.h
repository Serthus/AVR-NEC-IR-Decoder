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
void led_set_pattern(uint8_t data);
void led_all_on(void);
void led_all_off(void);

/* Animation sequences */
void led_binary_counting(void);
void led_sequence_left(void);'
void led_sequence_right(void);

#endif /* LED_CONTROL_H_ */

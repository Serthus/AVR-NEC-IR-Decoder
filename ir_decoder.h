/**
 * @file ir_decoder.h
 * @brief NEC IR Protocol Decoder Interface.
 */

#ifndef IR_DECODER_H_
#define IR_DECODER_H_

#include <avr/io.h>
#include <stdbool.h>

/* Global flags and data containers */
extern volatile uint8_t data_IR;   // Decoded command byte
extern volatile bool data_ready;   // Flag: true when new data is available

/**
 * @brief Initializes the IR receiver subsystem.
 * Configures Timer0 and External Interrupt INT0.
 */
void ir_init(void);

#endif /* IR_DECODER_H_ */
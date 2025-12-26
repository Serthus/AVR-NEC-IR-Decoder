# AVR NEC IR Decoder with Shift Register Control

## Project Overview
This project implements a **Bare-Metal C** driver for decoding NEC Infrared (IR) protocol signals on an ATmega328P microcontroller. It demonstrates precise timing measurement using hardware timers and interrupts without relying on high-level Arduino libraries.

The decoded commands control an LED array driven by an **SN74HC595 Shift Register**, showcasing efficient GPIO usage and hardware abstraction separation.

## Key Features
* **Protocol:** NEC IR Transmission Protocol (38kHz carrier).
* **Architecture:** Bare-Metal C (No Arduino framework overhead).
* **Timing:** Timer0 used for pulse width measurement (State Machine approach).
* **LED Control:** Timer1 used for non-blocking LED sequencing and animation.
* **Driver:** Custom driver for 74HC595 Serial-to-Parallel Shift Register.

## Hardware Requirements
* **Microcontroller:** ATmega328P (e.g., Arduino Uno board).
* **IR Receiver:** VS1838B or compatible (38kHz).
* **Shift Register:** SN74HC595.
* **Display:** 8x LEDs bar or discrete LEDs.
* **Remote:** Generic NEC-compatible remote (e.g., Car MP3 Remote).

## Pin Configuration (Wiring)
| Component | Pin Name | ATmega328P Pin | Arduino Pin |
| :--- | :--- | :--- | :--- |
| **IR Receiver** | Data | PD2 (INT0) | D2 |
| **74HC595** | SER (Data) | PD7 | D7 |
| **74HC595** | OE (Enable) | PD6 | D6 |
| **74HC595** | SRCLR (Clear)| PD5 | D5 |
| **74HC595** | SRCLK (Clock)| PD4 | D4 |
| **74HC595** | RCLK (Latch) | PD3 | D3 |

## Build & Flash
1.  Open the project in **Microchip Studio** (formerly Atmel Studio) or VS Code.
2.  Verify `F_CPU` in `config.h` matches your clock speed (default: 16MHz).
3.  Compile and upload using AVRDUDE or your preferred ISP programmer.

## Pin Configuration (Wiring)

### Control Logic (Microcontroller <-> Shift Register)
| Component | Pin Name | Wire Color | ATmega328P Pin | Arduino Pin |
| :--- | :--- | :--- | :--- | :--- |
| **IR Receiver** | Data | Green | PD2 (INT0) | D2 |
| **74HC595** | SER (Data) | Yellow | PD7 | D7 |
| **74HC595** | OE (Enable) | Yellow | PD6 | D6 |
| **74HC595** | SRCLR (Clear)| Yellow | PD5 | D5 |
| **74HC595** | SRCLK (Clock)| Yellow | PD4 | D4 |
| **74HC595** | RCLK (Latch) | Yellow | PD3 | D3 |

### LED Output (Shift Register <-> LEDs)
The output pins of the SN74HC595 are connected to the LEDs using **Yellow** wires.
* **QA - QH (Pins 15, 1-7):** Connected to LED anodes via 220Ω resistors.
* **GND:** All LED cathodes connected to Ground via **Black** wires.

## License
MIT License.

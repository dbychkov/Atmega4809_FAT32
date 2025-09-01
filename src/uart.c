#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "uart.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef UART_BAUD
#define UART_BAUD 9600UL
#endif

#define UART_BIT_US (1000000UL / (UART_BAUD))

static inline void uart_delay_bit(void) {
    _delay_us(UART_BIT_US);
}

void uart_init(void) {
    UART_TX_PORT.DIRSET = UART_TX_PIN_bm; // TX as output
    UART_TX_PORT.OUTSET = UART_TX_PIN_bm; // idle high
}

void uart_putc(char c) {
    // Start bit
    UART_TX_VPORT.OUT &= ~(UART_TX_PIN_bm);
    uart_delay_bit();

    // 8 data bits, LSB first
    for (uint8_t i = 0; i < 8; i++) {
        if (c & 0x01) {
            UART_TX_VPORT.OUT |= UART_TX_PIN_bm;
        } else {
            UART_TX_VPORT.OUT &= ~(UART_TX_PIN_bm);
        }
        uart_delay_bit();
        c >>= 1;
    }

    // Stop bit
    UART_TX_VPORT.OUT |= UART_TX_PIN_bm;
    uart_delay_bit();
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

void uart_print_hex8(uint8_t v) {
    const char hex[] = "0123456789ABCDEF";
    uart_putc(hex[(v >> 4) & 0x0F]);
    uart_putc(hex[v & 0x0F]);
}

void uart_print_hex32(uint32_t v) {
    uart_print_hex8((v >> 24) & 0xFF);
    uart_print_hex8((v >> 16) & 0xFF);
    uart_print_hex8((v >> 8) & 0xFF);
    uart_print_hex8(v & 0xFF);
}


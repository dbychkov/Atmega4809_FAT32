#ifndef UART_H
#define UART_H

#include <stdint.h>

// Bit-banged UART TX for portability across ATmega4809 pin mux variants.
// Default: PORTB PIN0 as TX. Adjust here to suit your wiring.

#define UART_TX_PORT PORTB
#define UART_TX_VPORT VPORTB
#define UART_TX_PIN_bm PIN0_bm

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_print_hex8(uint8_t v);
void uart_print_hex32(uint32_t v);

#endif // UART_H

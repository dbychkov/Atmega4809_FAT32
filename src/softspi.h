#ifndef SOFTSPI_H
#define SOFTSPI_H

#include <stdint.h>
#include <avr/io.h>

// Software SPI pins (adjust to your wiring)
// CS:   PORTC PIN0
// SCK:  PORTC PIN1
// MOSI: PORTC PIN2
// MISO: PORTC PIN3

#define SOFTSPI_CS_PORT   PORTC
#define SOFTSPI_CS_VPORT  VPORTC
#define SOFTSPI_CS_bm     PIN0_bm

#define SOFTSPI_SCK_PORT  PORTC
#define SOFTSPI_SCK_VPORT VPORTC
#define SOFTSPI_SCK_bm    PIN1_bm

#define SOFTSPI_MOSI_PORT  PORTC
#define SOFTSPI_MOSI_VPORT VPORTC
#define SOFTSPI_MOSI_bm    PIN2_bm

#define SOFTSPI_MISO_PORT  PORTC
#define SOFTSPI_MISO_VPORT VPORTC
#define SOFTSPI_MISO_bm    PIN3_bm

void softspi_init(void);
void softspi_select(void);
void softspi_deselect(void);
uint8_t softspi_transfer(uint8_t v);
void softspi_tx(uint8_t v);
uint8_t softspi_rx(void);

#endif // SOFTSPI_H

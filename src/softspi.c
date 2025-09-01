#include "softspi.h"
#include <util/delay.h>

#ifndef SOFTSPI_HALF_PERIOD_US
#define SOFTSPI_HALF_PERIOD_US 1
#endif

static inline void spi_delay(void) {
    _delay_us(SOFTSPI_HALF_PERIOD_US);
}

void softspi_init(void) {
    SOFTSPI_CS_PORT.DIRSET = SOFTSPI_CS_bm;
    SOFTSPI_SCK_PORT.DIRSET = SOFTSPI_SCK_bm;
    SOFTSPI_MOSI_PORT.DIRSET = SOFTSPI_MOSI_bm;
    SOFTSPI_MISO_PORT.DIRCLR = SOFTSPI_MISO_bm;

    SOFTSPI_CS_PORT.OUTSET = SOFTSPI_CS_bm;   // deselect
    SOFTSPI_SCK_PORT.OUTCLR = SOFTSPI_SCK_bm;  // clock low
    SOFTSPI_MOSI_PORT.OUTCLR = SOFTSPI_MOSI_bm;
}

void softspi_select(void) {
    SOFTSPI_CS_VPORT.OUT &= ~(SOFTSPI_CS_bm);
}

void softspi_deselect(void) {
    SOFTSPI_CS_VPORT.OUT |= SOFTSPI_CS_bm;
}

uint8_t softspi_transfer(uint8_t v) {
    uint8_t r = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (v & 0x80) {
            SOFTSPI_MOSI_VPORT.OUT |= SOFTSPI_MOSI_bm;
        } else {
            SOFTSPI_MOSI_VPORT.OUT &= ~(SOFTSPI_MOSI_bm);
        }
        spi_delay();
        SOFTSPI_SCK_VPORT.OUT |= SOFTSPI_SCK_bm;
        spi_delay();
        r <<= 1;
        if (SOFTSPI_MISO_VPORT.IN & SOFTSPI_MISO_bm) {
            r |= 1;
        }
        SOFTSPI_SCK_VPORT.OUT &= ~(SOFTSPI_SCK_bm);
        v <<= 1;
    }
    return r;
}

void softspi_tx(uint8_t v) {
    (void)softspi_transfer(v);
}

uint8_t softspi_rx(void) {
    return softspi_transfer(0xFF);
}


#include "sdcard.h"
#include "softspi.h"
#include "uart.h"
#include <util/delay.h>

static bool sdhc = false; // true if block addressing

static uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc) {
    softspi_deselect();
    softspi_tx(0xFF);
    softspi_select();
    softspi_tx(0xFF);

    softspi_tx(0x40 | cmd);
    softspi_tx((arg >> 24) & 0xFF);
    softspi_tx((arg >> 16) & 0xFF);
    softspi_tx((arg >> 8) & 0xFF);
    softspi_tx(arg & 0xFF);
    softspi_tx(crc);

    for (uint8_t i = 0; i < 10; i++) {
        uint8_t r = softspi_rx();
        if ((r & 0x80) == 0) return r;
    }
    return 0xFF;
}

static uint8_t sd_acmd(uint8_t cmd, uint32_t arg, uint8_t crc) {
    (void)sd_send_cmd(55, 0, 0x01);
    return sd_send_cmd(cmd, arg, crc);
}

bool sd_init(void) {
    softspi_init();
    // Send >74 clock cycles with CS high
    softspi_deselect();
    for (uint8_t i = 0; i < 10; i++) softspi_tx(0xFF);

    uint8_t r = sd_send_cmd(0, 0, 0x95); // CMD0 GO_IDLE_STATE
    if (r != 0x01) {
        uart_puts("CMD0 fail R1=0x"); uart_print_hex8(r); uart_puts("\r\n");
        return false;
    }

    r = sd_send_cmd(8, 0x000001AA, 0x87); // CMD8, check voltage range
    if (r == 0x01) {
        // read rest of R7
        uint8_t r7[4];
        for (uint8_t i = 0; i < 4; i++) r7[i] = softspi_rx();
        if (r7[2] != 0x01 || r7[3] != 0xAA) {
            uart_puts("CMD8 pattern mismatch\r\n");
            return false;
        }
    } else if (r & 0x04) {
        // Illegal command: V1 card
    } else {
        uart_puts("CMD8 error R1=0x"); uart_print_hex8(r); uart_puts("\r\n");
        return false;
    }

    // ACMD41 loop with HCS
    for (uint16_t i = 0; i < 10000; i++) {
        r = sd_acmd(41, 0x40000000UL, 0x01);
        if (r == 0x00) break;
        _delay_ms(1);
    }
    if (r != 0x00) {
        uart_puts("ACMD41 init timeout\r\n");
        return false;
    }

    // CMD58 read OCR
    r = sd_send_cmd(58, 0, 0x01);
    if (r != 0x00) {
        uart_puts("CMD58 fail\r\n");
        return false;
    }
    uint8_t ocr[4];
    for (uint8_t i = 0; i < 4; i++) ocr[i] = softspi_rx();
    sdhc = (ocr[0] & 0x40) != 0;

    softspi_deselect();
    softspi_tx(0xFF);
    return true;
}

static bool sd_read_block(uint32_t addr, uint8_t *buf) {
    uint8_t r = sd_send_cmd(17, addr, 0x01); // CMD17 READ_SINGLE_BLOCK
    if (r != 0x00) return false;
    uint16_t timeout = 60000;
    uint8_t token;
    do {
        token = softspi_rx();
    } while (token == 0xFF && --timeout);
    if (token != 0xFE) return false;
    for (uint16_t i = 0; i < 512; i++) buf[i] = softspi_rx();
    (void)softspi_rx(); (void)softspi_rx(); // CRC
    softspi_deselect();
    softspi_tx(0xFF);
    return true;
}

bool sd_read_sector(uint32_t lba, uint8_t *buf) {
    uint32_t addr = sdhc ? lba : (lba << 9);
    return sd_read_block(addr, buf);
}


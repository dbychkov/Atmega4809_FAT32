#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "softspi.h"
#include "sdcard.h"
#include "fat32.h"

static void blink(void) {
    PORTD.DIRSET = PIN7_bm;
    PORTD.OUTTGL = PIN7_bm;
}

int main(void) {
    uart_init();
    uart_puts("\r\nATmega4809 SD FAT32 demo (no Arduino)\r\n");

    softspi_init();
    if (!sd_init()) {
        uart_puts("SD init failed\r\n");
        for (;;) {
            blink();
            _delay_ms(300);
        }
    }

    fat32_fs_t fs;
    if (!fat32_mount(&fs)) {
        uart_puts("FAT32 mount failed\r\n");
        for (;;) {
            blink();
            _delay_ms(300);
        }
    }

    uart_puts("Root directory:\r\n");
    fat32_list_root(&fs);

    for (;;) {
        blink();
        _delay_ms(1000);
    }
}


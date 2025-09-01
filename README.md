## Atmega4809 SD FAT32 Root Directory Listing (No Arduino)

This project targets the ATmega4809 (DIP-40) and is intended for MPLAB X IDE as an Existing Makefile Project. It will list the FAT32 root directory from an SD card and print to a serial console.

### Open in MPLAB X

- File → New Project → Other → Existing Makefile Project with Existing Sources
- Select this folder and finish. Build will use `avr-gcc` per the included Makefile.

### Toolchain

- Requires `avr-gcc` and `avr-libc` on your system for building via the Makefile.
- MPLAB X can also be configured to call `make` with these tools.

### Pin Assignments (change in headers as needed)

- UART TX (bit-banged default): `PORTB, PIN0` to a USB-UART adapter (3.3V/5V level as appropriate)
- SD Card (Software SPI defaults):
  - CS:   `PORTC, PIN0`
  - SCK:  `PORTC, PIN1`
  - MOSI: `PORTC, PIN2`
  - MISO: `PORTC, PIN3`

Adjust these in `src/uart.h` and `src/softspi.h` if wiring differs.

### Build

```bash
make -C Atmega4809_SD_FAT32
```

Artifacts are placed in `build/`.

### Flash

Produce HEX via the build, then flash with your preferred programmer (e.g., Atmel-ICE).

### Notes

- This project avoids Arduino and uses plain C with AVR registers. The initial scaffold includes bit-banged UART and SPI to simplify pin routing across boards. You can replace with hardware USART/SPI easily by updating modules.

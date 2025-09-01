#ifndef SDCARD_H
#define SDCARD_H

#include <stdint.h>
#include <stdbool.h>

bool sd_init(void);
bool sd_read_sector(uint32_t lba, uint8_t *buf); // 512 bytes

#endif // SDCARD_H

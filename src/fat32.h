#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>
#include <stdbool.h>

typedef struct fat32_fs_s {
    uint32_t fat_start_lba;
    uint32_t sectors_per_fat;
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint32_t first_data_lba;
    uint32_t root_cluster;
} fat32_fs_t;

bool fat32_mount(fat32_fs_t *fs);
void fat32_list_root(fat32_fs_t *fs);

#endif // FAT32_H

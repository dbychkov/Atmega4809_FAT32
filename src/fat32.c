#include "fat32.h"
#include "sdcard.h"
#include "uart.h"
#include <string.h>

static uint8_t sector[512];

static uint16_t rd16(const uint8_t *p) { return (uint16_t)p[0] | ((uint16_t)p[1] << 8); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

bool fat32_mount(fat32_fs_t *fs) {
    if (!sd_read_sector(0, sector)) return false; // MBR or VBR
    uint32_t lba = 0;
    if (sector[0x1FE] == 0x55 && sector[0x1FF] == 0xAA && sector[0x1C2] == 0x0B) {
        lba = rd32(&sector[0x1C6]);
    }
    if (!sd_read_sector(lba, sector)) return false; // VBR

    fs->bytes_per_sector = rd16(&sector[11]);
    fs->sectors_per_cluster = sector[13];
    uint16_t rsvd = rd16(&sector[14]);
    uint8_t num_fats = sector[16];
    uint32_t sectors_per_fat = rd32(&sector[36]);
    uint32_t root_cluster = rd32(&sector[44]);

    fs->sectors_per_fat = sectors_per_fat;
    fs->fat_start_lba = lba + rsvd;
    fs->first_data_lba = fs->fat_start_lba + (num_fats * sectors_per_fat);
    fs->root_cluster = root_cluster;

    return true;
}

static uint32_t cluster_to_lba(fat32_fs_t *fs, uint32_t cluster) {
    return fs->first_data_lba + (cluster - 2) * fs->sectors_per_cluster;
}

static uint32_t fat_read_entry(fat32_fs_t *fs, uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fs->fat_start_lba + (fat_offset / 512);
    uint16_t ent_offset = fat_offset % 512;
    if (!sd_read_sector(fat_sector, sector)) return 0x0FFFFFFF;
    uint32_t val = rd32(&sector[ent_offset]) & 0x0FFFFFFF;
    return val;
}

static void print_name_8_3(const uint8_t *name) {
    // Skip deleted and dot entries
    if (name[0] == 0xE5 || name[0] == 0x00 || name[0] == '.') return;
    char out[13];
    uint8_t p = 0;
    for (uint8_t i = 0; i < 8 && name[i] != ' '; i++) out[p++] = name[i];
    if (name[8] != ' ') {
        out[p++] = '.';
        for (uint8_t i = 8; i < 11 && name[i] != ' '; i++) out[p++] = name[i];
    }
    out[p] = '\0';
    if (p) { uart_puts(out); }
}

void fat32_list_root(fat32_fs_t *fs) {
    uint32_t cluster = fs->root_cluster;
    while (cluster >= 2 && cluster < 0x0FFFFFF8) {
        for (uint8_t s = 0; s < fs->sectors_per_cluster; s++) {
            uint32_t lba = cluster_to_lba(fs, cluster) + s;
            if (!sd_read_sector(lba, sector)) return;
            for (uint16_t off = 0; off < 512; off += 32) {
                uint8_t first = sector[off + 0];
                if (first == 0x00) return; // no more entries
                uint8_t attr = sector[off + 11];
                if ((attr & 0x0F) == 0x0F) continue; // LFN
                if (first == 0xE5) continue; // deleted

                print_name_8_3(&sector[off]);
                if (attr & 0x10) {
                    uart_puts("/\r\n");
                } else {
                    uart_puts("\tSZ=0x");
                    uint32_t sz = rd32(&sector[off + 28]);
                    uart_print_hex32(sz);
                    uart_puts("\r\n");
                }
            }
        }
        uint32_t next = fat_read_entry(fs, cluster);
        if (next >= 0x0FFFFFF8) break;
        if (next == 0x0FFFFFF7 || next == 0) break;
        cluster = next;
    }
}


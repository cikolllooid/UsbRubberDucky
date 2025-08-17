
#ifndef SDCARD_H
#define SDCARD_H

#include "fatfs.h"
#include "stdint.h"

void sd_cs_low(void);
void sd_cs_high(void);
FRESULT mount_fileSystem(void);
FRESULT unmount_fileSystem(void);
void myprintf(const char *fmt, ...);
FRESULT format_fileSystem(void);
FRESULT check_and_mount_filesystem(void);
FRESULT process_data(uint8_t *binBuffer, uint16_t expected_size);

#endif


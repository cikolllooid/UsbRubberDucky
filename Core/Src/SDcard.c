#include "SDcard.h"
#include "main.h"

FATFS FatFs;
FIL fil;
int fs_mounted = 0;
BYTE work[_MAX_SS];

void myprintf(const char *fmt, ...){
	static char buffer[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	int len = strlen(buffer);
}

FRESULT format_fileSystem(void) {
    FRESULT res;
    sd_cs_low();

    res = f_mkfs("", FM_FAT32, 0, work, sizeof(work));
    if (res != FR_OK) {
        printf("Formatting failed. Error: %d\n", res);
        sd_cs_high();
        return res;
    }

    sd_cs_high();
    return FR_OK;
}

FRESULT check_and_mount_filesystem(void) {
    FRESULT res;
    res = mount_fileSystem();
    if (res != FR_OK) {
        res = format_fileSystem();
        if (res != FR_OK) {
            printf("Error during formatting. Error: %d\n", res);
            return res;
        }
        res = mount_fileSystem();
        if (res != FR_OK) {
            printf("Failed to mount filesystem after formatting. Error: %d\n", res);
            return res;
        }
    }

    return FR_OK;
}


void sd_cs_low(void) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}

void sd_cs_high(void) {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

FRESULT mount_fileSystem(void) {
    if (fs_mounted) {
        myprintf("Filesystem already mounted.\r\n");
        return FR_OK;
    }

    sd_cs_low();

    FRESULT fres = f_mount(&FatFs, "", 1);
    if (fres != FR_OK) {
        myprintf("f_mount error (%i)\r\n", fres);

        if (fres == FR_NO_FILESYSTEM) {
            myprintf("No valid filesystem found. Attempting to format the SD card.\r\n");
            BYTE work[100];
            fres = f_mkfs("", FM_FAT, 0, work, sizeof(work));
            if (fres == FR_OK) {
                myprintf("Filesystem created successfully. Remounting...\r\n");
                fres = f_mount(&FatFs, "", 1);
                if (fres == FR_OK) {
                    myprintf("Filesystem mounted successfully.\r\n");
                    fs_mounted = 1;
                } else {
                    myprintf("Remounting failed. Error (%i)\r\n", fres);
                }
            } else {
                myprintf("Formatting failed. Error (%i)\r\n", fres);
            }
        }
        sd_cs_high();
        while (1);
    } else {
        fs_mounted = 1;
        myprintf("Filesystem mounted successfully.\r\n");
    }

    sd_cs_high();

    return FR_OK;
}

FRESULT unmount_fileSystem(void) {
    if (!fs_mounted) {
        myprintf("Filesystem not mounted, nothing to unmount.\r\n");
        return FR_OK;
    }

    sd_cs_low();

    FRESULT fres = f_mount(NULL, "", 0);
    if (fres != FR_OK) {
        myprintf("f_mount(NULL) error (%i)\r\n", fres);
        sd_cs_high();
        return fres;
    }

    fs_mounted = 0;
    myprintf("Filesystem unmounted successfully.\r\n");

    sd_cs_high();

    return FR_OK;
}

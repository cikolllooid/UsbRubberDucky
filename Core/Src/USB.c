#include "USB.h"
#include "main.h"
#include "SDcard.h"
#include "keyboardik.h"

uint8_t fileCountUSB = 0;
int usbInit = 0;
char *menuUsb[2] = {"Attack", "Select File"};
int selectedFileUSB = 0;
int menuInitializedUSB = 0;

uint8_t get_modifier_flag(const char *mod) {
    if (strcmp(mod, "CTRL") == 0) return 0x01;
    if (strcmp(mod, "SHIFT") == 0) return 0x02;
    if (strcmp(mod, "ALT") == 0) return 0x04;
    if (strcmp(mod, "GUI") == 0) return 0x08;
    return 0x00;
}

int usbAttack = 0;
int btn_state_usb = 0;

void parse_and_execute(char *line) {
    char *newline = strchr(line, '\n');
    if (newline) *newline = '\0';

    if (strncmp(line, "DELAY ", 6) == 0) {
        int delay_ms = atoi(line + 6);
        HAL_Delay(delay_ms);
    } else if (strncmp(line, "HOLD ", 5) == 0) {
        const char *but = line + 5;
        hold_key(but);
    } else if (strncmp(line, "RELEASE ", 8) == 0) {
        release_key();
    } else if (strncmp(line, "STRING ", 7) == 0) {
        const char *text = line + 7;
        send_command(text);
    } else if (strncmp(line, "ENTER", 5) == 0) {
        send_key(0x28);
    } else if (strncmp(line, "CMD", 3) == 0) {
        open_cmd();
    } else {
        char *token = strtok(line, " ");
        uint8_t modifier = 0;
        char *key = NULL;

        while (token) {
            if (strcmp(token, "CTRL") == 0 || strcmp(token, "SHIFT") == 0 || strcmp(token, "ALT") == 0 || strcmp(token, "GUI") == 0) {
                modifier |= get_modifier_flag(token);
            } else {
                key = token;
            }
            token = strtok(NULL, " ");
        }

        if (key != NULL) {
        	keyboardReportDes keycode = get_hid_code(key);
            send_key_with_modifier(modifier | keycode.MODIFIER, keycode.KEYCODE1);
        } else {
            printf("Неизвестная команда: %s\n", line);
        }
    }
}


void execute_script() {
    FIL file;
    FRESULT res;
    char line[128];

    sd_cs_low();
    mount_fileSystem();

	char fileName[64];
	snprintf(fileName, sizeof(fileName), "USB.txt");

    res = f_open(&file, fileName, FA_READ);
    if (res != FR_OK) {
        printf("Не удалось открыть файл: %d\n", res);
        return;
    }

    while (f_gets(line, sizeof(line), &file)) {

        if (strncmp(line, "LOOP ", 5) == 0) {
            int loop_count = atoi(line + 5);  // �?спользуем 5, не 6 (без пробела в конце условия)
            char *loop_commands[100];
            int command_count = 0;

            while (f_gets(line, sizeof(line), &file)) {
                if (strncmp(line, "ENDLOOP", 7) == 0) break;

                loop_commands[command_count] = strdup(line);
                command_count++;
                if (command_count >= 100) break;
            }

            for (int i = 0; i < loop_count; i++) {
                for (int j = 0; j < command_count; j++) {
                    parse_and_execute(loop_commands[j]);
                }
            }

            for (int j = 0; j < command_count; j++) {
                free(loop_commands[j]);
            }

        } else {
            parse_and_execute(line);
        }
    }


    f_close(&file);
    sd_cs_high();
}

#include "keyboardik.h"
#include "usbd_hid.h"

keyboardReportDes HIDkeyBoard = {0, 0, 0, 0, 0, 0, 0, 0};

void send_key(uint8_t key) {
    HIDkeyBoard.KEYCODE1 = key;
    USBD_HID_SendReport(&hUsbDeviceFS, &HIDkeyBoard, sizeof(HIDkeyBoard));
    HAL_Delay(50);

    HIDkeyBoard.KEYCODE1 = 0x00;
    USBD_HID_SendReport(&hUsbDeviceFS, &HIDkeyBoard, sizeof(HIDkeyBoard));
    HAL_Delay(50);
}

void hold_key(const char *key) {
	keyboardReportDes but = get_hid_code(key);
    USBD_HID_SendReport(&hUsbDeviceFS, &but, sizeof(but));
    HAL_Delay(50);
}

void release_key() {
    HIDkeyBoard.KEYCODE1 = 0x00;
    USBD_HID_SendReport(&hUsbDeviceFS, &HIDkeyBoard, sizeof(HIDkeyBoard));
    HAL_Delay(50);
}

void send_key_with_modifier(uint8_t modifier, uint8_t key) {
    HIDkeyBoard.MODIFIER = modifier;
    HIDkeyBoard.KEYCODE1 = key;
    USBD_HID_SendReport(&hUsbDeviceFS, &HIDkeyBoard, sizeof(HIDkeyBoard));
    HAL_Delay(50);

    HIDkeyBoard.MODIFIER = 0x00;
    HIDkeyBoard.KEYCODE1 = 0x00;
    USBD_HID_SendReport(&hUsbDeviceFS, &HIDkeyBoard, sizeof(HIDkeyBoard));
    HAL_Delay(50);
}

void send_command(const char* command) {
    char ch_str[2] = {0, 0};

    while (*command) {
        ch_str[0] = *command++;
        keyboardReportDes k = get_hid_code(ch_str);

        if (k.KEYCODE1) {
            send_key_with_modifier(k.MODIFIER, k.KEYCODE1);
        }
    }
}

keyboardReportDes get_hid_code(char *ch) {
	keyboardReportDes key = {0, 0, 0, 0, 0, 0, 0, 0};

	if (strlen(ch) == 1) {
		char c = ch[0];
		if (c >= 'a' && c <= 'z') {
			key.KEYCODE1 = 0x04 + (c - 'a');
		} else if (c >= 'A' && c <= 'Z') {
			key.KEYCODE1 = 0x04 + (c - 'A');
			key.MODIFIER = 0x02;
		} else if (c >= '1' && c <= '9') {
			key.KEYCODE1 = 0x1E + (c - '1');
		} else if (c == '0') {
			key.KEYCODE1 = 0x27;
		} else if (c == '\n') {
			key.KEYCODE1 = 0x28;
		} else if (c == ' ') {
			key.KEYCODE1 = 0x2C;
		} else if (c == '=') {
			key.KEYCODE1 = 0x2E;
		} else if (c == '/') {
			key.KEYCODE1 = 0x38;
		} else if (c == '.') {
			key.KEYCODE1 = 0x37;
		} else if (c == '\'') {
			key.KEYCODE1 = 0x34;
		} else if (c == '!') {
			key.KEYCODE1 = 0x1E; key.MODIFIER = 0x02;
		} else if (c == '@') {
			key.KEYCODE1 = 0x1F; key.MODIFIER = 0x02;
		} else if (c == '#') {
			key.KEYCODE1 = 0x20; key.MODIFIER = 0x02;
		} else if (c == '$') {
			key.KEYCODE1 = 0x21; key.MODIFIER = 0x02;
		} else if (c == '%') {
			key.KEYCODE1 = 0x22; key.MODIFIER = 0x02;
		} else if (c == '^') {
			key.KEYCODE1 = 0x23; key.MODIFIER = 0x02;
		} else if (c == '&') {
			key.KEYCODE1 = 0x24; key.MODIFIER = 0x02;
		} else if (c == '*') {
			key.KEYCODE1 = 0x25; key.MODIFIER = 0x02;
		} else if (c == '(') {
			key.KEYCODE1 = 0x26; key.MODIFIER = 0x02;
		} else if (c == ')') {
			key.KEYCODE1 = 0x27; key.MODIFIER = 0x02;
		}
	} else if (strcmp(ch, "esc") == 0) {
		key.KEYCODE1 = 0x29;
	}

	return key;
}


void open_cmd() {
    send_key_with_modifier(0x08, 0x15);
    HAL_Delay(1000);

    send_key(0x06);
    send_key(0x10);
    send_key(0x07);
    send_key(0x28);
}

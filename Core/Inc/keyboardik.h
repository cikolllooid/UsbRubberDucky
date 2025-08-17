// menu.h
#ifndef KEYBOARDIK_H
#define KEYBOARDIK_H

#include "usb_device.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
typedef struct {
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}keyboardReportDes;

// Функции меню
void send_key(uint8_t key);
void send_key_with_modifier(uint8_t modifier, uint8_t key);
void send_command(const char* command);
void open_cmd();
void hold_key(const char *key);
void release_key();
keyboardReportDes get_hid_code(char *c);
//void send_key_with_modifier_reserved(uint8_t modifier, uint8_t reserved, uint8_t key);
//void send_modifier_reserved(uint8_t modifier, uint8_t reserved);
#endif // MENU_H

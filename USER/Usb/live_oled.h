#ifndef MULTIPAD_LIVE_OLED_H
#define MULTIPAD_LIVE_OLED_H

#include <stdbool.h>
#include <stdint.h>

#define LIVE_OLED_CAPABILITY 0x4c

bool live_oled_receive(const uint8_t *data, uint32_t length);
void live_oled_poll(void);
void live_oled_menu_applied(uint8_t menu);
void live_oled_mode_changed(void);
bool live_oled_read(uint8_t screen, uint32_t offset, uint8_t *pixels, uint16_t size);

#endif

#ifndef MULTIPAD_USB_UPDATE_H
#define MULTIPAD_USB_UPDATE_H
#include <stdint.h>
void usb_update_boot(void);
void usb_update_poll(void);
void usb_update_receive(const uint8_t *data, uint32_t size);
void usb_update_enter(void);
void usb_update_reenumerate(void);
#endif

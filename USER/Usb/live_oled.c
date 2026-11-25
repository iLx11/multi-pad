#include "live_oled.h"
#include "flash_user.h"
#include "oled_42.h"
#include "pet_mode.h"
#include "screen_readback.h"
#include "usbd_cdc_if.h"
#include <string.h>

enum {
    HEADER_SIZE = 11,
    CHUNK_SIZE = 45,
    FRAME_SIZE = 360,
    CHUNK_COUNT = FRAME_SIZE / CHUNK_SIZE,
    CLEAR_OVERLAY = 0xff,
    STATUS_OK = 0,
    STATUS_INVALID = 1,
    STATUS_STALE = 2
};

static const uint8_t request_magic[] = {0xaa, 0xbb, 0xce, 0x44};
static uint8_t reply[] = {'M', 'P', 'O', 'L', 0, 0, 0, 0, 0, 0, 0, 0};
static bool reply_pending;
static uint8_t request[HEADER_SIZE + CHUNK_SIZE];
static volatile uint32_t request_size;
static uint8_t active_menu;
static uint8_t overlay_mask;
static uint32_t generation;
static uint8_t staged_screen;
static uint8_t next_chunk;
static uint8_t frame[FRAME_SIZE];
static uint8_t displayed[2][FRAME_SIZE];

bool live_oled_read(uint8_t screen, uint32_t offset, uint8_t *pixels, uint16_t size) {
    if (screen < 1 || screen > 2 || offset >= FRAME_SIZE || size > FRAME_SIZE - offset ||
        !(overlay_mask & (1u << (screen - 1)))) return false;
    memcpy(pixels, displayed[screen - 1] + offset, size);
    return true;
}

bool live_oled_receive(const uint8_t *data, uint32_t length) {
    if ((length != HEADER_SIZE && length != sizeof(request)) ||
        memcmp(data, request_magic, sizeof(request_magic)) != 0) {
        return false;
    }
    if (!request_size) {
        memcpy(request, data, length);
        request_size = length;
    }
    return true;
}

void live_oled_menu_applied(uint8_t menu) {
    active_menu = menu;
    overlay_mask = 0;
    staged_screen = 0;
    generation++;
}

static void restore_oled(uint8_t screen) {
    uint8_t saved[2 * FRAME_SIZE] = {0};
    if (active_menu) load_menu_from_flash(active_menu, saved, sizeof(saved), 1);
    oled_42_show_pic_index(0, 0, 72, 40, saved + (screen - 1) * FRAME_SIZE, 1, screen - 1);
}

void live_oled_mode_changed(void) {
    for (uint8_t screen = 1; screen <= 2; screen++) {
        if (overlay_mask & (1u << (screen - 1))) restore_oled(screen);
    }
    screen_readback_overlay_changed();
    live_oled_menu_applied(active_menu);
}

static void send_reply(void) {
    if (CDC_Transmit_FS(reply, sizeof(reply)) == USBD_OK) reply_pending = false;
}

void live_oled_poll(void) {
    if ((!request_size && !reply_pending) || !CDC_TransmitReady_FS()) return;
    if (reply_pending) {
        send_reply();
        return;
    }

    uint8_t screen = request[5];
    uint8_t chunk = request[6];
    uint32_t expected = (uint32_t)request[7] | (uint32_t)request[8] << 8 |
                        (uint32_t)request[9] << 16 | (uint32_t)request[10] << 24;
    uint8_t status = STATUS_OK;
    if (screen == 0 && request_size == HEADER_SIZE) {
        staged_screen = 0;
    } else if (screen < 1 || screen > 2) {
        status = STATUS_INVALID;
    } else if ((!active_menu && !pet_mode_is_active()) || generation != expected) {
        status = STATUS_STALE;
    } else if (chunk == CLEAR_OVERLAY && request_size == HEADER_SIZE) {
        restore_oled(screen);
        overlay_mask &= (uint8_t)~(1u << (screen - 1));
        screen_readback_overlay_changed();
        staged_screen = 0;
    } else if (chunk >= CHUNK_COUNT || request_size != sizeof(request)) {
        status = STATUS_INVALID;
    } else {
        if (chunk == 0) {
            staged_screen = screen;
            next_chunk = 0;
        }
        if (staged_screen != screen || chunk != next_chunk) {
            status = STATUS_INVALID;
        } else {
            memcpy(frame + chunk * CHUNK_SIZE, request + HEADER_SIZE, CHUNK_SIZE);
            next_chunk++;
            if (next_chunk == CHUNK_COUNT) {
                oled_42_show_pic_index(0, 0, 72, 40, frame, 1, screen - 1);
                memcpy(displayed[screen - 1], frame, FRAME_SIZE);
                overlay_mask |= (uint8_t)(1u << (screen - 1));
                screen_readback_overlay_changed();
                staged_screen = 0;
            }
        }
    }

    reply[4] = request[4];
    reply[5] = status;
    reply[6] = active_menu;
    reply[7] = overlay_mask;
    for (uint8_t i = 0; i < 4; i++) reply[8 + i] = (uint8_t)(generation >> (8 * i));
    reply_pending = true;
    request_size = 0;
    send_reply();
}

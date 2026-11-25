#include "screen_readback.h"
#include "flash_user.h"
#include "live_oled.h"
#include "lcd_user.h"
#include <stdio.h>
#include "pet_mode.h"
#include "pet_care.h"
#include "usbd_cdc_if.h"
#include <string.h>

enum {
    REQUEST_SIZE = 17,
    HEADER_SIZE = 19,
    CHECKSUM_SIZE = 2,
    MAX_CHUNK = 224,
    COLOR_SIZE = 320 * 172 * 2,
    OLED_SIZE = 72 * 40 / 8,
    PERFORMANCE = 0xf9,
    PET_CONTROL = 0xfa,
    PET_EVENTS = 0xfb,
    MENU_CONFIG = 0xfc,
    PET_STATE = 0xfd,
    BUTTON_STATE = 0xfe,
    SCREEN_INFO = 0xff,
    STATUS_OK = 0,
    STATUS_EMPTY,
    STATUS_STALE,
    STATUS_INVALID,
};

static const uint8_t request_magic[] = {0xaa, 0xbb, 0xc1, 0x53};
static const uint8_t response_magic[] = {0x4d, 0x50, 0x53, 1};
static uint8_t request[REQUEST_SIZE];
static volatile bool request_pending;
static uint8_t response[HEADER_SIZE + MAX_CHUNK + CHECKSUM_SIZE];
static uint16_t response_size;
static uint8_t active_menu;
static bool pixels_dirty;
static uint32_t generation;
static uint16_t held_buttons;
static uint16_t pressed_buttons;
static uint8_t performance_menu;
static uint32_t performance_updated;
static uint8_t last_metrics[3];

static void clear_performance(void) {
    if (!performance_menu) return;
    performance_menu = 0;
    if (!pet_mode_is_active()) restore_menu_color();
}

static void show_performance(const uint8_t *metrics, bool first) {
    static const char *labels[] = {"CPU", "MEM", "DISK"};
    const uint16_t background = 0x1106, track = 0x29ca, accent = 0x575a;
    if (first) {
        lcd_fill(0, 0, 320, 172, background, 0);
        lcd_show_string(14, 10, (const uint8_t *)"PC PERFORMANCE", WHITE, background, 24, 0, 0);
    }
    for (uint8_t i = 0; i < 3; i++) {
        if (!first && last_metrics[i] == metrics[i]) continue;
        const uint16_t y = 50 + 36 * i;
        uint8_t value[8];
        if (metrics[i] == 255) snprintf((char *)value, sizeof(value), "   --");
        else snprintf((char *)value, sizeof(value), "%4u%%", (unsigned)metrics[i]);
        lcd_show_string(14, y, (const uint8_t *)labels[i], WHITE, background, 16, 0, 0);
        lcd_show_string(262, y, value, WHITE, background, 16, 0, 0);
        lcd_fill(66, y + 3, 254, y + 13, track, 0);
        if (metrics[i] > 0 && metrics[i] <= 100)
            lcd_fill(66, y + 3, 66 + 188 * metrics[i] / 100, y + 13, accent, 0);
    }
    memcpy(last_metrics, metrics, sizeof(last_metrics));
}

void button_readback_record(uint8_t index, bool down) {
    uint16_t bit = (uint16_t)(1u << index);
    if (down) {
        held_buttons |= bit;
        pressed_buttons |= bit;
    } else {
        held_buttons &= (uint16_t)~bit;
    }
}

static uint16_t read_u16(const uint8_t *data) {
    return (uint16_t)(data[0] | (uint16_t)data[1] << 8);
}

static uint32_t read_u32(const uint8_t *data) {
    return read_u16(data) | (uint32_t)read_u16(data + 2) << 16;
}

static void write_u16(uint8_t *data, uint16_t value) {
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8);
}

static unsigned write_room_foods(uint8_t *out, uint8_t scene) {
    PetFloorFood foods[PET_FLOOR_CAPACITY];
    uint8_t count = pet_mode_room_foods(scene, foods);
    *out++ = count;
    for (unsigned i = 0; i < count; i++, out += 6) {
        out[0] = foods[i].id;
        write_u16(out + 1, (uint16_t)foods[i].x);
        write_u16(out + 3, (uint16_t)foods[i].y);
        out[5] = foods[i].stage;
    }
    return 1 + 6 * count;
}

static void write_u32(uint8_t *data, uint32_t value) {
    write_u16(data, (uint16_t)value);
    write_u16(data + 2, (uint16_t)(value >> 16));
}

static uint16_t crc16(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0xffff;
    while (length--) {
        crc ^= (uint16_t)*data++ << 8;
        for (uint8_t bit = 0; bit < 8; bit++) {
            crc = (uint16_t)((crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1);
        }
    }
    return crc;
}

bool screen_readback_receive(const uint8_t *data, uint32_t length) {
    if (length != REQUEST_SIZE || memcmp(data, request_magic, sizeof(request_magic)) != 0) {
        return false;
    }
    if (!request_pending) {
        memcpy(request, data, REQUEST_SIZE);
        request_pending = true;
    }
    return true;
}

void screen_readback_applied(uint8_t menu) {
    active_menu = menu;
    performance_menu = 0;
    pixels_dirty = false;
    generation++;
}

void screen_readback_overlay_changed(void) {
    generation++;
}

void screen_readback_invalidate(uint8_t menu) {
    if (menu == active_menu && !pixels_dirty) {
        pixels_dirty = true;
        generation++;
    }
}

bool screen_readback_needs_refresh(uint8_t menu) {
    return menu == active_menu && pixels_dirty;
}

static void send_response(void) {
    if (CDC_Transmit_FS(response, response_size) == USBD_OK) response_size = 0;
}

void screen_readback_poll(void) {
    if (performance_menu && (pet_mode_is_active() || HAL_GetTick() - performance_updated >= 7000))
        clear_performance();
    if ((!request_pending && !response_size) || !CDC_TransmitReady_FS()) return;
    if (response_size) {
        send_response();
        return;
    }

    uint16_t id = read_u16(request + 4);
    uint32_t requested_generation = read_u32(request + 6);
    uint8_t screen = request[10];
    uint32_t offset = read_u32(request + 11);
    uint16_t length = read_u16(request + 15);
    uint8_t status = STATUS_OK;
    uint8_t *pixels = response + HEADER_SIZE;

    if (screen == PERFORMANCE) {
        uint8_t metrics[] = {(uint8_t)requested_generation, (uint8_t)(requested_generation >> 8),
                             (uint8_t)(requested_generation >> 16)};
        if (length || offset > 10 || requested_generation > 0xffffff) {
            status = STATUS_INVALID;
        } else if (!offset) {
            clear_performance();
        } else if (offset != active_menu || pixels_dirty || pet_mode_is_active()) {
            status = STATUS_STALE;
        } else if ((metrics[0] > 100 && metrics[0] != 255) ||
                   (metrics[1] > 100 && metrics[1] != 255) ||
                   (metrics[2] > 100 && metrics[2] != 255)) {
            status = STATUS_INVALID;
        } else {
            show_performance(metrics, !performance_menu);
            performance_menu = (uint8_t)offset;
            performance_updated = HAL_GetTick();
        }
    } else if (screen == BUTTON_STATE) {
        if (offset != 0 || length != 0) {
            status = STATUS_INVALID;
        } else {
            write_u16(pixels, held_buttons);
            write_u16(pixels + 2, pressed_buttons);
            pressed_buttons = 0;
            length = 4;
        }
    } else if (screen == MENU_CONFIG) {
        if (offset != 0 || length != 0) {
            status = STATUS_INVALID;
        } else {
            length = 31;
            load_config(pixels, length);
        }
    } else if (screen == PET_EVENTS) {
        if (offset || length) {
            status = STATUS_INVALID;
        } else {
            PetEvent events[PET_EVENTS_PER_REPLY];
            bool overrun, more;
            const uint8_t count = pet_mode_events(requested_generation, events, &overrun, &more);
            *pixels = (overrun ? 1u : 0u) | (more ? 2u : 0u);
            for (uint8_t i = 0; i < count; i++) {
                uint8_t *event = pixels + 1 + 12 * i;
                write_u32(event, events[i].sequence);
                write_u32(event + 4, events[i].time_ms);
                event[8] = events[i].kind;
                event[9] = events[i].value;
                event[10] = events[i].source;
                event[11] = events[i].detail;
            }
            length = 1 + 12 * count;
        }
    } else if (screen == PET_STATE || screen == PET_CONTROL) {
        const uint8_t command = (uint8_t)offset, value = (uint8_t)(offset >> 8);
        const bool decor_supported = requested_generation >= 4 && requested_generation <= 10;
        const bool move_hutch = screen == PET_CONTROL && decor_supported && command == 14;
        if (length || (move_hutch ? offset >> 25 : offset > UINT16_MAX) || (screen == PET_STATE && offset)) {
            status = STATUS_INVALID;
        } else {
            if (screen == PET_CONTROL && offset) {
                if (move_hutch) status = pet_mode_move_hutch((offset >> 8) & 511, offset >> 17);
                else if (command >= 23 && command <= 26 && requested_generation != 10) status = STATUS_INVALID;
                else if (!decor_supported && (command == 14 || command == 15)) status = STATUS_INVALID;
                else if (command == 22 && (requested_generation < 5 || requested_generation > 10 || (requested_generation == 5 && value > 4))) status = STATUS_INVALID;
                else if (requested_generation < 7 && command == 3 && value >= PET_STAND_WATCH) status = STATUS_INVALID;
                else if ((requested_generation < 8 || requested_generation > 10) && command == 3 && value == PET_BINKY) status = STATUS_INVALID;
                else if ((requested_generation < 9 || requested_generation > 10) && command == 3 && value >= PET_GROOM_FUR) status = STATUS_INVALID;
                else if (requested_generation <= 3 && value == 1 && (command == 7 || command == 11 || command == 13))
                    status = pet_mode_control(15, 0); /* Former rabbit_hutch toy now visits the decoration. */
                else if (command == 24 && value <= 100) status = pet_mode_control(command, 100 - value);
                else status = pet_mode_control(command, value);
            }
            pixels[0] = pet_mode_is_active();
            PetAction action = pet_mode_action();
            pixels[1] = (requested_generation < 7 && action >= PET_STAND_WATCH) ||
                        (requested_generation < 8 && action == PET_BINKY) ||
                        ((requested_generation < 9 || requested_generation > 10) && action >= PET_GROOM_FUR) ? PET_IDLE : action;
            write_u16(pixels + 2, pet_mode_x());
            write_u32(pixels + 4, pet_mode_frames());
            pixels[8] = pet_mode_fruit();
            pixels[9] = pet_mode_hour();
            pixels[10] = pet_mode_season();
            pixels[11] = pet_mode_queue_depth();
            length = screen == PET_STATE ? 11 : 12;
            if (screen == PET_CONTROL && requested_generation >= 2 && requested_generation <= 10) {
                const PetCare *care = pet_care_get();
                pixels[12] = care->satiety;
                pixels[13] = 100 - care->energy; /* USB generations 2–10 encode fatigue. */
                pixels[14] = care->mood;
                pixels[15] = care->affection;
                pixels[16] = care->scene;
                pixels[17] = pet_mode_active_toy();
                if (requested_generation < 7 && pixels[17] == 3) pixels[17] = 0;
                pixels[18] = pixels[17] ? pet_mode_toy_phase() : 0;
                length = 19;
                if (requested_generation >= 3) {
                    memcpy(pixels + 19, care->bindings, PET_KEY_COUNT);
                    pixels[27] = (care->follow_time ? 1u : 0u) | (pet_mode_is_bedtime() ? 2u : 0u);
                    pixels[28] = pet_mode_minute();
                    pixels[29] = care->placed[PET_GARDEN];
                    pixels[30] = care->placed[PET_NURSERY];
                    pixels[31] = pet_mode_toy_drop();
                    length = 32 + write_room_foods(pixels + 32, care->scene);
                    if (decor_supported) {
                        write_u16(pixels + length, care->hutch_x);
                        pixels[length + 2] = care->hutch_y;
                        pixels[length + 3] = pet_mode_y();
                        length += 4;
                    }
                    if (requested_generation >= 5) {
                        pixels[length] = pet_mode_head_pat_ticks(pixels + length + 1);
                        length += 2;
                        if (requested_generation >= 6) {
                            uint8_t other = care->scene == PET_GARDEN ? PET_NURSERY : PET_GARDEN;
                            pixels[length++] = pet_mode_pat_phase();
                            pixels[length++] = pet_mode_room_flags();
                            pixels[length++] = pet_mode_room_drop(other);
                            length += write_room_foods(pixels + length, other);
                            if (requested_generation == 7) pixels[length++] = 7;
                            if (requested_generation >= 8) {
                                write_u16(pixels + length, pet_mode_action_age());
                                length += 2;
                                pixels[length++] = requested_generation;
                            }
                        }
                    }
                }
            }
        }
    } else if (active_menu == 0 || pixels_dirty) {
        status = STATUS_EMPTY;
    } else if (screen == SCREEN_INFO) {
        if (offset != 0 || length != 0) status = STATUS_INVALID;
    } else if (requested_generation != generation) {
        status = STATUS_STALE;
    } else if (screen > 2 || length == 0 || length > MAX_CHUNK) {
        status = STATUS_INVALID;
    } else {
        uint32_t size = screen == 0 ? COLOR_SIZE : OLED_SIZE;
        if (offset >= size || length > size - offset) {
            status = STATUS_INVALID;
        } else if (screen == 0) {
            read_color_screen(active_menu, offset, pixels, length);
        } else if (!live_oled_read(screen, offset, pixels, length)) {
            uint8_t oled[2 * OLED_SIZE];
            load_menu_from_flash(active_menu, oled, sizeof(oled), 1);
            memcpy(pixels, oled + (screen - 1) * OLED_SIZE + offset, length);
        }
    }
    if (status != STATUS_OK) length = 0;

    memcpy(response, response_magic, sizeof(response_magic));
    write_u16(response + 4, id);
    response[6] = status;
    response[7] = screen;
    write_u32(response + 8, generation);
    write_u32(response + 12, offset);
    write_u16(response + 16, length);
    response[18] = active_menu;
    uint16_t size = HEADER_SIZE + length;
    write_u16(response + size, crc16(response, size));
    /* A handshake can take the endpoint after the readiness check. Retry this
       exact reply without applying the command a second time. */
    response_size = size + CHECKSUM_SIZE;
    request_pending = false;
    send_response();
}

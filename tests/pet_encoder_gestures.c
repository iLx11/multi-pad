#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "encoder1.h"
#include "encoder3.h"
#include "pet_care.h"
#include "pet_mode.h"

static uint32_t now, counter, cursor;
static bool held;
static uint8_t macro;
static unsigned macros, pats;
static PetEvent last_pat;
static uint16_t lcd[172][320], before[172][320];

uint32_t HAL_GetTick(void) { return now; }
uint8_t HAL_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin) { (void)port; (void)pin; return !held; }
void HAL_GPIO_Init(GPIO_TypeDef *port, GPIO_InitTypeDef *init) { (void)port; (void)init; }
uint32_t get_encoder3_counter(void) { return counter; }
void set_encoder3_counter(uint8_t value) { counter = value; }
void tim4_init(void) {}
void encoder1_init(void) {}
void encoder2_init(void) {}
void encoder1_scan(void) {}
void encoder2_scan(void) {}
void load_menu(uint8_t menu) { (void)menu; }
void parse_json_value(uint8_t value) { macro = value; macros++; }
void oled_42_show_num(uint8_t n, uint8_t mode, uint8_t i) { (void)n; (void)mode; (void)i; }
void button_readback_record(uint8_t index, bool down) { assert(index == 10 && down == held); }
void live_oled_mode_changed(void) {}
void restore_menu_color(void) {}
void erase_flash_sector(uint32_t sector) { (void)sector; assert(0); }
void write_flash_page(uint8_t *buf, uint32_t addr, uint16_t n) { (void)buf; (void)addr; (void)n; assert(0); }
void read_flash(uint8_t *buf, uint32_t addr, uint16_t n) { (void)buf; (void)addr; (void)n; assert(0); }
void lcd_show_pic_index(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *buf, uint8_t i) {
    assert(x + w <= 320 && y + h <= 172 && i == 0);
    for (unsigned j = 0; j < h; j++) for (unsigned k = 0; k < w; k++) {
        unsigned p = 2 * (j*w+k);
        lcd[y+j][x+k] = (uint16_t)(buf[p] << 8 | buf[p+1]);
    }
}

static void advance(unsigned duration) { while (duration--) { now++; pet_mode_poll(); } }
static void scan(bool down, unsigned count) { held = down; counter = count; encoder3_scan(); }

static void read_pats(void) {
    PetEvent events[PET_EVENTS_PER_REPLY];
    bool overrun, more;
    do {
        unsigned count = pet_mode_events(cursor, events, &overrun, &more);
        assert(!overrun);
        for (unsigned i = 0; i < count; i++) {
            cursor = events[i].sequence;
            if (events[i].kind == PET_EVENT_HEAD_PETTED) { last_pat = events[i]; pats++; }
        }
    } while (more);
}

static void expect_pat(unsigned count, uint8_t kind, uint8_t source) {
    read_pats();
    assert(pats == count && last_pat.value == kind && last_pat.source == source && last_pat.detail == 3);
}

static void expect_reaction(uint8_t ticks, uint8_t kind) {
    uint8_t actual_kind;
    assert(pet_mode_head_pat_ticks(&actual_kind) == ticks && actual_kind == kind);
}

static void gestures_and_framebuffer(void) {
    assert(pet_mode_control(1, 1) == 0);
    assert(pet_mode_control(3, PET_IDLE) == 0);
    advance(90);
    memcpy(before, lcd, sizeof(lcd));
    scan(true, 0); advance(65); scan(false, 0);
    expect_pat(1, 0, 1); expect_reaction(120, 0);
    const PetCare *care = pet_care_get();
    assert(care->affection == 36 && care->mood == 71 && care->satiety == 65);
    advance(140); assert(memcmp(lcd, before, sizeof(lcd)) != 0);
    advance(1160); expect_reaction(0, 0);
    assert(memcmp(lcd, before, sizeof(lcd)) == 0); /* Entire overlay cleared. */
    scan(true, 0); advance(1500); assert(pet_mode_is_active());
    scan(true, 124); expect_pat(2, 1, 1); expect_reaction(120, 1);
    unsigned phase = pet_mode_pat_phase();
    int held_x = pet_mode_x();
    advance(110);
    assert(pet_mode_pat_phase() == phase && pet_mode_x() == held_x);
    scan(true, 4);
    assert(pet_mode_pat_phase() != phase); expect_pat(3, 2, 1);
    unsigned reversed = pet_mode_pat_phase();
    advance(1250); assert(pet_mode_pat_phase() == reversed && pet_mode_x() == held_x); encoder3_callback(CLOCKWISE); /* Ordinary direction reported during an owned press. */
    expect_pat(4, 1, 1); expect_reaction(120, 1);
    scan(false, 0); expect_pat(4, 1, 1);
    assert(care->affection == 37 && macros == 0 && pet_mode_is_active());
    scan(false, 4); assert(macros == 1 && macro == 28 + ANTICLOCKWISE);
    scan(true, 0); advance(1300); assert(pet_mode_is_active()); scan(false, 0);
    assert(!pet_mode_is_active() && macros == 1); expect_pat(4, 1, 1);
    scan(true, 124); scan(false, 0); assert(macros == 4 && macro == 28 + ENCODER_UP);
    scan(true, 0); advance(1250); assert(pet_mode_is_active()); scan(false, 0);
    assert(macros == 6 && macro == 28 + ENCODER_UP); /* Pair an already-sent work DOWN. */
    scan(true, 0); assert(pet_mode_control(1, 0) == 0); assert(pet_mode_control(1, 1) == 0);
    scan(true, 124); scan(false, 0); expect_pat(4, 1, 1); assert(macros == 6);
}

static void concurrent_activities(void) {
    assert(pet_mode_control(3, PET_SLEEP) == 0); advance(3500);
    assert(pet_mode_is_bedtime() && pet_mode_action() == PET_SLEEP);
    uint8_t mood = pet_care_get()->mood, satiety = pet_care_get()->satiety;
    assert(pet_mode_control(22, 1) == 0); expect_pat(5, 1, 2);
    assert(pet_mode_is_bedtime() && pet_mode_action() == PET_SLEEP);
    advance(1350); assert(pet_mode_control(22, 4) == 0); expect_pat(6, 2, 2);
    assert(pet_care_get()->mood == mood + 1 && pet_care_get()->satiety == satiety);
    assert(pet_mode_control(22, 5) == 0); expect_pat(6, 2, 2);
    assert(pet_mode_control(2, 1) == 0);
    for (unsigned i = 0; i < 5000 && pet_mode_action() != PET_EAT; i++) advance(1);
    assert(pet_mode_action() == PET_EAT && pet_mode_fruit() == 1);
    scan(true, 4); scan(false, 0); expect_pat(7, 2, 1);
    assert(pet_mode_action() == PET_EAT && pet_mode_fruit() == 1);
    advance(1200); assert(pet_care_get()->satiety == satiety + 12);
    assert(pet_mode_control(7, 2) == 0);
    for (unsigned i = 0; i < 5000 && pet_mode_toy_phase() != 2; i++) advance(1);
    assert(pet_mode_active_toy() == 2 && pet_mode_toy_phase() == 2);
    scan(true, 0); advance(80); scan(false, 0); expect_pat(8, 0, 1);
    assert(pet_mode_active_toy() == 2 && pet_mode_toy_phase() == 2);
}

static void table_and_food_positions(void) {
    pet_mode_control(1, 0);
    pet_mode_control(1, 1);
    pet_mode_control(6, 0);
    assert(pet_mode_control(3, PET_INSPECT_TABLE) == 3);
    pet_mode_control(6, 1);
    assert(pet_mode_control(3, PET_INSPECT_TABLE) == 0);
    advance(2900);
    assert(pet_mode_action() == PET_INSPECT_TABLE && pet_mode_active_toy() == 3);
    assert(pet_mode_x() == 98 && pet_mode_y() == 116);
    advance(4200);
    assert(pet_mode_active_toy() == 0 && pet_mode_y() == 139);
    assert(pet_mode_control(3, PET_STAND_WATCH) == 0);
    advance(3200);
    assert(pet_mode_action() == PET_STAND_WATCH && pet_mode_y() == 139);
    advance(780);
    assert(pet_mode_action() != PET_STAND_WATCH);
    pet_care_adjust(-100, 0, 0, 0);
    assert(pet_care_get()->satiety == 0);
    assert(pet_mode_control(2, 1) == 0 && pet_mode_control(2, 2) == 0);
    for (unsigned id = 1; id <= 2; id++) {
        for (unsigned t = 0; t < 6000 && pet_mode_action() != PET_EAT; t++) advance(1);
        PetFloorFood food[PET_FLOOR_CAPACITY];
        assert(pet_mode_foods(food) > 0 && food[0].id == id && pet_mode_fruit() == id);
        int distance = food[0].x - pet_mode_x();
        assert(distance == (id == 1 ? 28 : -28));
        advance(851);
        assert(pet_care_get()->satiety == 12 * id);
    }
}

static void dance_and_window(void) {
    assert(pet_mode_control(3, PET_BINKY) == 0);
    const int origin = pet_mode_x(), facing = (pet_mode_room_flags() & 1) ? 1 : -1;
    advance(300);
    assert((pet_mode_x() - origin) * facing > 0 && (pet_mode_room_flags() & 1) == (facing > 0));
    advance(340);
    assert(pet_mode_action() == PET_BINKY && (pet_mode_room_flags() & 1) == (facing < 0));
    advance(530);
    assert(pet_mode_action() == PET_IDLE && pet_mode_x() == origin + 36 * facing);
    assert((pet_mode_room_flags() & 1) == (facing < 0));

    assert(pet_mode_control(9, 0) == 0);
    assert(pet_mode_control(4, 10) == 0);
    advance(150);
    const uint16_t sky = lcd[28][124], frame_colour = lcd[28][137];
    assert(pet_mode_control(4, 18) == 0);
    advance(150);
    assert(sky != lcd[28][124] && frame_colour == lcd[28][137]);
}

int main(void) {
    pet_care_init(0); /* In-memory care. Separate NOR tests exercise durable storage. */
    gestures_and_framebuffer();
    concurrent_activities();
    table_and_food_positions();
    dance_and_window();
    const PetAction grooming[] = {PET_GROOM_FUR, PET_GROOM_EAR};
    for (unsigned i = 0; i < 2; ++i) {
        int x = pet_mode_x(), affection = pet_care_get()->affection;
        assert(pet_mode_control(3, grooming[i]) == 0);
        advance(400);
        memcpy(before, lcd, sizeof(lcd));
        advance(410);
        assert(pet_mode_action() == grooming[i] && memcmp(before, lcd, sizeof(lcd)) != 0);
        advance(i ? 2250 : 1750);
        assert(pet_mode_action() == PET_IDLE && pet_mode_x() == x && pet_care_get()->affection == affection);
    }
    pet_care_adjust(100, 100, 0, 0);
    bool saw_lookout = false, saw_table = false;
    for (unsigned i = 0; i < 1200 && !(saw_lookout && saw_table); i++) {
        advance(75);
        saw_lookout |= pet_mode_action() == PET_STAND_WATCH;
        saw_table |= pet_mode_action() == PET_INSPECT_TABLE;
    }
    assert(saw_lookout && saw_table);
    assert(pet_mode_control(6, 0) == 0 && pet_mode_control(6, 1) == 0);
    assert(pet_mode_control(3, PET_INSPECT_TABLE) == 0);
    pet_care_adjust(-100, 0, 0, 0);
    assert(pet_mode_active_toy() != 0);
    assert(pet_mode_control(2, 1) == 0);
    advance(675);
    assert(pet_mode_active_toy() == 0 && pet_mode_fruit() == 1);
    advance(4500);
    assert(pet_care_get()->satiety == 12);
    assert(pet_mode_queue_depth() == 0);
    puts("Encoder 3 head-pat tests passed");
    return 0;
}

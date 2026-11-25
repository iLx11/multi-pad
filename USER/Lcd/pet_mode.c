#include "pet_mode.h"
#include "lcd_user.h"

#include "pet_art.inc"

enum {
    SLICE_ROWS = 4,
    FRAME_MS = 48,
    SLEEP_AFTER_MS = 18000,
    ENCODER_HOLD_MS = 1200,
    RABBIT_WIDTH = 56,
    RABBIT_HEIGHT = 44,
    GROUND = 139,
    WALK_MIN = 46,
    WALK_MAX = 274,
    DIRTY_MARGIN = 38,
    DIRTY_TOP = 73,
    DIRTY_BOTTOM = 143,
    FRUIT_COUNT = 8,
    FRUIT_RADIUS = 9,
    FRUIT_GROUND = 129,
    FALL_MS = 650,
    FRUIT_EAT_MS = 850
};

#define RGB565(r, g, b) (((r) >> 3) << 11 | ((g) >> 2) << 5 | ((b) >> 3))
#define HEX565(rgb) RGB565(((rgb) >> 16) & 255, ((rgb) >> 8) & 255, (rgb) & 255)

#include "pet_scene_art.inc"

_Static_assert(sizeof(fruit_art) / sizeof(fruit_art[0]) == FRUIT_COUNT, "Each key needs fruit art");

typedef struct {
    uint16_t left, right, row, bottom;
    int16_t x, y;
    int8_t direction;
    uint8_t sprite;
    PetAction action;
    uint32_t elapsed;
    int16_t fruit_x, fruit_y;
    uint8_t fruit, hour, season;
    bool bowl;
} PetFrame;

static bool active;
static PetAction action;
static uint32_t action_since, last_input, last_frame, last_motion, frames;
static int16_t x, last_x;
static int8_t direction;
static uint32_t walk_remainder;
static PetFrame frame;
static uint8_t pixels[LCD_WIDTH * SLICE_ROWS * 2];
static uint16_t scene_palette[24];
static uint8_t hour = 8, season;
static bool redraw_scene;
static uint8_t fruit, fruit_head, fruit_pending, fruit_queue[FRUIT_COUNT], keys_seen;
static uint32_t key_at[FRUIT_COUNT];
static int16_t fruit_x, previous_fruit_x, previous_fruit_top;
static bool previous_fruit_visible;

static bool encoder_down, encoder_rotated, encoder_toggled, encoder_claimed;
static uint32_t encoder_since;

static void set_action(PetAction next, uint32_t now) {
    action = next;
    action_since = now;
}

static void start_mode(uint32_t now) {
    active = true;
    x = last_x = 78;
    direction = 1;
    frames = walk_remainder = 0;
    last_input = last_frame = last_motion = now;
    fruit = fruit_head = fruit_pending = keys_seen = 0;
    previous_fruit_visible = false;
    redraw_scene = true;
    set_action(PET_WALK, now);
    frame = (PetFrame){0};
}

bool pet_mode_is_active(void) { return active; }
PetAction pet_mode_action(void) { return active ? action : PET_IDLE; }
uint16_t pet_mode_x(void) { return active ? (uint16_t)x : 0; }
uint32_t pet_mode_frames(void) { return frames; }
uint8_t pet_mode_fruit(void) { return active ? fruit : 0; }
uint8_t pet_mode_hour(void) { return hour; }
uint8_t pet_mode_season(void) { return season; }

bool pet_mode_encoder_button(bool down) {
    const uint32_t now = HAL_GetTick();
    if (down) {
        encoder_down = true;
        encoder_since = now;
        encoder_rotated = encoder_toggled = false;
        encoder_claimed = active;
    } else if (encoder_down) {
        encoder_down = false;
        if (encoder_claimed && active && !fruit && !encoder_toggled && !encoder_rotated &&
            now - encoder_since >= 40) {
            last_input = now;
            set_action(PET_EAT, now);
        }
    }
    return encoder_claimed;
}

void pet_mode_encoder_rotated(void) {
    if (encoder_down) encoder_rotated = true;
}

void pet_mode_key_down(uint8_t key) {
    if (!active || key >= FRUIT_COUNT || fruit_pending >= FRUIT_COUNT) return;
    const uint32_t now = HAL_GetTick();
    const uint8_t bit = (uint8_t)(1u << key);
    if ((keys_seen & bit) && now - key_at[key] < 65) return;
    keys_seen |= bit;
    key_at[key] = now;
    fruit_queue[(fruit_head + fruit_pending) % FRUIT_COUNT] = key + 1;
    fruit_pending++;
    last_input = now;
}

void pet_mode_adjust_scene(int8_t direction_step, bool change_season) {
    if (!active) return;
    if (change_season) {
        season = (season + 4 + direction_step) % 4;
    } else {
        hour = (hour + 24 + direction_step) % 24;
    }
    redraw_scene = true;
}

static void launch_fruit(uint32_t now) {
    fruit = fruit_queue[fruit_head];
    fruit_head = (fruit_head + 1) % FRUIT_COUNT;
    fruit_pending--;
    const int16_t distance = 69 + (fruit % 4) * 8;
    const int8_t preferred = fruit % 2 ? 1 : -1;
    fruit_x = x + preferred * distance;
    if (fruit_x < 28 || fruit_x > LCD_WIDTH - 28) fruit_x = x - preferred * distance;
    direction = fruit_x > x ? 1 : -1;
    walk_remainder = 0;
    set_action(PET_FALL, now);
}

static int16_t walking_step(uint32_t step_ms, uint8_t speed) {
    walk_remainder += step_ms * speed;
    const int16_t step = (int16_t)(walk_remainder / 1000);
    walk_remainder %= 1000;
    return direction * step;
}

static void update_action(uint32_t now, uint32_t step_ms) {
    const uint32_t elapsed = now - action_since;
    if (!fruit && fruit_pending) {
        launch_fruit(now);
        return;
    }
    switch (action) {
        case PET_SLEEP:
            return;
        case PET_FALL:
            if (elapsed >= FALL_MS) set_action(PET_FETCH, now);
            return;
        case PET_FETCH: {
            const int16_t target = fruit_x - direction * 25;
            x += walking_step(step_ms, 88);
            if ((direction > 0 && x >= target) || (direction < 0 && x <= target)) {
                x = target;
                set_action(PET_EAT, now);
            }
            return;
        }
        case PET_EAT:
            if (elapsed >= (fruit ? FRUIT_EAT_MS : 2100u)) {
                fruit = 0;
                last_input = now;
                set_action(PET_IDLE, now);
            }
            return;
        case PET_IDLE:
        case PET_WALK:
            break;
    }

    if (now - last_input >= SLEEP_AFTER_MS) {
        set_action(PET_SLEEP, now);
    } else if (action == PET_IDLE) {
        if (elapsed >= 1700) set_action(PET_WALK, now);
    } else {
        x += walking_step(step_ms, 37);
        if ((direction < 0 && x <= WALK_MIN) || (direction > 0 && x >= WALK_MAX)) {
            x = direction < 0 ? WALK_MIN : WALK_MAX;
            direction = -direction;
            set_action(PET_IDLE, now);
        } else if (elapsed >= 3600) {
            set_action(PET_IDLE, now);
        }
    }
}

static uint16_t blend(uint16_t from, uint16_t to, uint8_t part, uint8_t total) {
    const uint8_t rest = total - part;
    const uint16_t red = (((from >> 11) * rest + (to >> 11) * part) / total) << 11;
    const uint16_t green = ((((from >> 5) & 63) * rest + ((to >> 5) & 63) * part) / total) << 5;
    const uint16_t blue = ((from & 31) * rest + (to & 31) * part) / total;
    return red | green | blue;
}

static void update_palette(void) {
    const uint8_t start = hour / 3;
    const uint8_t end = (start + 1) % 8;
    const uint8_t night = hour < 6 ? 6 - hour : hour > 18 ? hour - 18 : 0;
    /* The original garden stores its base sky color before the other bands. */
    static const uint8_t sky_entries[] = {2, 3, 1, 4};
    for (uint8_t index = 0; index < 4; index++) {
        scene_palette[sky_entries[index]] = blend(sky_at_hour[start][index],
                                                  sky_at_hour[end][index], hour % 3, 3);
    }
    scene_palette[6] = blend(scene_palette[3], HEX565(0xffeade), 3, 8);
    scene_palette[7] = blend(scene_palette[6], HEX565(0xfffbf1), 2, 8);
    for (uint8_t index = 8; index < 24; index++) {
        scene_palette[index] = blend(garden_at_season[season][index - 8],
                                    HEX565(0x262446), night, 10);
    }
}

static void begin_frame(uint32_t now) {
    const uint32_t elapsed = now - action_since;
    uint8_t sprite = 0;
    uint16_t hop = 0;
    switch (action) {
        case PET_IDLE: sprite = elapsed % 1400 > 1260; break;
        case PET_WALK:
        case PET_FETCH: sprite = 2 + elapsed / (action == PET_FETCH ? 85 : 110) % 4; break;
        case PET_SLEEP: sprite = 6; break;
        case PET_EAT: sprite = 7 + elapsed / 140 % 2; break;
        case PET_FALL:
            if (elapsed < 360) hop = (56u * elapsed * (360u - elapsed)) / (360u * 360u);
            break;
    }
    int16_t left = (x < last_x ? x : last_x) - DIRTY_MARGIN;
    int16_t right = (x > last_x ? x : last_x) + DIRTY_MARGIN;
    int16_t top = DIRTY_TOP;
    const bool visible = fruit && (action == PET_FALL || action == PET_FETCH || elapsed < 320);
    int16_t fruit_y = FRUIT_GROUND;
    if (action == PET_FALL) {
        fruit_y = (int16_t)(((FRUIT_GROUND + FRUIT_RADIUS) * elapsed * (elapsed + FALL_MS)) /
                           (2u * FALL_MS * FALL_MS)) - FRUIT_RADIUS;
    }
    const int16_t fruit_top = fruit_y - FRUIT_RADIUS;
    if (visible && fruit_x - FRUIT_RADIUS < left) left = fruit_x - FRUIT_RADIUS;
    if (visible && fruit_x + FRUIT_RADIUS > right) right = fruit_x + FRUIT_RADIUS;
    if (visible && fruit_top < top) top = fruit_top;
    if (previous_fruit_visible) {
        if (previous_fruit_x - FRUIT_RADIUS < left) left = previous_fruit_x - FRUIT_RADIUS;
        if (previous_fruit_x + FRUIT_RADIUS > right) right = previous_fruit_x + FRUIT_RADIUS;
        if (previous_fruit_top < top) top = previous_fruit_top;
    }
    frame = (PetFrame){
        .left = left > 0 ? left : 0, .right = right < LCD_WIDTH ? right : LCD_WIDTH,
        .row = top > 0 ? top : 0, .bottom = DIRTY_BOTTOM, .x = x, .y = GROUND - hop,
        .direction = direction, .sprite = sprite, .action = action, .elapsed = elapsed,
        .fruit = visible ? fruit : 0, .fruit_x = fruit_x, .fruit_y = fruit_y,
        .hour = hour, .season = season, .bowl = action == PET_EAT && !fruit
    };
    if (redraw_scene) {
        update_palette();
        frame.left = frame.row = 0;
        frame.right = LCD_WIDTH;
        frame.bottom = LCD_HEIGHT;
        redraw_scene = false;
    }
    previous_fruit_visible = visible;
    previous_fruit_x = fruit_x;
    previous_fruit_top = fruit_top;
    last_x = x;
    last_frame = now;
}

static void paint_span(uint8_t *line, int16_t left, int16_t right, uint16_t color) {
    if (left < frame.left) left = frame.left;
    if (right > frame.right) right = frame.right;
    for (int16_t column = left; column < right; column++) {
        uint16_t offset = (column - frame.left) * 2;
        line[offset] = color >> 8;
        line[offset + 1] = color;
    }
}

static void paint_rect(uint8_t *line, uint16_t row, int16_t left, int16_t top,
                       uint8_t width, uint8_t height, uint16_t color) {
    if (row >= top && row < top + height) paint_span(line, left, left + width, color);
}

static void paint_rabbit(uint8_t *line, uint16_t row) {
    const int16_t sprite_y = row - frame.y + RABBIT_HEIGHT;
    if (sprite_y < 0 || sprite_y >= RABBIT_HEIGHT) return;
    const uint16_t row_index = frame.sprite * RABBIT_HEIGHT + sprite_y;
    uint8_t offset = 0;
    for (uint16_t run = rabbit_rows[row_index]; run < rabbit_rows[row_index + 1]; run++) {
        const uint8_t count = rabbit_runs[run * 2];
        const uint8_t color = rabbit_runs[run * 2 + 1];
        if (color) {
            const int16_t left = frame.direction > 0 ? frame.x - RABBIT_WIDTH / 2 + offset :
                                 frame.x + RABBIT_WIDTH / 2 - offset - count;
            paint_span(line, left, left + count, pet_palette[color]);
        }
        offset += count;
    }
}

static void paint_fruit(uint8_t *line, uint16_t row) {
    if (!frame.fruit) return;
    const int16_t local_y = (int16_t)row - frame.fruit_y + FRUIT_RADIUS;
    if (local_y < 0 || local_y >= FRUIT_RADIUS * 2) return;
    const uint8_t fruit_index = frame.fruit - 1;
    const char *shape = fruit_art[fruit_index].rows[local_y / 2];
    for (uint8_t column = 0; column < 9; column++) {
        uint16_t color;
        switch (shape[column]) {
            case 'x': color = HEX565(0x3f2b40); break;
            case 'f': color = fruit_art[fruit_index].body; break;
            case 'h': color = fruit_art[fruit_index].light; break;
            case 'g': color = HEX565(0x76a94e); break;
            default: continue;
        }
        const int16_t left = frame.fruit_x - FRUIT_RADIUS + column * 2;
        paint_span(line, left, left + 2, color);
    }
}

static uint16_t sky_color(uint16_t row) {
    return scene_palette[row < 23 ? 2 : row < 44 ? 3 : row < 66 ? 1 : 4];
}

static void paint_sky(uint8_t *line, uint16_t row) {
    const bool daylight = frame.hour >= 6 && frame.hour <= 18;
    int16_t body_x = 236, body_y = 43;
    uint8_t radius = 11;
    if (daylight) {
        const uint8_t progress = frame.hour - 6;
        body_x = 37 + 20 * progress;
        body_y = 63 - (unsigned)progress * (12 - progress);
        radius = 15;
    }
    const int16_t dy = (int16_t)row - body_y;
    if (dy >= -(int16_t)radius && dy <= radius) {
        int16_t half = radius;
        while (half * half + dy * dy > radius * radius) half--;
        const uint16_t color = daylight ? HEX565(0xffe7b1) : HEX565(0xf4e6d1);
        paint_span(line, body_x - half, body_x + half + 1, color);
        const int16_t cut_y = dy + 3;
        if (!daylight && cut_y >= -10 && cut_y <= 10) {
            int16_t cut_half = 10;
            while (cut_half * cut_half + cut_y * cut_y > 100) cut_half--;
            const int16_t cut_left = body_x + 6 - cut_half;
            const int16_t left = cut_left > body_x - half ? cut_left : body_x - half;
            const int16_t cut_right = body_x + 6 + cut_half;
            const int16_t right = cut_right < body_x + half ? cut_right : body_x + half;
            paint_span(line, left, right + 1, sky_color(row));
        }
    }
    if (!daylight && row >= 28 && row <= 66) {
        static const uint16_t stars[][2] = {{27, 47}, {51, 32}, {85, 49}, {128, 35},
                                           {162, 65}, {184, 46}, {266, 60}, {292, 35}};
        for (uint8_t i = 0; i < sizeof(stars) / sizeof(stars[0]); i++) {
            paint_rect(line, row, stars[i][0], stars[i][1], 2, 2, HEX565(0xedddc8));
        }
    }
}

static void paint_controls(uint8_t *line, uint16_t row) {
    if (row < 8 || row >= 27) return;
    const uint16_t ink = HEX565(0x3b314b);
    paint_rect(line, row, 8, 8, 49, 19, ink);
    paint_rect(line, row, 285, 8, 27, 19, ink);

    if (row >= 12 && row < 22) {
        const uint8_t digit_row = (row - 12) / 2;
        const uint8_t hour_digits[] = {frame.hour / 10, frame.hour % 10, 0, 0};
        const uint8_t lefts[] = {15, 23, 35, 43};
        const uint16_t time_color = HEX565(0xffe8c6);
        for (uint8_t index = 0; index < 4; index++) {
            for (uint8_t column = 0; column < 3; column++) {
                if ((digits[hour_digits[index]] >> (14 - digit_row * 3 - column)) & 1) {
                    const uint8_t left = lefts[index] + 2 * column;
                    paint_span(line, left, left + 2, time_color);
                }
            }
        }
        paint_rect(line, row, 31, 14, 2, 2, time_color);
        paint_rect(line, row, 31, 18, 2, 2, time_color);
    }

    if (row >= 10 && row < 24) {
        static const uint16_t icon_colors[] = {
            HEX565(0xf5aec6), HEX565(0xffdb85), HEX565(0xf7ab6f), HEX565(0xdbf1ff)
        };
        const char *icon = season_icons[frame.season][(row - 10) / 2];
        for (uint8_t column = 0; column < 7; column++) {
            if (icon[column] == 'x') paint_span(line, 292 + column * 2, 294 + column * 2,
                                               icon_colors[frame.season]);
        }
    }
}

static void paint_row(uint8_t *line, uint16_t row) {
    uint16_t x_pos = 0;
    for (uint16_t run = garden_rows[row]; run < garden_rows[row + 1]; run++) {
        const uint16_t end = x_pos + garden_runs[run * 2];
        const uint8_t color = garden_runs[run * 2 + 1];
        paint_span(line, x_pos, end, color == 5 ? sky_color(row) : scene_palette[color]);
        x_pos = end;
    }
    paint_sky(line, row);

    const uint16_t shadow = blend(scene_palette[15], scene_palette[18], 2, 3);
    paint_rect(line, row, frame.x - 23, 138, 46, 3, shadow);
    paint_rect(line, row, frame.x - 18, 141, 36, 1, shadow);
    if (frame.bowl) {
        const int16_t bowl = frame.x + frame.direction * 29 - 6;
        paint_rect(line, row, bowl, 134, 12, 5, RGB565(91, 69, 81));
        paint_rect(line, row, bowl + 1, 135, 10, 3, RGB565(246, 184, 134));
        paint_rect(line, row, bowl + 5, 132, 4, 2, RGB565(104, 68, 75));
    }
    paint_rabbit(line, row);
    paint_fruit(line, row);

    const uint16_t accent = RGB565(255, 243, 201);
    if (frame.action == PET_SLEEP) {
        const int16_t sign_x = frame.x + frame.direction * 25;
        const int16_t top = 102 - (int16_t)(frame.elapsed % 1500 / 140);
        paint_rect(line, row, sign_x, top, 6, 2, accent);
        paint_rect(line, row, sign_x + 2, top + 2, 2, 2, accent);
        paint_rect(line, row, sign_x, top + 4, 6, 2, accent);
    } else if (frame.action == PET_EAT || (frame.action == PET_FALL && frame.elapsed < 360)) {
        const int16_t star_x = frame.x + frame.direction * 23;
        const int16_t top = (frame.action == PET_FALL ? 82 : 103) - frame.elapsed / 90 % 5;
        paint_rect(line, row, star_x + 2, top, 2, 8, accent);
        paint_rect(line, row, star_x - 1, top + 3, 8, 2, accent);
    }
    paint_controls(line, row);
}

static void draw_slice(void) {
    const uint16_t first_row = frame.row;
    const uint16_t width = frame.right - frame.left;
    for (; frame.row < frame.bottom && frame.row - first_row < SLICE_ROWS; frame.row++) {
        paint_row(pixels + (frame.row - first_row) * width * 2, frame.row);
    }
    lcd_show_pic_index(frame.left, first_row, width, frame.row - first_row, pixels, 0);
    if (frame.row == frame.bottom) frames++;
}

void pet_mode_poll(void) {
    const uint32_t now = HAL_GetTick();
    if (encoder_down && !encoder_toggled && !encoder_rotated && now - encoder_since >= ENCODER_HOLD_MS) {
        encoder_toggled = true;
        if (active) {
            active = false;
            restore_menu_color();
        } else {
            start_mode(now);
        }
    }
    if (!active) return;

    const uint32_t step_ms = now - last_motion;
    last_motion = now;
    update_action(now, step_ms > 60 ? 60 : step_ms);
    if (frame.row == frame.bottom && (redraw_scene || now - last_frame >= FRAME_MS)) begin_frame(now);
    if (frame.row < frame.bottom) draw_slice();
}

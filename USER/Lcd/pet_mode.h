#ifndef MULTIPAD_PET_MODE_H
#define MULTIPAD_PET_MODE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PET_IDLE,
    PET_WALK,
    PET_SLEEP,
    PET_EAT,
    PET_FALL,
    PET_FETCH,
    PET_RUN,
    PET_JUMP,
    PET_DAYDREAM,
    PET_FLEE,
    PET_BLINK,
    PET_TRIP_ANGRY,
    PET_GAZE_OWNER, PET_STAND_WATCH, PET_INSPECT_TABLE, PET_BINKY, PET_GROOM_FUR, PET_GROOM_EAR
} PetAction;

typedef enum {
    PET_EVENT_MODE = 1,
    PET_EVENT_ACTION,
    PET_EVENT_FRUIT_QUEUED,
    PET_EVENT_FRUIT_DROPPED,
    PET_EVENT_FRUIT_EATEN,
    PET_EVENT_HOUR,
    PET_EVENT_SEASON,
    PET_EVENT_KEY,
    PET_EVENT_SNACK,
    PET_EVENT_SCENE,
    PET_EVENT_TOY_STARTED,
    PET_EVENT_TOY_FINISHED,
    PET_EVENT_TOY_PLACED,
    PET_EVENT_TOY_REMOVED,
    PET_EVENT_DECORATION_STARTED,
    PET_EVENT_DECORATION_FINISHED,
    PET_EVENT_DECORATION_MOVED,
    PET_EVENT_HEAD_PETTED
} PetEventKind;

typedef struct {
    uint32_t sequence, time_ms;
    uint8_t kind, value, source, detail;
} PetEvent;

enum { PET_EVENT_CAPACITY = 64, PET_EVENTS_PER_REPLY = 16, PET_API_CAPABILITY = 0x50 };

bool pet_mode_is_active(void);
bool pet_mode_encoder_button(bool down);
bool pet_mode_encoder_rotated(bool clockwise, bool pressed_detent);
bool pet_mode_key_down(uint8_t key);
void pet_mode_cycle_background(int8_t direction);
void pet_mode_adjust_scene(int8_t direction, bool change_season);
void pet_mode_poll(void);

/* USB diagnostics report the action, position and completed LCD frame count. */
PetAction pet_mode_action(void);
uint16_t pet_mode_action_age(void);
uint16_t pet_mode_x(void);
uint8_t pet_mode_y(void);
uint32_t pet_mode_frames(void);
uint8_t pet_mode_fruit(void);
uint8_t pet_mode_hour(void);
uint8_t pet_mode_minute(void);
bool pet_mode_is_bedtime(void);
uint8_t pet_mode_pat_phase(void);
uint8_t pet_mode_head_pat_ticks(uint8_t *kind);
uint8_t pet_mode_season(void);
uint8_t pet_mode_queue_depth(void);
uint8_t pet_mode_active_toy(void);
uint8_t pet_mode_toy_phase(void);
enum { PET_FLOOR_CAPACITY = 8 };
typedef struct {
    int16_t x, y;
    uint8_t id, stage;
} PetFloorFood;
uint8_t pet_mode_foods(PetFloorFood *out);
uint8_t pet_mode_toy_drop(void);
uint8_t pet_mode_room_foods(uint8_t scene, PetFloorFood *out);
uint8_t pet_mode_room_drop(uint8_t scene);
uint8_t pet_mode_room_flags(void);
uint8_t pet_mode_control(uint8_t opcode, uint8_t value);
uint8_t pet_mode_move_hutch(uint16_t x, uint16_t y);
uint8_t pet_mode_events(uint32_t after, PetEvent *out, bool *overrun, bool *more);

#endif

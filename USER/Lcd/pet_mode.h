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
    PET_FETCH
} PetAction;

bool pet_mode_is_active(void);
bool pet_mode_encoder_button(bool down);
void pet_mode_encoder_rotated(void);
void pet_mode_key_down(uint8_t key);
void pet_mode_adjust_scene(int8_t direction, bool change_season);
void pet_mode_poll(void);

/* USB diagnostics report the action, position and completed LCD frame count. */
PetAction pet_mode_action(void);
uint16_t pet_mode_x(void);
uint32_t pet_mode_frames(void);
uint8_t pet_mode_fruit(void);
uint8_t pet_mode_hour(void);
uint8_t pet_mode_season(void);

#endif

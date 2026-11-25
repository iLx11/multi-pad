#ifndef MULTIPAD_PET_CARE_H
#define MULTIPAD_PET_CARE_H

#include <stdbool.h>
#include <stdint.h>

enum { PET_GARDEN, PET_NURSERY, PET_SCENE_COUNT };

enum { PET_KEY_COUNT = 8, ITEM_HUTCH = 9, ITEM_CARROT = 10 };
/* The 81 x 74 sprite starts at (anchor_x - 41, anchor_y - 69). */
enum { HUTCH_X_MIN = 41, HUTCH_X_MAX = 280, HUTCH_Y_MIN = 69, HUTCH_Y_MAX = 167 };

typedef struct {
    uint8_t satiety, energy, mood, affection, scene;
    uint8_t bindings[PET_KEY_COUNT];
    bool follow_time;
    uint8_t placed[PET_SCENE_COUNT];
    uint16_t hutch_x;
    uint8_t hutch_y;
} PetCare;

void pet_care_init(uint16_t flash_id);
const PetCare *pet_care_get(void);
void pet_care_set_scene(uint8_t scene);
bool pet_care_set_stat(uint8_t index, uint8_t value);
void pet_care_adjust(int satiety, int energy, int mood, int affection);
void pet_care_eat_fruit(void);
void pet_care_bind(uint8_t key, uint8_t item);
void pet_care_set_toy(uint8_t scene, uint8_t toy);
bool pet_care_move_hutch(uint16_t x, uint16_t y);
void pet_care_follow_clock(bool follow);
void pet_care_tick(uint32_t elapsed_ms, bool sleeping, unsigned exertion);
void pet_care_save(void);

#endif

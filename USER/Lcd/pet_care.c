#include "pet_care.h"
#include "flash.h"
#include <string.h>

/* External SPI flash. Menu 10's last color sector is 10 * 27 + 22 + 26 = 318.
 * The alternate sector retains a complete record during erase or power loss. */
enum { FIRST_SECTOR = 319, SECTOR_BYTES = 4096, V1_BYTES = 16, RECORD_BYTES = 32,
       PER_SECTOR = SECTOR_BYTES / RECORD_BYTES, SLOTS = 2 * PER_SECTOR };
static const uint8_t signature[] = {'P', 'E', 'T'};
static const PetCare initial_care = {
    .satiety = 65, .energy = 75, .mood = 70, .affection = 35,
    .bindings = {1,2,3,4,5,6,7,8}, .hutch_x = 246, .hutch_y = 142
};
static PetCare care;
/* Pending changes can accumulate while a failed page program is retried.
 * A completed meal is also reflected in live stats so it has immediate feedback. */
static struct { uint8_t satiety, energy, mood, affection; } pending_stats;
static uint32_t sequence, metabolism_ms, energy_ms, exercise_ms, retry_at;
static uint16_t next_slot;
static uint32_t bond_ms;
static bool dirty, journal_available, retry_pending, was_sleeping;

static uint16_t crc16(const uint8_t *data, unsigned bytes) {
    uint16_t crc = 0xffff;
    for (unsigned i = 0; i < bytes - 2; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (unsigned bit = 0; bit < 8; bit++)
            crc = (uint16_t)((crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1);
    }
    return crc;
}

static uint32_t address(uint16_t slot) {
    return FIRST_SECTOR * SECTOR_BYTES + slot * RECORD_BYTES;
}

static bool valid_hutch_position(uint16_t x, uint16_t y) {
    return x >= HUTCH_X_MIN && x <= HUTCH_X_MAX && y >= HUTCH_Y_MIN && y <= HUTCH_Y_MAX;
}

static bool valid(const uint8_t *record, unsigned bytes) {
    if (memcmp(record, signature, sizeof(signature)) || record[8] > 100 || record[9] > 100 ||
        record[10] > 100 || record[11] > 100 || record[12] >= PET_SCENE_COUNT) return false;
    if (bytes == RECORD_BYTES) {
        if (record[13] > 1 || record[22] > 2 || record[23] > 2 || record[22] == 1) return false;
        for (unsigned k = 0; k < PET_KEY_COUNT; k++)
            if (record[14+k] > ITEM_CARROT) return false;
        if (record[3] == 3 &&
            (record[23] == 1 || !valid_hutch_position(record[24] | record[25] << 8, record[26]))) return false;
    }
    return crc16(record, bytes) == (record[bytes-2] | record[bytes-1] << 8);
}

void pet_care_init(uint16_t flash_id) {
    care = initial_care;
    sequence = metabolism_ms = energy_ms = exercise_ms = bond_ms = next_slot = 0;
    dirty = retry_pending = was_sleeping = false;
    switch (flash_id) {
        case W25Q16: case W25Q32: case W25Q64: case W25Q128: case W25Q256:
        case BY25Q64: case BY25Q128: case NM25Q64: case NM25Q128:
            journal_available = true;
            break;
        default:
            journal_available = false;
    }
    bool found = false;
    uint8_t group[256];
    for (uint16_t base = 0; journal_available && base < SLOTS; base += sizeof(group) / RECORD_BYTES) {
        read_flash(group, address(base), sizeof(group));
        /* V1 records are 16-byte aligned. Later formats use 32-byte slots. */
        for (unsigned i = 0; i < sizeof(group); i += V1_BYTES) {
            const uint8_t *record = group + i;
            unsigned bytes = record[3] == 1 ? V1_BYTES : RECORD_BYTES;
            if ((record[3] != 1 && ((record[3] != 2 && record[3] != 3) || i % RECORD_BYTES)) ||
                !valid(record, bytes)) continue;
            uint32_t serial;
            memcpy(&serial, record + 4, sizeof(serial));
            if (found && (int32_t)(serial - sequence) <= 0) continue;
            found = true;
            sequence = serial;
            care = initial_care;
            /* V3 reserved byte 27 was zero on older firmware. */
            bond_ms = record[3] == 3 && record[27] < 10 ? record[27] * 60000u : 0;
            care.satiety = record[8];
            care.energy = 100 - record[9]; /* V1–V3 journals encode fatigue. */
            care.mood = record[10];
            care.affection = record[11];
            care.scene = record[12];
            if (record[3] >= 2) {
                care.follow_time = record[13];
                memcpy(care.bindings, record + 14, PET_KEY_COUNT);
                memcpy(care.placed, record + 22, PET_SCENE_COUNT);
                if (care.placed[PET_NURSERY] == 1) care.placed[PET_NURSERY] = 0;
            }
            if (record[3] == 3) {
                care.hutch_x = record[24] | record[25] << 8;
                care.hutch_y = record[26];
            }
            next_slot = (base + i / RECORD_BYTES + 1) % SLOTS;
        }
    }
    pending_stats.satiety = care.satiety;
    pending_stats.energy = care.energy;
    pending_stats.mood = care.mood;
    pending_stats.affection = care.affection;
}

const PetCare *pet_care_get(void) { return &care; }

void pet_care_set_scene(uint8_t scene) {
    if (scene == care.scene || scene >= PET_SCENE_COUNT) return;
    care.scene = scene;
    dirty = true;
    pet_care_save();
}

void pet_care_bind(uint8_t key, uint8_t item) {
    if (key >= PET_KEY_COUNT || item > ITEM_CARROT || care.bindings[key] == item) return;
    care.bindings[key] = item;
    dirty = true;
    pet_care_save();
}

void pet_care_set_toy(uint8_t scene, uint8_t toy) {
    if (scene >= PET_SCENE_COUNT || (toy != 0 && toy != 2) || care.placed[scene] == toy) return;
    care.placed[scene] = toy;
    dirty = true;
    pet_care_save();
}

bool pet_care_move_hutch(uint16_t x, uint16_t y) {
    if (!valid_hutch_position(x, y)) return false;
    if (x != care.hutch_x || y != care.hutch_y) {
        care.hutch_x = x;
        care.hutch_y = (uint8_t)y;
        dirty = true;
        pet_care_save();
    }
    return true;
}

void pet_care_follow_clock(bool follow) {
    if (care.follow_time == follow) return;
    care.follow_time = follow;
    dirty = true;
    pet_care_save();
}

static uint8_t adjusted(uint8_t value, int delta) {
    const int sum = value + delta;
    return sum < 0 ? 0 : sum > 100 ? 100 : sum;
}

static void adjust(uint8_t *value, int delta) {
    const uint8_t result = adjusted(*value, delta);
    if (result != *value) dirty = true;
    *value = result;
}

static int bond_direction(void) {
    if (pending_stats.satiety <= 25 || pending_stats.mood <= 30 || pending_stats.energy <= 20)
        return pending_stats.affection ? -1 : 0;
    if (pending_stats.satiety >= 70 && pending_stats.mood >= 70 && pending_stats.energy >= 70)
        return pending_stats.affection < 100 ? 1 : 0;
    return 0;
}

static void reset_bond_if_changed(int previous) {
    if (previous != bond_direction()) {
        if (bond_ms >= 60000u) dirty = true;
        bond_ms = 0;
    }
}

void pet_care_adjust(int satiety, int energy, int mood, int affection) {
    int previous = bond_direction();
    adjust(&pending_stats.satiety, satiety);
    adjust(&pending_stats.energy, energy);
    adjust(&pending_stats.mood, mood);
    adjust(&pending_stats.affection, affection);
    reset_bond_if_changed(previous);
    pet_care_save();
}

void pet_care_eat_fruit(void) {
    pet_care_adjust(12, 0, 3, 2);
    if (!dirty) return;
    /* The fruit has been consumed. Show its reward while flash retries the
     * pending snapshot. Explicit stat edits still require verified storage. */
    care.satiety = adjusted(care.satiety, 12);
    care.mood = adjusted(care.mood, 3);
    care.affection = adjusted(care.affection, 2);
}

bool pet_care_set_stat(uint8_t index, uint8_t value) {
    uint8_t *fields[] = {&pending_stats.satiety, &pending_stats.energy, &pending_stats.mood, &pending_stats.affection};
    if (!journal_available || index >= sizeof(fields) / sizeof(fields[0]) || value > 100) return false;
    int previous = bond_direction();
    adjust(fields[index], (int)value - *fields[index]);
    reset_bond_if_changed(previous);
    pet_care_save();
    return !dirty;
}

void pet_care_tick(uint32_t elapsed_ms, bool sleeping, unsigned exertion) {
    /* Charge time only while this pet is on screen. Separate fatigue progress
     * avoids attributing a whole minute to a sleep entered on its last tick. */
    if (elapsed_ms > 60000) elapsed_ms = 60000;
    int previous = bond_direction();
    if (previous) {
        unsigned saved_minutes = bond_ms / 60000u;
        bond_ms += elapsed_ms;
        unsigned interval = previous > 0 ? 600000u : 300000u;
        if (bond_ms >= interval) {
            bond_ms -= interval;
            adjust(&pending_stats.affection, previous);
        }
        if (bond_ms / 60000u != saved_minutes) dirty = true;
    }
    if (was_sleeping != sleeping) {
        energy_ms = 0;
        was_sleeping = sleeping;
    }
    const uint32_t energy_interval = sleeping ? 20000 : 60000;
    energy_ms += elapsed_ms;
    while (energy_ms >= energy_interval) {
        energy_ms -= energy_interval;
        adjust(&pending_stats.energy, sleeping ? 1 : -1);
    }
    exercise_ms += elapsed_ms * exertion;
    if (exercise_ms >= 60000u) {
        const int cost = exercise_ms / 60000u;
        exercise_ms %= 60000u;
        adjust(&pending_stats.satiety, -cost);
        adjust(&pending_stats.energy, -cost);
    }
    metabolism_ms += elapsed_ms;
    if (metabolism_ms >= 60000) {
        metabolism_ms -= 60000;
        if (pending_stats.satiety < 25 || pending_stats.energy < 15) adjust(&pending_stats.mood, -1);
        adjust(&pending_stats.satiety, -1);
    }
    reset_bond_if_changed(previous);
    pet_care_save();
}

static bool append_record(void) {
    uint8_t previous[RECORD_BYTES], record[RECORD_BYTES] = {0};
    /* Skip a torn append. Never erase the sector holding the last valid record. */
    while (next_slot % PER_SECTOR) {
        read_flash(previous, address(next_slot), sizeof(previous));
        unsigned i = 0;
        while (i < sizeof(previous) && previous[i] == 0xff) i++;
        if (i == sizeof(previous)) break;
        next_slot = (next_slot + 1) % SLOTS;
    }
    if (!(next_slot % PER_SECTOR)) erase_flash_sector(FIRST_SECTOR + next_slot / PER_SECTOR);
    memcpy(record, signature, sizeof(signature));
    record[3] = 3;
    /* A failed readback can still leave a valid record. Never reuse its serial. */
    uint32_t serial = ++sequence;
    memcpy(record + 4, &serial, sizeof(serial));
    record[8] = pending_stats.satiety;
    record[9] = 100 - pending_stats.energy;
    record[10] = pending_stats.mood;
    record[11] = pending_stats.affection;
    record[12] = care.scene;
    record[13] = care.follow_time;
    memcpy(record + 14, care.bindings, PET_KEY_COUNT);
    memcpy(record + 22, care.placed, PET_SCENE_COUNT);
    record[24] = (uint8_t)care.hutch_x;
    record[25] = (uint8_t)(care.hutch_x >> 8);
    record[26] = care.hutch_y;
    record[27] = bond_ms / 60000u;
    uint16_t crc = crc16(record, sizeof(record));
    record[RECORD_BYTES-2] = (uint8_t)crc;
    record[RECORD_BYTES-1] = (uint8_t)(crc >> 8);
    write_flash_page(record, address(next_slot), sizeof(record));
    read_flash(previous, address(next_slot), sizeof(previous));
    if (memcmp(previous, record, sizeof(record)) != 0) return false;
    next_slot = (next_slot + 1) % SLOTS;
    return true;
}

void pet_care_save(void) {
    if (!dirty) return;
    if (journal_available) {
        if (retry_pending && (int32_t)(HAL_GetTick() - retry_at) < 0) return;
        if (!append_record()) {
            retry_pending = true;
            retry_at = HAL_GetTick() + 1000;
            return;
        }
    }
    retry_pending = dirty = false;
    care.satiety = pending_stats.satiety;
    care.energy = pending_stats.energy;
    care.mood = pending_stats.mood;
    care.affection = pending_stats.affection;
}

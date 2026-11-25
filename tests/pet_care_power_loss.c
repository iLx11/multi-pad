/* Native test: cmake -S firmware/tests -B build-tests, then build and run ctest. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include "flash.h"
#include "pet_care.h"

enum { SECTOR = 4096, FIRST = 319, RECORD = 32 };
static uint8_t flash[2 * SECTOR];
static uint32_t now, programs, erases;
static unsigned program_bytes = RECORD;
static bool corrupt_next_verify;
static uint32_t last_program;
static jmp_buf disconnected;
static int cut_erase_after = -1;

uint32_t HAL_GetTick(void) { return now; }

static size_t offset_of(uint32_t address, uint16_t length) {
    assert(address >= FIRST * SECTOR);
    size_t offset = address - FIRST * SECTOR;
    assert(offset + length <= sizeof(flash));
    return offset;
}

void read_flash(uint8_t *out, uint32_t address, uint16_t length) {
    memcpy(out, flash + offset_of(address, length), length);
    if (corrupt_next_verify && last_program == address && length == RECORD) {
        out[0] ^= 1;
        corrupt_next_verify = false;
    }
}

void write_flash_page(uint8_t *data, uint32_t address, uint16_t length) {
    assert(length == RECORD && address % RECORD == 0);
    uint8_t *dest = flash + offset_of(address, length);
    for (unsigned i = 0; i < program_bytes; ++i) {
        assert(dest[i] == 0xff);
        dest[i] &= data[i];
    }
    last_program = address;
    programs++;
}

void erase_flash_sector(uint32_t sector) {
    assert(sector == FIRST || sector == FIRST + 1);
    uint8_t *start = flash + (sector - FIRST) * SECTOR;
    if (cut_erase_after >= 0) {
        memset(start, 0xff, (unsigned)cut_erase_after);
        longjmp(disconnected, 1);
    }
    memset(start, 0xff, SECTOR);
    erases++;
}

static void new_flash(void) {
    memset(flash, 0xff, sizeof(flash));
    now = programs = erases = last_program = 0;
    program_bytes = RECORD;
    corrupt_next_verify = false;
    pet_care_init(W25Q128);
}

static void stats(unsigned satiety, unsigned energy, unsigned mood, unsigned affection) {
    const PetCare *pet = pet_care_get();
    assert(pet->satiety == satiety && pet->energy == energy);
    assert(pet->mood == mood && pet->affection == affection);
}

static void tick(unsigned ms, bool sleeping) {
    now += ms;
    pet_care_tick(ms, sleeping, 0);
}

static void actual_changes_survive_unplug(void) {
    new_flash();
    pet_care_adjust(-45, -62, -13, 22);
    pet_care_init(W25Q128);
    stats(20, 13, 57, 57);
    tick(60000, false);
    assert(programs == 2); /* Fatigue, metabolism and mood share one snapshot. */
    pet_care_init(W25Q128);
    stats(19, 12, 56, 57);
    tick(19999, true);
    assert(programs == 2);
    tick(1, true);
    assert(programs == 3);
    now += 24u * 60 * 60 * 1000;
    pet_care_init(W25Q128); /* Pull power while sleeping, no exit-mode save. */
    stats(19, 13, 56, 57);
    tick(1, false);
    stats(19, 13, 56, 57);
    pet_care_adjust(100, 100, 100, 100);
    pet_care_adjust(1, 1, 1, 1);
    tick(20000, true);
    assert(programs == 4); /* Clamped stats and unchanged ticks do not program flash. */
    pet_care_init(W25Q128);
    stats(100, 100, 100, 100);
}

static void long_term_affection(void) {
    new_flash();
    pet_care_adjust(35, 25, 10, 0);
    for (int i = 0; i < 9; ++i) tick(60000, false);
    pet_care_init(W25Q128);
    assert(pet_care_get()->affection == 35);
    tick(60000, false);
    assert(pet_care_get()->affection == 36);
    pet_care_adjust(0, 0, -60, 0); /* Low mood alone is enough. */
    for (int i = 0; i < 4; ++i) tick(60000, false);
    pet_care_adjust(0, 0, 35, 0); /* A neutral interval breaks the streak. */
    pet_care_adjust(0, 0, -35, 0);
    tick(60000, false);
    assert(pet_care_get()->affection == 36);
    pet_care_init(W25Q128);
    for (int i = 0; i < 4; ++i) tick(60000, false);
    assert(pet_care_get()->affection == 35);
    pet_care_adjust(0, -100, 70, 0); /* A tired rabbit loses affection even when happy. */
    for (int i = 0; i < 5; ++i) tick(60000, false);
    assert(pet_care_get()->affection == 34);
    pet_care_adjust(-100, 100, 0, -100);
    for (int i = 0; i < 5; ++i) tick(60000, true);
    assert(pet_care_get()->affection == 0);
}

static void failed_writes_recover(void) {
    new_flash();
    pet_care_adjust(1, -1, 1, 1);
    /* Interrupt each possible prefix of the append, including its checksum. */
    for (program_bytes = 0; program_bytes < RECORD; ++program_bytes) {
        pet_care_adjust(6, -3, 7, 4);
        stats(66, 74, 71, 36);
        pet_care_init(W25Q128);
        stats(66, 74, 71, 36);
    }
    program_bytes = 12;
    pet_care_adjust(6, -3, 7, 4);
    stats(66, 74, 71, 36);
    uint32_t attempts = programs;
    tick(999, true);
    assert(programs == attempts);
    program_bytes = RECORD;
    tick(1, true);
    stats(72, 71, 78, 40);
    pet_care_init(W25Q128);
    stats(72, 71, 78, 40);

    corrupt_next_verify = true;
    pet_care_adjust(1, -1, 1, 1); /* Program succeeded but verification failed. */
    stats(72, 71, 78, 40);
    pet_care_adjust(1, -1, 1, 1);
    now += 1000;
    pet_care_save(); /* Main-loop retry, including in work mode. */
    pet_care_init(W25Q128);
    stats(74, 69, 80, 42);
}

static void fruit_reward_is_visible_during_flash_retry(void) {
    new_flash();
    assert(pet_care_set_stat(0, 0));
    program_bytes = 12;
    pet_care_eat_fruit();
    stats(12, 75, 73, 37);
    pet_care_eat_fruit(); /* A second meal during the retry window still counts once. */
    stats(24, 75, 76, 39);
    program_bytes = RECORD;
    tick(1000, false);
    pet_care_init(W25Q128);
    stats(24, 75, 76, 39);
}

static void sector_rotation_survives_unplug(void) {
    new_flash();
    for (unsigned batch = 0; batch < 3; batch++) {
        for (unsigned i = 0; i < SECTOR / RECORD; i++)
            pet_care_adjust(i % 2 ? -1 : 1, 0, 0, 0);
        assert(erases == 1 + 2 * batch);
        program_bytes = 8;
        pet_care_adjust(1, -1, 1, 1); /* Power loss during first record of next sector. */
        pet_care_init(W25Q128);
        stats(65, 75, 70, 35);
        program_bytes = RECORD;
    }
    /* The rollover target still contains old valid records. An interrupted erase
       must not take the latest snapshot, which lives in the opposite sector. */
    for (cut_erase_after = 0; cut_erase_after < SECTOR; cut_erase_after += 511) {
        if (!setjmp(disconnected)) {
            pet_care_adjust(0, 0, 0, 2);
            assert(0 && "expected interruption during erase");
        }
        pet_care_init(W25Q128);
        stats(65, 75, 70, 35);
    }
    cut_erase_after = -1;
    pet_care_adjust(0, 0, 0, 2);
    pet_care_init(W25Q128);
    stats(65, 75, 70, 37);
}

static void exercise_charges_both_stats(void) {
    new_flash();
    pet_care_tick(20000, false, 1);
    pet_care_tick(19999, false, 2);
    stats(65, 75, 70, 35);
    pet_care_tick(1, false, 2);
    stats(64, 74, 70, 35);
    assert(flash[9] == 26); /* Old V3 readers still see the corresponding fatigue. */
    pet_care_init(W25Q128);
    stats(64, 74, 70, 35);
    pet_care_tick(20000, true, 0);
    stats(64, 75, 70, 35);
}

int main(void) {
    exercise_charges_both_stats();
    actual_changes_survive_unplug();
    long_term_affection();
    new_flash();
    for (unsigned i = 0; i < 4; ++i) assert(pet_care_set_stat(i, i == 3 ? 100 : i));
    assert(!pet_care_set_stat(4, 20) && !pet_care_set_stat(0, 101));
    pet_care_init(W25Q128);
    stats(0, 1, 2, 100);
    failed_writes_recover();
    fruit_reward_is_visible_during_flash_retry();
    sector_rotation_survives_unplug();
    puts("Pet care power-loss tests passed");
    return 0;
}

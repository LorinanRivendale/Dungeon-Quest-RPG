#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

// Random number generation
void random_seed(uint32_t seed);
uint8_t random_range(uint8_t min, uint8_t max);
bool random_chance(uint8_t percentage);

// String utilities
void safe_string_copy(char* dest, const char* src, uint8_t max_length);
void clear_screen(void);

// Input handling (PC testing)
typedef enum {
    INPUT_UP = 0,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_A,
    INPUT_B,
    INPUT_START,
    INPUT_SELECT,
    INPUT_NONE
} InputButton;

InputButton input_get_key(void);
void input_wait_for_key(void);

// Display utilities (text-based for PC)
void display_text(const char* text);
void display_menu(const char* title, const char** options, uint8_t option_count, uint8_t selected);
void display_party_status(void);
void display_battle_scene(void);
void display_dungeon(void);

#endif // UTILS_H
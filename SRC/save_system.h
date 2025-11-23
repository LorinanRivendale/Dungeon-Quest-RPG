#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include "game_state.h"
#include "party.h"
#include "inventory.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_SAVE_SLOTS 2
#define SAVE_FILE_PREFIX "dqrpg_save_"
#define SUSPEND_SAVE_FILE "dqrpg_suspend.sav"
#define SAVE_MAGIC 0x44515250  // "DQRP" magic number for validation

// Save data structure - mirrors game state but with fixed sizes for SRAM compatibility
typedef struct {
    uint32_t magic;           // Magic number for validation
    uint32_t version;         // Save format version
    uint32_t checksum;        // Simple checksum for corruption detection

    // Game state
    GameState current_state;
    uint8_t current_dungeon_index;
    bool dungeon_initialized[MAX_DUNGEONS + 1];
    bool dungeons_completed[MAX_DUNGEONS];
    bool final_dungeon_unlocked;
    uint8_t key_items_collected;
    uint16_t gold;
    uint32_t game_time;

    // Dungeons - only save minimal info, will regenerate from fixed maps
    struct {
        uint8_t current_floor;
        uint8_t player_x;
        uint8_t player_y;
        uint8_t encounter_steps;
        bool boss_defeated;
        bool completed;
        // Explored tiles (bitfield - 256 bits for 16x16 map)
        uint8_t explored_tiles[DUNGEON_HEIGHT * DUNGEON_WIDTH / 8];
        // Treasure collected (bitfield - 256 bits for 16x16 map)
        uint8_t treasure_collected[DUNGEON_HEIGHT * DUNGEON_WIDTH / 8];
    } dungeon_data[MAX_DUNGEONS + 1];

    // Party data
    struct {
        uint8_t member_count;
        struct {
            char name[MAX_NAME_LENGTH];
            JobType job;
            uint8_t level;
            uint32_t experience;

            // Stats
            uint16_t max_hp;
            uint16_t current_hp;
            uint16_t max_mp;
            uint16_t current_mp;
            uint8_t attack;
            uint8_t defense;
            uint8_t intelligence;
            uint8_t agility;
            uint8_t luck;

            // Skills
            uint8_t skill_count;
            uint8_t skills[MAX_SKILLS];

            // Equipment
            uint8_t equipped_items[EQUIP_SLOT_COUNT];

            // Status
            uint8_t status_effects;
        } members[MAX_PARTY_SIZE];
    } party_data;

    // Inventory
    struct {
        uint8_t item_count;
        struct {
            uint8_t item_id;
            uint8_t quantity;
            char name[MAX_NAME_LENGTH];
        } items[MAX_INVENTORY_ITEMS];

        uint8_t equipment_count;
        struct {
            uint8_t equipment_id;
            EquipmentSlot slot;
            uint8_t attack_bonus;
            uint8_t defense_bonus;
            uint8_t intelligence_bonus;
            uint8_t agility_bonus;
            bool is_equipped;
            char name[MAX_NAME_LENGTH];
        } equipment[MAX_EQUIPMENT_SLOTS];
    } inventory_data;

} SaveData;

// Save slot info (for displaying save slot selection)
typedef struct {
    bool exists;
    char preview_text[64];  // e.g., "Floor 2 - Cave of Earth - Lv 5"
    uint32_t game_time;
    uint16_t gold;
    uint8_t party_level;
} SaveSlotInfo;

// Save/Load functions
bool save_game_to_slot(uint8_t slot);
bool load_game_from_slot(uint8_t slot);
bool save_suspend_game(void);
bool load_suspend_game(void);
bool delete_suspend_save(void);

// Utility functions
bool save_slot_exists(uint8_t slot);
bool suspend_save_exists(void);
SaveSlotInfo get_save_slot_info(uint8_t slot);
void display_save_slots(void);

// Internal functions
void save_data_from_game_state(SaveData* save_data);
bool load_data_to_game_state(const SaveData* save_data);
uint32_t calculate_checksum(const SaveData* save_data);

#endif // SAVE_SYSTEM_H

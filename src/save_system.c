#include "save_system.h"
#include "dungeon.h"
#include "dungeon_maps.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVE_VERSION 1

// Helper function to get save file path
static void get_save_file_path(uint8_t slot, char* path, size_t path_size) {
    snprintf(path, path_size, "%s%d.sav", SAVE_FILE_PREFIX, slot);
}

// Calculate simple checksum
uint32_t calculate_checksum(const SaveData* save_data) {
    if (!save_data) return 0;

    uint32_t checksum = 0;
    const uint8_t* data = (const uint8_t*)save_data;
    size_t offset = sizeof(save_data->magic) + sizeof(save_data->version) + sizeof(save_data->checksum);

    for (size_t i = offset; i < sizeof(SaveData); i++) {
        checksum += data[i];
    }

    return checksum;
}

// Convert game state to save data
void save_data_from_game_state(SaveData* save_data) {
    if (!save_data) return;

    memset(save_data, 0, sizeof(SaveData));

    // Header
    save_data->magic = SAVE_MAGIC;
    save_data->version = SAVE_VERSION;

    // Game state
    save_data->current_state = g_game_state.current_state;
    save_data->current_dungeon_index = g_game_state.current_dungeon_index;
    save_data->final_dungeon_unlocked = g_game_state.final_dungeon_unlocked;
    save_data->key_items_collected = g_game_state.key_items_collected;
    save_data->gold = g_game_state.gold;
    save_data->game_time = g_game_state.game_time;

    memcpy(save_data->dungeon_initialized, g_game_state.dungeon_initialized, sizeof(g_game_state.dungeon_initialized));
    memcpy(save_data->dungeons_completed, g_game_state.dungeons_completed, sizeof(g_game_state.dungeons_completed));

    // Dungeon data
    for (int i = 0; i <= MAX_DUNGEONS; i++) {
        if (g_game_state.dungeon_initialized[i]) {
            Dungeon* dungeon = &g_game_state.dungeons[i];
            save_data->dungeon_data[i].current_floor = dungeon->current_floor;
            save_data->dungeon_data[i].boss_defeated = dungeon->boss_defeated;
            save_data->dungeon_data[i].completed = dungeon->completed;

            // Save current floor player position and encounter steps
            DungeonFloor* floor = &dungeon->floors[dungeon->current_floor];
            save_data->dungeon_data[i].player_x = floor->player_x;
            save_data->dungeon_data[i].player_y = floor->player_y;
            save_data->dungeon_data[i].encounter_steps = floor->encounter_steps;

            // Save explored tiles and collected treasures as bitfields
            for (int f = 0; f < dungeon->floor_count; f++) {
                DungeonFloor* df = &dungeon->floors[f];
                for (int y = 0; y < DUNGEON_HEIGHT; y++) {
                    for (int x = 0; x < DUNGEON_WIDTH; x++) {
                        int tile_index = y * DUNGEON_WIDTH + x;
                        int byte_index = tile_index / 8;
                        int bit_index = tile_index % 8;

                        if (df->tiles[y][x].explored) {
                            save_data->dungeon_data[i].explored_tiles[byte_index] |= (1 << bit_index);
                        }

                        // Track if treasure was collected (changed from TREASURE to FLOOR)
                        if (df->tiles[y][x].type == TILE_FLOOR) {
                            // Check if this was originally a treasure tile by checking the fixed map
                            const char (*fixed_map)[DUNGEON_WIDTH + 1] = dungeon_get_fixed_map(dungeon->dungeon_id, f);
                            if (fixed_map && fixed_map[y][x] == 'T') {
                                save_data->dungeon_data[i].treasure_collected[byte_index] |= (1 << bit_index);
                            }
                        }
                    }
                }
            }
        }
    }

    // Party data
    if (g_game_state.party) {
        save_data->party_data.member_count = g_game_state.party->member_count;

        for (int i = 0; i < g_game_state.party->member_count; i++) {
            PartyMember* member = &g_game_state.party->members[i];
            safe_string_copy(save_data->party_data.members[i].name, member->name, MAX_NAME_LENGTH);
            save_data->party_data.members[i].job = member->job;
            save_data->party_data.members[i].level = member->stats.level;
            save_data->party_data.members[i].experience = member->stats.experience;

            // Stats
            save_data->party_data.members[i].max_hp = member->stats.max_hp;
            save_data->party_data.members[i].current_hp = member->stats.current_hp;
            save_data->party_data.members[i].max_mp = member->stats.max_mp;
            save_data->party_data.members[i].current_mp = member->stats.current_mp;
            save_data->party_data.members[i].attack = member->stats.strength;
            save_data->party_data.members[i].defense = member->stats.defense;
            save_data->party_data.members[i].intelligence = member->stats.intelligence;
            save_data->party_data.members[i].agility = member->stats.agility;
            save_data->party_data.members[i].luck = member->stats.luck;

            // Skills
            save_data->party_data.members[i].skill_count = member->skill_count;
            memcpy(save_data->party_data.members[i].skills, member->skills, sizeof(member->skills));

            // Equipment
            memcpy(save_data->party_data.members[i].equipped_items, member->equipped_items, sizeof(member->equipped_items));

            // Status effects
            save_data->party_data.members[i].status_effects = member->status_effects;
        }
    }

    // Inventory data
    if (g_game_state.inventory) {
        save_data->inventory_data.item_count = g_game_state.inventory->item_count;

        for (int i = 0; i < g_game_state.inventory->item_count; i++) {
            Item* item = &g_game_state.inventory->items[i];
            save_data->inventory_data.items[i].item_id = item->item_id;
            save_data->inventory_data.items[i].quantity = item->quantity;
            safe_string_copy(save_data->inventory_data.items[i].name, item->name, MAX_NAME_LENGTH);
        }

        save_data->inventory_data.equipment_count = g_game_state.inventory->equipment_count;

        for (int i = 0; i < g_game_state.inventory->equipment_count; i++) {
            Item* equip = &g_game_state.inventory->equipment[i];
            save_data->inventory_data.equipment[i].equipment_id = equip->item_id;
            save_data->inventory_data.equipment[i].slot = equip->equip_type;
            save_data->inventory_data.equipment[i].attack_bonus = equip->attack_bonus;
            save_data->inventory_data.equipment[i].defense_bonus = equip->defense_bonus;
            save_data->inventory_data.equipment[i].intelligence_bonus = equip->intelligence_bonus;
            save_data->inventory_data.equipment[i].agility_bonus = equip->agility_bonus;
            save_data->inventory_data.equipment[i].is_equipped = equip->is_equipped;
            safe_string_copy(save_data->inventory_data.equipment[i].name, equip->name, MAX_NAME_LENGTH);
        }
    }

    // Calculate checksum last
    save_data->checksum = calculate_checksum(save_data);
}

// Load save data into game state
bool load_data_to_game_state(const SaveData* save_data) {
    if (!save_data) return false;

    // Validate magic and version
    if (save_data->magic != SAVE_MAGIC) {
        printf("Error: Invalid save file (bad magic number)\n");
        return false;
    }

    if (save_data->version != SAVE_VERSION) {
        printf("Error: Save file version mismatch\n");
        return false;
    }

    // Validate checksum
    uint32_t expected_checksum = calculate_checksum(save_data);
    if (save_data->checksum != expected_checksum) {
        printf("Error: Save file corrupted (checksum mismatch)\n");
        return false;
    }

    // Clean up existing state
    game_state_cleanup();
    game_state_init();

    // Restore game state
    g_game_state.current_state = save_data->current_state;
    g_game_state.current_dungeon_index = save_data->current_dungeon_index;
    g_game_state.final_dungeon_unlocked = save_data->final_dungeon_unlocked;
    g_game_state.key_items_collected = save_data->key_items_collected;
    g_game_state.gold = save_data->gold;
    g_game_state.game_time = save_data->game_time;

    memcpy(g_game_state.dungeon_initialized, save_data->dungeon_initialized, sizeof(save_data->dungeon_initialized));
    memcpy(g_game_state.dungeons_completed, save_data->dungeons_completed, sizeof(save_data->dungeons_completed));

    // Restore dungeons
    for (int i = 0; i <= MAX_DUNGEONS; i++) {
        if (save_data->dungeon_initialized[i]) {
            // Re-initialize dungeon (regenerates from fixed maps)
            uint8_t floor_count = (i == MAX_DUNGEONS) ? 5 : 3;
            extern const char* dungeon_names[];
            dungeon_init(&g_game_state.dungeons[i], i, dungeon_names[i], floor_count);

            Dungeon* dungeon = &g_game_state.dungeons[i];
            dungeon->current_floor = save_data->dungeon_data[i].current_floor;
            dungeon->boss_defeated = save_data->dungeon_data[i].boss_defeated;
            dungeon->completed = save_data->dungeon_data[i].completed;

            // Restore current floor player position and encounter steps
            DungeonFloor* floor = &dungeon->floors[dungeon->current_floor];
            floor->player_x = save_data->dungeon_data[i].player_x;
            floor->player_y = save_data->dungeon_data[i].player_y;
            floor->encounter_steps = save_data->dungeon_data[i].encounter_steps;

            // Restore explored tiles and collected treasures
            for (int f = 0; f < dungeon->floor_count; f++) {
                DungeonFloor* df = &dungeon->floors[f];
                for (int y = 0; y < DUNGEON_HEIGHT; y++) {
                    for (int x = 0; x < DUNGEON_WIDTH; x++) {
                        int tile_index = y * DUNGEON_WIDTH + x;
                        int byte_index = tile_index / 8;
                        int bit_index = tile_index % 8;

                        // Restore explored status
                        if (save_data->dungeon_data[i].explored_tiles[byte_index] & (1 << bit_index)) {
                            df->tiles[y][x].explored = true;
                        }

                        // Restore treasure collected status
                        if (save_data->dungeon_data[i].treasure_collected[byte_index] & (1 << bit_index)) {
                            df->tiles[y][x].type = TILE_FLOOR;  // Treasure was collected
                        }
                    }
                }
            }
        }
    }

    // Restore party
    g_game_state.party = party_create();
    if (g_game_state.party) {
        g_game_state.party->member_count = save_data->party_data.member_count;

        for (int i = 0; i < save_data->party_data.member_count; i++) {
            PartyMember* member = &g_game_state.party->members[i];
            safe_string_copy(member->name, save_data->party_data.members[i].name, MAX_NAME_LENGTH);
            member->job = save_data->party_data.members[i].job;
            member->stats.level = save_data->party_data.members[i].level;
            member->stats.experience = save_data->party_data.members[i].experience;

            // Stats
            member->stats.max_hp = save_data->party_data.members[i].max_hp;
            member->stats.current_hp = save_data->party_data.members[i].current_hp;
            member->stats.max_mp = save_data->party_data.members[i].max_mp;
            member->stats.current_mp = save_data->party_data.members[i].current_mp;
            member->stats.strength = save_data->party_data.members[i].attack;
            member->stats.defense = save_data->party_data.members[i].defense;
            member->stats.intelligence = save_data->party_data.members[i].intelligence;
            member->stats.agility = save_data->party_data.members[i].agility;
            member->stats.luck = save_data->party_data.members[i].luck;

            // Skills
            member->skill_count = save_data->party_data.members[i].skill_count;
            memcpy(member->skills, save_data->party_data.members[i].skills, sizeof(member->skills));

            // Equipment
            memcpy(member->equipped_items, save_data->party_data.members[i].equipped_items, sizeof(member->equipped_items));

            // Status effects
            member->status_effects = save_data->party_data.members[i].status_effects;
        }
    }

    // Restore inventory
    g_game_state.inventory = inventory_create();
    if (g_game_state.inventory) {
        g_game_state.inventory->item_count = save_data->inventory_data.item_count;

        for (int i = 0; i < save_data->inventory_data.item_count; i++) {
            Item* item = &g_game_state.inventory->items[i];
            item->item_id = save_data->inventory_data.items[i].item_id;
            item->quantity = save_data->inventory_data.items[i].quantity;
            safe_string_copy(item->name, save_data->inventory_data.items[i].name, MAX_NAME_LENGTH);
        }

        g_game_state.inventory->equipment_count = save_data->inventory_data.equipment_count;

        for (int i = 0; i < save_data->inventory_data.equipment_count; i++) {
            Item* equip = &g_game_state.inventory->equipment[i];
            equip->item_id = save_data->inventory_data.equipment[i].equipment_id;
            equip->equip_type = save_data->inventory_data.equipment[i].slot;
            equip->attack_bonus = save_data->inventory_data.equipment[i].attack_bonus;
            equip->defense_bonus = save_data->inventory_data.equipment[i].defense_bonus;
            equip->intelligence_bonus = save_data->inventory_data.equipment[i].intelligence_bonus;
            equip->agility_bonus = save_data->inventory_data.equipment[i].agility_bonus;
            equip->is_equipped = save_data->inventory_data.equipment[i].is_equipped;
            safe_string_copy(equip->name, save_data->inventory_data.equipment[i].name, MAX_NAME_LENGTH);
        }
    }

    return true;
}

// Save to slot
bool save_game_to_slot(uint8_t slot) {
    if (slot >= MAX_SAVE_SLOTS) {
        printf("Error: Invalid save slot %d\n", slot);
        return false;
    }

    char filepath[64];
    get_save_file_path(slot, filepath, sizeof(filepath));

    SaveData save_data;
    save_data_from_game_state(&save_data);

    FILE* file = fopen(filepath, "wb");
    if (!file) {
        printf("Error: Could not create save file %s\n", filepath);
        return false;
    }

    size_t written = fwrite(&save_data, sizeof(SaveData), 1, file);
    fclose(file);

    if (written != 1) {
        printf("Error: Failed to write save data\n");
        return false;
    }

    printf("Game saved to slot %d\n", slot + 1);
    return true;
}

// Load from slot
bool load_game_from_slot(uint8_t slot) {
    if (slot >= MAX_SAVE_SLOTS) {
        printf("Error: Invalid save slot %d\n", slot);
        return false;
    }

    char filepath[64];
    get_save_file_path(slot, filepath, sizeof(filepath));

    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Error: Save file %s not found\n", filepath);
        return false;
    }

    SaveData save_data;
    size_t read = fread(&save_data, sizeof(SaveData), 1, file);
    fclose(file);

    if (read != 1) {
        printf("Error: Failed to read save data\n");
        return false;
    }

    if (!load_data_to_game_state(&save_data)) {
        return false;
    }

    printf("Game loaded from slot %d\n", slot + 1);
    return true;
}

// Suspend save
bool save_suspend_game(void) {
    SaveData save_data;
    save_data_from_game_state(&save_data);

    FILE* file = fopen(SUSPEND_SAVE_FILE, "wb");
    if (!file) {
        printf("Error: Could not create suspend save file\n");
        return false;
    }

    size_t written = fwrite(&save_data, sizeof(SaveData), 1, file);
    fclose(file);

    if (written != 1) {
        printf("Error: Failed to write suspend save\n");
        return false;
    }

    printf("Suspend save created\n");
    return true;
}

// Load suspend save
bool load_suspend_game(void) {
    FILE* file = fopen(SUSPEND_SAVE_FILE, "rb");
    if (!file) {
        printf("No suspend save found\n");
        return false;
    }

    SaveData save_data;
    size_t read = fread(&save_data, sizeof(SaveData), 1, file);
    fclose(file);

    if (read != 1) {
        printf("Error: Failed to read suspend save\n");
        return false;
    }

    if (!load_data_to_game_state(&save_data)) {
        return false;
    }

    // Delete suspend save after loading
    delete_suspend_save();

    printf("Suspend save loaded\n");
    return true;
}

// Delete suspend save
bool delete_suspend_save(void) {
    if (remove(SUSPEND_SAVE_FILE) == 0) {
        return true;
    }
    return false;
}

// Check if save slot exists
bool save_slot_exists(uint8_t slot) {
    if (slot >= MAX_SAVE_SLOTS) return false;

    char filepath[64];
    get_save_file_path(slot, filepath, sizeof(filepath));

    FILE* file = fopen(filepath, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Check if suspend save exists
bool suspend_save_exists(void) {
    FILE* file = fopen(SUSPEND_SAVE_FILE, "rb");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Get save slot info
SaveSlotInfo get_save_slot_info(uint8_t slot) {
    SaveSlotInfo info;
    memset(&info, 0, sizeof(SaveSlotInfo));

    if (!save_slot_exists(slot)) {
        info.exists = false;
        snprintf(info.preview_text, sizeof(info.preview_text), "[ Empty ]");
        return info;
    }

    char filepath[64];
    get_save_file_path(slot, filepath, sizeof(filepath));

    FILE* file = fopen(filepath, "rb");
    if (!file) {
        info.exists = false;
        return info;
    }

    SaveData save_data;
    size_t read = fread(&save_data, sizeof(SaveData), 1, file);
    fclose(file);

    if (read != 1 || save_data.magic != SAVE_MAGIC) {
        info.exists = false;
        snprintf(info.preview_text, sizeof(info.preview_text), "[ Corrupted ]");
        return info;
    }

    info.exists = true;
    info.game_time = save_data.game_time;
    info.gold = save_data.gold;
    info.party_level = (save_data.party_data.member_count > 0) ? save_data.party_data.members[0].level : 1;

    // Build preview text
    extern const char* dungeon_names[];
    const char* location = "Town";
    if (save_data.current_state == STATE_DUNGEON_EXPLORE || save_data.current_state == STATE_BATTLE) {
        if (save_data.current_dungeon_index <= MAX_DUNGEONS) {
            location = dungeon_names[save_data.current_dungeon_index];
        }
    }

    snprintf(info.preview_text, sizeof(info.preview_text),
             "Lv%d %s - %dG",
             info.party_level, location, info.gold);

    return info;
}

// Display save slots
void display_save_slots(void) {
    printf("\n=== SAVE SLOTS ===\n");
    for (uint8_t i = 0; i < MAX_SAVE_SLOTS; i++) {
        SaveSlotInfo info = get_save_slot_info(i);
        printf("%d. %s\n", i + 1, info.preview_text);
    }

    if (suspend_save_exists()) {
        printf("S. Suspend Save (available)\n");
    }
}

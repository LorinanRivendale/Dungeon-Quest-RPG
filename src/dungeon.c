#include "dungeon.h"
#include "dungeon_maps.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void dungeon_init(Dungeon* dungeon, uint8_t dungeon_id, const char* name, uint8_t floors) {
    if (!dungeon) return;
    if (floors > MAX_DUNGEON_FLOORS) floors = MAX_DUNGEON_FLOORS;

    memset(dungeon, 0, sizeof(Dungeon));

    safe_string_copy(dungeon->name, name, MAX_DUNGEON_NAME);
    dungeon->dungeon_id = dungeon_id;
    dungeon->floor_count = floors;
    dungeon->current_floor = 0;
    dungeon->completed = false;
    dungeon->boss_defeated = false;

    // Generate all floors
    for (uint8_t i = 0; i < floors; i++) {
        dungeon_generate_floor(dungeon, i);
    }

    // Initialize boss
    dungeon_init_boss(dungeon, dungeon_id);

    printf("Initialized dungeon: %s (%d floors)\n", dungeon->name, floors);
}

void dungeon_generate_floor(Dungeon* dungeon, uint8_t floor_index) {
    if (!dungeon || floor_index >= dungeon->floor_count) return;

    DungeonFloor* floor = &dungeon->floors[floor_index];

    // Load fixed map for this dungeon and floor
    const char (*fixed_map)[DUNGEON_WIDTH + 1] = dungeon_get_fixed_map(dungeon->dungeon_id, floor_index);

    if (fixed_map) {
        // Use fixed map
        dungeon_load_fixed_map(floor, fixed_map, floor_index);
        printf("Loaded fixed map for floor %d\n", floor_index + 1);
    } else {
        // Fallback: generate procedurally (shouldn't happen with our fixed maps)
        printf("Warning: No fixed map found, using procedural generation for floor %d\n", floor_index + 1);

        // Simple generation: outer walls, some inner walls, rooms
        for (int y = 0; y < DUNGEON_HEIGHT; y++) {
            for (int x = 0; x < DUNGEON_WIDTH; x++) {
                DungeonTile* tile = &floor->tiles[y][x];
                tile->explored = false;
                tile->encounter_rate = 30 + (floor_index * 10);

                // Outer walls
                if (x == 0 || x == DUNGEON_WIDTH - 1 || y == 0 || y == DUNGEON_HEIGHT - 1) {
                    tile->type = TILE_WALL;
                }
                // Random inner walls (20% chance - reduced for better navigation)
                else if (random_chance(20)) {
                    tile->type = TILE_WALL;
                }
                // Floor
                else {
                    tile->type = TILE_FLOOR;
                }
            }
        }

        // Place entrance (start position)
        floor->player_x = DUNGEON_WIDTH / 2;
        floor->player_y = DUNGEON_HEIGHT - 2;
        floor->tiles[floor->player_y][floor->player_x].type = TILE_ENTRANCE;
        floor->tiles[floor->player_y][floor->player_x].explored = true;

        // Place stairs down (if not last floor) - ensure on floor tile
        if (floor_index < dungeon->floor_count - 1) {
            for (int attempts = 0; attempts < 100; attempts++) {
                uint8_t stairs_x = random_range(2, DUNGEON_WIDTH - 3);
                uint8_t stairs_y = random_range(2, DUNGEON_HEIGHT - 3);
                if (floor->tiles[stairs_y][stairs_x].type == TILE_FLOOR) {
                    floor->tiles[stairs_y][stairs_x].type = TILE_STAIRS_DOWN;
                    break;
                }
            }
        }

        // Place stairs up (if not first floor) - ensure on floor tile
        if (floor_index > 0) {
            for (int attempts = 0; attempts < 100; attempts++) {
                uint8_t stairs_x = random_range(2, DUNGEON_WIDTH - 3);
                uint8_t stairs_y = random_range(2, DUNGEON_HEIGHT - 3);
                if (floor->tiles[stairs_y][stairs_x].type == TILE_FLOOR) {
                    floor->tiles[stairs_y][stairs_x].type = TILE_STAIRS_UP;
                    break;
                }
            }
        }

        // Place boss room on last floor - ensure on floor tile
        if (floor_index == dungeon->floor_count - 1) {
            for (int attempts = 0; attempts < 100; attempts++) {
                uint8_t boss_x = random_range(2, DUNGEON_WIDTH - 3);
                uint8_t boss_y = random_range(2, DUNGEON_HEIGHT - 3);
                if (floor->tiles[boss_y][boss_x].type == TILE_FLOOR) {
                    floor->tiles[boss_y][boss_x].type = TILE_BOSS_ROOM;
                    break;
                }
            }
        }

        // Initialize encounter counter
        floor->encounter_steps = random_range(15, 30);
    }
}

void dungeon_init_boss(Dungeon* dungeon, uint8_t dungeon_id) {
    if (!dungeon) return;
    
    const char* boss_names[] = {
        "Earth Golem",
        "Leviathan",
        "Ifrit",
        "Garuda",
        "Dark Lord"
    };
    
    KeyItem key_items[] = {
        KEY_ITEM_EARTH_CRYSTAL,
        KEY_ITEM_WATER_CRYSTAL,
        KEY_ITEM_FIRE_CRYSTAL,
        KEY_ITEM_WIND_CRYSTAL,
        KEY_ITEM_NONE
    };
    
    safe_string_copy(dungeon->boss.name, boss_names[dungeon_id], MAX_NAME_LENGTH);

    // Final boss (Dark Lord) is rebalanced to match expected player level (~20)
    if (dungeon_id == 4) {
        dungeon->boss.max_hp = 450;      // Was 600 (reduced ~25%)
        dungeon->boss.attack = 26;       // Was 35 (reduced ~26%)
        dungeon->boss.defense = 18;      // Was 22 (reduced ~18%)
        dungeon->boss.level = 20;        // Was 30 (matches expected player level)
    } else {
        // Regular bosses use formula
        dungeon->boss.max_hp = 200 + (dungeon_id * 100);
        dungeon->boss.attack = 15 + (dungeon_id * 5);
        dungeon->boss.defense = 10 + (dungeon_id * 3);
        dungeon->boss.level = 10 + (dungeon_id * 5);
    }

    dungeon->boss.current_hp = dungeon->boss.max_hp;
    dungeon->boss.key_item_reward = key_items[dungeon_id];
    dungeon->boss.buff_count = 0;
}

bool dungeon_move_player(Dungeon* dungeon, int8_t dx, int8_t dy) {
    if (!dungeon) return false;
    
    DungeonFloor* floor = &dungeon->floors[dungeon->current_floor];
    
    int new_x = floor->player_x + dx;
    int new_y = floor->player_y + dy;
    
    // Check bounds
    if (new_x < 0 || new_x >= DUNGEON_WIDTH || new_y < 0 || new_y >= DUNGEON_HEIGHT) {
        return false;
    }
    
    // Check if tile is walkable
    TileType tile = floor->tiles[new_y][new_x].type;
    if (tile == TILE_WALL) {
        return false;
    }
    
    // Move player
    floor->player_x = new_x;
    floor->player_y = new_y;
    floor->tiles[new_y][new_x].explored = true;
    
    return true;
}

TileType dungeon_get_current_tile(Dungeon* dungeon) {
    if (!dungeon) return TILE_WALL;
    
    DungeonFloor* floor = &dungeon->floors[dungeon->current_floor];
    return floor->tiles[floor->player_y][floor->player_x].type;
}

bool dungeon_change_floor(Dungeon* dungeon, bool going_down) {
    if (!dungeon) return false;
    
    if (going_down && dungeon->current_floor < dungeon->floor_count - 1) {
        dungeon->current_floor++;
        printf("Descending to floor %d...\n", dungeon->current_floor + 1);
        return true;
    } else if (!going_down && dungeon->current_floor > 0) {
        dungeon->current_floor--;
        printf("Ascending to floor %d...\n", dungeon->current_floor + 1);
        return true;
    }
    
    return false;
}

bool dungeon_check_encounter(Dungeon* dungeon) {
    if (!dungeon) return false;

    DungeonFloor* floor = &dungeon->floors[dungeon->current_floor];
    DungeonTile* tile = &floor->tiles[floor->player_y][floor->player_x];

    // Step-counter encounter system (Final Fantasy style)
    // Boss rooms are safe zones - no random encounters until you interact
    if (tile->type == TILE_FLOOR || tile->type == TILE_ENTRANCE) {
        // Decrement encounter counter
        if (floor->encounter_steps > 0) {
            floor->encounter_steps--;
        }

        // Check if encounter triggers
        if (floor->encounter_steps == 0) {
            // Reset counter for next encounter (random 15-30 steps)
            floor->encounter_steps = random_range(15, 30);
            return true;
        }
    }

    return false;
}

bool dungeon_is_completed(Dungeon* dungeon) {
    return dungeon && dungeon->completed;
}

void dungeon_mark_completed(Dungeon* dungeon) {
    if (dungeon) {
        dungeon->completed = true;
        printf("\n*** %s completed! ***\n", dungeon->name);
    }
}
#ifndef DUNGEON_H
#define DUNGEON_H

#include "game_state.h"
#include <stdint.h>
#include <stdbool.h>

// All dungeon types are now defined in game_state.h to avoid circular dependencies

// Dungeon management
void dungeon_init(Dungeon* dungeon, uint8_t dungeon_id, const char* name, uint8_t floors);
void dungeon_generate_floor(Dungeon* dungeon, uint8_t floor_index);
void dungeon_init_boss(Dungeon* dungeon, uint8_t dungeon_id);

// Dungeon exploration
void dungeon_update_camera(DungeonFloor* floor);
bool dungeon_move_player(Dungeon* dungeon, int8_t dx, int8_t dy);
TileType dungeon_get_current_tile(Dungeon* dungeon);
bool dungeon_change_floor(Dungeon* dungeon, bool going_down);
bool dungeon_check_encounter(Dungeon* dungeon);

// Dungeon queries
bool dungeon_is_completed(Dungeon* dungeon);
void dungeon_mark_completed(Dungeon* dungeon);

// Dungeon names
extern const char* dungeon_names[MAX_DUNGEONS + 1];

#endif // DUNGEON_H
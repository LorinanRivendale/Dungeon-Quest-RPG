#ifndef DUNGEON_MAPS_H
#define DUNGEON_MAPS_H

#include "game_state.h"

// Fixed dungeon map definitions
// Map format: ASCII characters for easy editing
//   '#' = Wall
//   '.' = Floor
//   'E' = Entrance (player start)
//   'D' = Stairs Down
//   'U' = Stairs Up
//   'T' = Treasure
//   'B' = Boss Room

// Function to load a fixed map into a dungeon floor
void dungeon_load_fixed_map(DungeonFloor* floor, const char map[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1], uint8_t floor_number);

// Get the fixed map for a specific dungeon and floor
const char (*dungeon_get_fixed_map(uint8_t dungeon_id, uint8_t floor_number))[DUNGEON_WIDTH + 1];

#endif // DUNGEON_MAPS_H

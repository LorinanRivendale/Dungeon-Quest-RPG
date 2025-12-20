#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include <stdint.h>

// World Map Dimensions
#define WORLD_MAP_WIDTH  128
#define WORLD_MAP_HEIGHT 128
#define WORLD_MAP_SIZE   (WORLD_MAP_WIDTH * WORLD_MAP_HEIGHT)

// Tile ID Definitions (based on Final Fantasy NES tileset)
// These correspond to the tileset: "NES - Final Fantasy - Tilesets - Overworld.png"

// Terrain Tiles
#define TILE_GRASS_1        0x00
#define TILE_GRASS_2        0x01
#define TILE_GRASS_3        0x02
#define TILE_GRASS_4        0x03

#define TILE_FOREST_1       0x10
#define TILE_FOREST_2       0x11
#define TILE_FOREST_3       0x12
#define TILE_FOREST_DARK    0x13

#define TILE_WATER_1        0x20
#define TILE_WATER_2        0x21
#define TILE_WATER_DEEP     0x22
#define TILE_WATER_SHORE_N  0x23
#define TILE_WATER_SHORE_S  0x24
#define TILE_WATER_SHORE_E  0x25
#define TILE_WATER_SHORE_W  0x26

#define TILE_MOUNTAIN_1     0x30
#define TILE_MOUNTAIN_2     0x31
#define TILE_MOUNTAIN_PEAK  0x32
#define TILE_MOUNTAIN_CAVE  0x33

#define TILE_DESERT_1       0x40
#define TILE_DESERT_2       0x41
#define TILE_DESERT_DUNE    0x42

#define TILE_BRIDGE_H       0x50
#define TILE_BRIDGE_V       0x51

#define TILE_ROAD_H         0x60
#define TILE_ROAD_V         0x61
#define TILE_ROAD_CROSS     0x62

// Town/Building Tiles (2x2 metasprites)
#define TILE_TOWN_TL        0x70  // Town top-left
#define TILE_TOWN_TR        0x71  // Town top-right
#define TILE_TOWN_BL        0x72  // Town bottom-left
#define TILE_TOWN_BR        0x73  // Town bottom-right

#define TILE_CASTLE_TL      0x74
#define TILE_CASTLE_TR      0x75
#define TILE_CASTLE_BL      0x76
#define TILE_CASTLE_BR      0x77

#define TILE_DUNGEON_TL     0x78  // Dungeon entrance top-left
#define TILE_DUNGEON_TR     0x79
#define TILE_DUNGEON_BL     0x7A
#define TILE_DUNGEON_BR     0x7B

// Special Tiles
#define TILE_SANCTUM_TL     0x7C  // Final Sanctum (4 crystals required)
#define TILE_SANCTUM_TR     0x7D
#define TILE_SANCTUM_BL     0x7E
#define TILE_SANCTUM_BR     0x7F

// World Map Locations (coordinates are top-left of 2x2 structures)
typedef struct {
    uint8_t x;
    uint8_t y;
    char name[32];
} WorldLocation;

// Town Locations (8 total)
#define TOWN_GREENLEAF      0   // Starter town (SE grasslands)
#define TOWN_CORAL_BAY      1   // Port town (S coast)
#define TOWN_EMBERFORGE     2   // Mountain town (W mountains)
#define TOWN_WINDSPIRE      3   // Sky town (N highlands)
#define TOWN_CROSSROADS     4   // Market hub (center)
#define TOWN_SANDFALL       5   // Desert oasis (SW desert)
#define TOWN_ELDERWOOD      6   // Forest village (E forest)
#define TOWN_SCHOLAR        7   // Ruins camp (NE)

// Dungeon Locations (9 total)
#define DUNGEON_CAVE_EARTH      0   // Near Greenleaf
#define DUNGEON_WATER_TEMPLE    1   // Near Coral Bay
#define DUNGEON_VOLCANO_KEEP    2   // Near Emberforge
#define DUNGEON_SKY_TOWER       3   // Near Windspire
#define DUNGEON_FINAL_SANCTUM   4   // Center (locked)
#define DUNGEON_SMUGGLER_CAVE   5   // Coast side dungeon
#define DUNGEON_PYRAMID         6   // Desert side dungeon
#define DUNGEON_FOREST_SHRINE   7   // Forest side dungeon
#define DUNGEON_BONUS           8   // Post-game bonus

#define NUM_TOWNS       8
#define NUM_DUNGEONS    9

// Encounter Zone Definitions
typedef enum {
    ZONE_NONE = 0,
    ZONE_GRASSLAND,     // Low level (Greenleaf area)
    ZONE_FOREST,        // Medium level
    ZONE_DESERT,        // Medium level
    ZONE_MOUNTAIN,      // High level
    ZONE_COAST,         // Medium level
    ZONE_DEEP_WATER,    // No encounters (impassable)
    ZONE_HIGHLAND,      // High level (Windspire area)
    ZONE_SANCTUM_PATH   // Very high level (near Final Sanctum)
} EncounterZone;

// World Map Structure
typedef struct {
    uint8_t tiles[WORLD_MAP_HEIGHT][WORLD_MAP_WIDTH];      // Tile IDs
    uint8_t zones[WORLD_MAP_HEIGHT][WORLD_MAP_WIDTH];      // Encounter zones
    uint8_t collision[WORLD_MAP_HEIGHT][WORLD_MAP_WIDTH];  // Collision flags
    WorldLocation towns[NUM_TOWNS];
    WorldLocation dungeons[NUM_DUNGEONS];
} WorldMap;

// Global world map data
extern WorldMap g_world_map;

// World map functions
void world_map_init(void);
uint8_t world_map_get_tile(uint8_t x, uint8_t y);
uint8_t world_map_get_zone(uint8_t x, uint8_t y);
uint8_t world_map_is_passable(uint8_t x, uint8_t y);
void world_map_set_player_position(uint8_t x, uint8_t y);
WorldLocation* world_map_get_location_at(uint8_t x, uint8_t y);

#endif // WORLD_MAP_H

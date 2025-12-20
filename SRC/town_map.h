#ifndef TOWN_MAP_H
#define TOWN_MAP_H

#include <stdint.h>

// Town Map Dimensions
// Towns are smaller than the world map, typically 32×32 tiles
#define TOWN_MAP_WIDTH  32
#define TOWN_MAP_HEIGHT 32
#define TOWN_MAP_SIZE   (TOWN_MAP_WIDTH * TOWN_MAP_HEIGHT)

// Building/Location Types
typedef enum {
    BUILDING_NONE = 0,
    BUILDING_INN,               // Rest and heal
    BUILDING_ITEM_SHOP,         // Buy/sell items
    BUILDING_EQUIPMENT_SHOP,    // Buy/sell equipment
    BUILDING_HOUSE,             // NPC residence
    BUILDING_ELDER,             // Town elder/quest giver
    BUILDING_BLACKSMITH,        // Upgrade equipment
    BUILDING_TAVERN,            // Info/quests
    BUILDING_CHURCH,            // Save point
    BUILDING_LIBRARY            // Lore/hints
} TownBuildingType;

// Town Map Structure
typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t tiles[TOWN_MAP_HEIGHT][TOWN_MAP_WIDTH];      // Tile IDs
    uint8_t collision[TOWN_MAP_HEIGHT][TOWN_MAP_WIDTH];  // Collision flags (0=walk, 1=blocked)
    uint8_t entrance_x;                                   // Entry point from world map
    uint8_t entrance_y;
} TownMap;

// NPC data structure
typedef struct {
    uint8_t x, y;
    char name[32];
    char dialogue[128];
    uint8_t sprite_id;
} TownNPC;

// Global town map instance
extern TownMap g_current_town;

// Town initialization functions
void greenleaf_init(TownMap* map);
void coral_bay_init(TownMap* map);
void emberforge_init(TownMap* map);
void windspire_init(TownMap* map);

// Town map functions
void town_map_load(const char* town_name);
uint8_t town_map_get_tile(uint8_t x, uint8_t y);
uint8_t town_map_is_passable(uint8_t x, uint8_t y);
TownBuildingType town_map_get_building_at(uint8_t x, uint8_t y);

#endif // TOWN_MAP_H

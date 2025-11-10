#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdint.h>
#include <stdbool.h>

// Game constants
#define MAX_PARTY_SIZE 4
#define MAX_JOB_TYPES 6
#define MAX_DUNGEONS 4
#define FINAL_DUNGEON 4
#define MAX_INVENTORY_ITEMS 20
#define MAX_EQUIPMENT_SLOTS 20
#define MAX_NAME_LENGTH 12
#define MAX_DUNGEON_NAME 20
#define DUNGEON_WIDTH 16
#define DUNGEON_HEIGHT 16
#define MAX_DUNGEON_FLOORS 5

// Job types enumeration
typedef enum {
    JOB_KNIGHT = 0,
    JOB_BLACK_BELT,
    JOB_THIEF,
    JOB_SAGE,
    JOB_PRIEST,
    JOB_MAGE,
    JOB_NONE = 0xFF
} JobType;

// Game states
typedef enum {
    STATE_TITLE,
    STATE_PARTY_SELECT,
    STATE_DUNGEON_SELECT,
    STATE_DUNGEON_EXPLORE,
    STATE_BATTLE,
    STATE_BOSS_BATTLE,
    STATE_INVENTORY,
    STATE_VICTORY,
    STATE_GAME_OVER
} GameState;

// Buff/Debuff system
#define MAX_BUFFS_PER_CHARACTER 4

typedef enum {
    BUFF_NONE = 0,
    BUFF_ATK_UP,
    BUFF_DEF_UP,
    BUFF_INT_UP,
    BUFF_AGI_UP,
    BUFF_ATK_DOWN,
    BUFF_DEF_DOWN,
    BUFF_INT_DOWN,
    BUFF_AGI_DOWN,
    BUFF_DEFEND,      // Temporary high defense (doubled DEF for 1 turn)
    BUFF_COUNTER,     // Will counter the next attack
    BUFF_REGEN_MP     // Regenerate MP each turn
} BuffType;

typedef struct {
    BuffType type;
    int8_t magnitude;  // Percentage boost/reduction (e.g., 50 = +50%, -30 = -30%)
    uint8_t duration;  // Turns remaining (0 = expired)
} ActiveBuff;

// Key items for dungeon completion
typedef enum {
    KEY_ITEM_NONE = 0,
    KEY_ITEM_EARTH_CRYSTAL = 1,
    KEY_ITEM_WATER_CRYSTAL = 2,
    KEY_ITEM_FIRE_CRYSTAL = 4,
    KEY_ITEM_WIND_CRYSTAL = 8
} KeyItem;

// Tile types
typedef enum {
    TILE_FLOOR = 0,
    TILE_WALL,
    TILE_DOOR,
    TILE_STAIRS_UP,
    TILE_STAIRS_DOWN,
    TILE_TREASURE,
    TILE_BOSS_ROOM,
    TILE_ENTRANCE
} TileType;

// Dungeon tile
typedef struct {
    TileType type;
    bool explored;
    uint8_t encounter_rate; // 0-255, higher = more encounters
} DungeonTile;

// Dungeon floor
typedef struct {
    DungeonTile tiles[DUNGEON_HEIGHT][DUNGEON_WIDTH];
    uint8_t player_x;
    uint8_t player_y;
    uint8_t encounter_steps; // Steps until next encounter (step-counter system)
} DungeonFloor;

// Boss data
typedef struct {
    char name[MAX_NAME_LENGTH];
    uint16_t max_hp;
    uint16_t current_hp;
    uint8_t attack;
    uint8_t defense;
    uint8_t level;
    KeyItem key_item_reward;
    ActiveBuff active_buffs[MAX_BUFFS_PER_CHARACTER];
    uint8_t buff_count;
} BossData;

// Dungeon structure
typedef struct {
    char name[MAX_DUNGEON_NAME];
    uint8_t dungeon_id;
    uint8_t floor_count;
    uint8_t current_floor;
    DungeonFloor floors[MAX_DUNGEON_FLOORS];
    BossData boss;
    bool completed;
    bool boss_defeated;
} Dungeon;

// Forward declarations
typedef struct PartyMember PartyMember;
typedef struct Party Party;
typedef struct Inventory Inventory;

// Main game state structure
typedef struct {
    GameState current_state;
    Party* party;
    Inventory* inventory;
    Dungeon dungeons[MAX_DUNGEONS + 1]; // 4 regular + 1 final (static allocation)
    bool dungeon_initialized[MAX_DUNGEONS + 1]; // Track which dungeons are initialized
    uint8_t key_items_collected; // Bitfield for key items
    uint8_t current_dungeon_index;
    bool dungeons_completed[MAX_DUNGEONS];
    bool final_dungeon_unlocked;
    uint32_t game_time; // Frame counter or time tracker
    uint16_t gold;
} GameStateData;

// Global game state
extern GameStateData g_game_state;

// Game state functions
void game_state_init(void);
void game_state_update(void);
void game_state_cleanup(void);
void game_state_change(GameState new_state);
bool is_final_dungeon_unlocked(void);
void collect_key_item(KeyItem item);

#endif // GAME_STATE_H
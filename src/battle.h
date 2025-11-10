#ifndef BATTLE_H
#define BATTLE_H

#include "party.h"
#include "dungeon.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_ENEMIES 4

// Enemy types
typedef enum {
    ENEMY_GOBLIN = 0,
    ENEMY_ORC,
    ENEMY_SKELETON,
    ENEMY_WOLF,
    ENEMY_DRAGON,
    ENEMY_DEMON,
    ENEMY_TYPE_COUNT
} EnemyType;

// Enemy structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    EnemyType type;
    uint16_t max_hp;
    uint16_t current_hp;
    uint8_t attack;
    uint8_t defense;
    uint8_t agility;
    uint8_t level;
    uint16_t exp_reward;
    uint16_t gold_reward;
    bool is_alive;
    ActiveBuff active_buffs[MAX_BUFFS_PER_CHARACTER];
    uint8_t buff_count;
} Enemy;

// Battle action types
typedef enum {
    ACTION_ATTACK = 0,
    ACTION_SKILL,
    ACTION_ITEM,
    ACTION_DEFEND,
    ACTION_FLEE
} BattleActionType;

// Battle action
typedef struct {
    BattleActionType type;
    uint8_t actor_index; // Party member index
    uint8_t target_index; // Enemy or ally index
    uint8_t item_or_skill_id; // For items or skills
} BattleAction;

// Battle state
typedef struct {
    Enemy enemies[MAX_ENEMIES];
    uint8_t enemy_count;
    bool is_boss_battle;
    BossData* boss; // Pointer to boss if boss battle
    uint8_t turn_order[MAX_PARTY_SIZE + MAX_ENEMIES];
    uint8_t turn_count;
    uint8_t current_turn;
    bool battle_won;
    bool battle_fled;
} BattleState;

// Global battle state
extern BattleState g_battle_state;

// Battle management
void battle_init(uint8_t dungeon_level, bool is_boss);
void battle_init_boss(BossData* boss);
void battle_cleanup(void);
void battle_generate_enemies(uint8_t dungeon_level, uint8_t count);
void battle_calculate_turn_order(void);

// Battle actions
void battle_execute_turn(void);
void battle_process_action(BattleAction* action);
uint16_t battle_calculate_damage(uint8_t attacker_atk, uint8_t defender_def, bool is_critical);
bool battle_attempt_flee(void);

// Battle queries
bool battle_is_over(void);
bool battle_is_victory(void);
void battle_distribute_rewards(void);

// Helper functions
uint8_t battle_find_valid_enemy_target(void);

// Enemy data
extern const char* enemy_names[ENEMY_TYPE_COUNT];

#endif // BATTLE_H
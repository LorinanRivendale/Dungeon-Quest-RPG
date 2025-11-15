#ifndef PARTY_H
#define PARTY_H

#include "game_state.h"
#include <stdint.h>
#include <stdbool.h>

#define MAX_PARTY_SIZE 4
#define MAX_SKILLS 8

// Character stats
typedef struct {
    uint16_t max_hp;
    uint16_t current_hp;
    uint16_t max_mp;
    uint16_t current_mp;
    uint8_t strength;
    uint8_t defense;
    uint8_t intelligence;
    uint8_t agility;
    uint8_t luck;
    uint8_t level;
    uint32_t experience;
} CharacterStats;

// Equipment slots
typedef enum {
    EQUIP_WEAPON = 0,
    EQUIP_ARMOR,
    EQUIP_HELMET,
    EQUIP_ACCESSORY,
    EQUIP_SLOT_COUNT
} EquipmentSlot;

// Status effects
typedef enum {
    STATUS_NONE = 0,
    STATUS_POISON = 1,
    STATUS_PARALYSIS = 2,
    STATUS_SLEEP = 4,
    STATUS_CONFUSION = 8,
    STATUS_DEAD = 16
} StatusEffect;

// Skill/Spell types
typedef enum {
    SKILL_TYPE_ATTACK = 0,
    SKILL_TYPE_HEAL,
    SKILL_TYPE_BUFF,
    SKILL_TYPE_DEBUFF
} SkillType;

// Stat used for skill scaling
typedef enum {
    SCALE_STRENGTH = 0,
    SCALE_INTELLIGENCE,
    SCALE_AGILITY,
    SCALE_LUCK
} ScalingStat;

// Skill/Spell structure
typedef struct {
    uint8_t skill_id;
    char name[16];
    SkillType type;
    ScalingStat scaling_stat; // Which stat to use for damage calculation
    uint8_t mp_cost;
    uint8_t power;           // Damage or healing amount (base)
    uint8_t target_enemy;    // 1 = targets enemy, 0 = targets ally
    uint8_t target_all;      // 1 = targets all, 0 = single target
    const char* description;
} Skill;

// Skill IDs
#define SKILL_NONE 0

// Knight skills
#define SKILL_POWER_STRIKE 1
#define SKILL_SHIELD_BASH 2
#define SKILL_TAUNT 3
#define SKILL_GUARD 4

// Black Belt skills
#define SKILL_FOCUS_STRIKE 10
#define SKILL_COUNTER_STANCE 11
#define SKILL_MEDITATION 12

// Thief skills
#define SKILL_STEAL 20
#define SKILL_BACKSTAB 21
#define SKILL_SMOKE_BOMB 22

// Sage spells
#define SKILL_FIRE 30
#define SKILL_FIRA 31
#define SKILL_FIRAGA 32
#define SKILL_CURE 33
#define SKILL_CURA 34
#define SKILL_TRANQUILITY 35

// Priest spells
#define SKILL_HEAL 40
#define SKILL_HEALA 41
#define SKILL_HEALAGA 42
#define SKILL_PROTECT 43
#define SKILL_ESUNA 44
#define SKILL_PRAYER 45

// Mage spells
#define SKILL_BOLT 50
#define SKILL_BOLT2 51
#define SKILL_BOLT3 52
#define SKILL_ICE 53
#define SKILL_ICE2 54
#define SKILL_ICE3 55
#define SKILL_FOCUS 56

// Party member structure
typedef struct PartyMember {
    char name[MAX_NAME_LENGTH];
    JobType job;
    CharacterStats stats;
    uint8_t equipped_items[EQUIP_SLOT_COUNT]; // Indices to inventory equipment
    uint8_t status_effects; // Bitfield
    uint8_t skills[MAX_SKILLS]; // Skill/spell IDs (for later implementation)
    uint8_t skill_count;
    ActiveBuff active_buffs[MAX_BUFFS_PER_CHARACTER];
    uint8_t buff_count;
} PartyMember;

// Party structure
typedef struct Party {
    PartyMember members[MAX_PARTY_SIZE];
    uint8_t member_count;
    uint8_t active_member; // For turn-based actions
} Party;

// Party management functions
Party* party_create(void);  // Returns pointer to static party (GameBoy-compatible - no malloc!)
bool party_add_member(Party* party, JobType job, const char* name);
void party_remove_member(Party* party, uint8_t index);
PartyMember* party_get_member(Party* party, uint8_t index);
bool party_is_full(Party* party);
bool party_is_defeated(Party* party);
void party_heal_all(Party* party);

// Character functions
void character_init_stats(PartyMember* member, JobType job);
void character_take_damage(PartyMember* member, uint16_t damage);
void character_heal(PartyMember* member, uint16_t amount);
void character_add_status(PartyMember* member, StatusEffect status);
void character_remove_status(PartyMember* member, StatusEffect status);
bool character_has_status(PartyMember* member, StatusEffect status);
void character_use_mp(PartyMember* member, uint16_t mp_cost);
uint32_t character_get_exp_for_next_level(uint8_t level);
void character_gain_experience(PartyMember* member, uint32_t exp);

// Job-specific stat tables (base stats at level 1)
extern const CharacterStats job_base_stats[MAX_JOB_TYPES];
extern const char* job_names[MAX_JOB_TYPES];

// Skill/Magic system
const Skill* get_skill_data(uint8_t skill_id);
void character_learn_skill(PartyMember* member, uint8_t skill_id);
bool character_can_use_skill(PartyMember* member, uint8_t skill_id);
void character_init_starting_skills(PartyMember* member);

// Buff/Debuff management
void character_add_buff(PartyMember* member, BuffType type, int8_t magnitude, uint8_t duration);
void character_remove_buff(PartyMember* member, BuffType type);
void character_clear_all_buffs(PartyMember* member);
void character_update_buffs(PartyMember* member);
bool character_has_buff(PartyMember* member, BuffType type);
int16_t character_get_buff_modifier(PartyMember* member, BuffType stat_type);

#endif // PARTY_H
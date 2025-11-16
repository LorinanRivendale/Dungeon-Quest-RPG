#include "party.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char* job_names[MAX_JOB_TYPES] = {
    "Knight",
    "Black Belt",
    "Thief",
    "Sage",
    "Priest",
    "Mage"
};

const CharacterStats job_base_stats[MAX_JOB_TYPES] = {
    // Knight: High HP, high defense, low magic
    {100, 100, 10, 10, 12, 10, 4, 6, 5, 1, 0},
    // Black Belt: Medium HP, high strength, high agility
    {80, 80, 5, 5, 14, 8, 5, 12, 8, 1, 0},
    // Thief: Low HP, high agility, high luck
    {60, 60, 5, 5, 8, 6, 5, 15, 12, 1, 0},
    // Sage: Medium HP, high intelligence, medium MP
    {70, 70, 30, 30, 6, 6, 14, 8, 7, 1, 0},
    // Priest: Medium HP, high MP, medium intelligence
    {75, 75, 35, 35, 7, 7, 12, 7, 6, 1, 0},
    // Mage: Low HP, very high MP, very high intelligence
    {55, 55, 40, 40, 5, 5, 16, 6, 5, 1, 0}
};

// External reference to static party in game_state.c
extern Party g_static_party;

Party* party_create(void) {
    // Initialize and return pointer to static party (no malloc!)
    memset(&g_static_party, 0, sizeof(Party));
    g_static_party.member_count = 0;
    g_static_party.active_member = 0;

    return &g_static_party;
}

bool party_add_member(Party* party, JobType job, const char* name) {
    if (!party || party_is_full(party) || job >= MAX_JOB_TYPES) {
        return false;
    }
    
    PartyMember* member = &party->members[party->member_count];
    
    // Set name
    safe_string_copy(member->name, name, MAX_NAME_LENGTH);
    
    // Set job
    member->job = job;
    
    // Initialize stats based on job
    character_init_stats(member, job);
    
    // Initialize equipment (empty)
    for (int i = 0; i < EQUIP_SLOT_COUNT; i++) {
        member->equipped_items[i] = 0xFF; // 0xFF = no item
    }
    
    // No status effects
    member->status_effects = STATUS_NONE;

    // Initialize starting skills
	member->skill_count = 0;
	character_init_starting_skills(member);

    // Initialize buffs
    member->buff_count = 0;

    party->member_count++;
    
    printf("Added %s the %s to the party!\n", member->name, job_names[job]);
    
    return true;
}

void party_remove_member(Party* party, uint8_t index) {
    if (!party || index >= party->member_count) return;
    
    // Shift members down
    for (uint8_t i = index; i < party->member_count - 1; i++) {
        party->members[i] = party->members[i + 1];
    }
    
    party->member_count--;
}

PartyMember* party_get_member(Party* party, uint8_t index) {
    if (!party || index >= party->member_count) return NULL;
    return &party->members[index];
}

bool party_is_full(Party* party) {
    return party && party->member_count >= MAX_PARTY_SIZE;
}

bool party_is_defeated(Party* party) {
    if (!party || party->member_count == 0) return true;
    
    for (uint8_t i = 0; i < party->member_count; i++) {
        if (party->members[i].stats.current_hp > 0) {
            return false;
        }
    }
    
    return true;
}

void party_heal_all(Party* party) {
    if (!party) return;
    
    for (uint8_t i = 0; i < party->member_count; i++) {
        PartyMember* member = &party->members[i];
        member->stats.current_hp = member->stats.max_hp;
        member->stats.current_mp = member->stats.max_mp;
        member->status_effects = STATUS_NONE;
    }
    
    printf("Party fully healed!\n");
}

void character_init_stats(PartyMember* member, JobType job) {
    if (!member || job >= MAX_JOB_TYPES) return;
    
    member->stats = job_base_stats[job];
}

void character_take_damage(PartyMember* member, uint16_t damage) {
    if (!member) return;
    
    if (damage >= member->stats.current_hp) {
        member->stats.current_hp = 0;
        member->status_effects |= STATUS_DEAD;
        printf("%s has been defeated!\n", member->name);
    } else {
        member->stats.current_hp -= damage;
        printf("%s takes %d damage! (%d HP remaining)\n", 
               member->name, damage, member->stats.current_hp);
    }
}

void character_heal(PartyMember* member, uint16_t amount) {
    if (!member || character_has_status(member, STATUS_DEAD)) return;
    
    member->stats.current_hp += amount;
    if (member->stats.current_hp > member->stats.max_hp) {
        member->stats.current_hp = member->stats.max_hp;
    }
    
    printf("%s recovers %d HP!\n", member->name, amount);
}

void character_add_status(PartyMember* member, StatusEffect status) {
    if (!member) return;
    member->status_effects |= status;
}

void character_remove_status(PartyMember* member, StatusEffect status) {
    if (!member) return;
    member->status_effects &= ~status;
}

bool character_has_status(PartyMember* member, StatusEffect status) {
    if (!member) return false;
    return (member->status_effects & status) != 0;
}

void character_use_mp(PartyMember* member, uint16_t mp_cost) {
    if (!member) return;
    
    if (mp_cost > member->stats.current_mp) {
        member->stats.current_mp = 0;
    } else {
        member->stats.current_mp -= mp_cost;
    }
}

uint32_t character_get_exp_for_next_level(uint8_t level) {
    // Simple exponential curve: 100 * level^2
    return 100 * level * level;
}

void character_gain_experience(PartyMember* member, uint32_t exp) {
    if (!member || character_has_status(member, STATUS_DEAD)) return;
    
    member->stats.experience += exp;
    printf("%s gains %d EXP!\n", member->name, exp);
    
    // Check for level up
    uint32_t exp_needed = character_get_exp_for_next_level(member->stats.level);
    
    while (member->stats.experience >= exp_needed) {
        member->stats.level++;
        
        // Stat growth based on job
        member->stats.max_hp += 8 + (random_range(0, 4));
        member->stats.max_mp += 3 + (random_range(0, 2));
        member->stats.strength += random_range(0, 2);
        member->stats.defense += random_range(0, 2);
        member->stats.intelligence += random_range(0, 2);
        member->stats.agility += random_range(0, 2);
        member->stats.luck += random_range(0, 1);
        
        // Heal on level up
        member->stats.current_hp = member->stats.max_hp;
        member->stats.current_mp = member->stats.max_mp;
        
        printf("*** %s leveled up to level %d! ***\n", member->name, member->stats.level);
        
        exp_needed = character_get_exp_for_next_level(member->stats.level);
    }
	// Inside character_gain_experience, after the level up prints:

	// Learn new skills at certain levels
	switch (member->job) {
		case JOB_KNIGHT:
			if (member->stats.level == 5) character_learn_skill(member, SKILL_TAUNT);
			break;
			
		case JOB_BLACK_BELT:
			if (member->stats.level == 5) character_learn_skill(member, SKILL_COUNTER_STANCE);
			break;
			
		case JOB_THIEF:
			if (member->stats.level == 5) character_learn_skill(member, SKILL_SMOKE_BOMB);
			break;
			
		case JOB_SAGE:
			if (member->stats.level == 5) character_learn_skill(member, SKILL_FIRA);
			if (member->stats.level == 8) character_learn_skill(member, SKILL_CURA);
			if (member->stats.level == 12) character_learn_skill(member, SKILL_FIRAGA);
			break;
			
		case JOB_PRIEST:
			if (member->stats.level == 5) character_learn_skill(member, SKILL_PROTECT);
			if (member->stats.level == 10) character_learn_skill(member, SKILL_HEALAGA);
			break;
			
		case JOB_MAGE:
			if (member->stats.level == 4) character_learn_skill(member, SKILL_ICE2);
			if (member->stats.level == 5) character_learn_skill(member, SKILL_BOLT2);
			if (member->stats.level == 10) character_learn_skill(member, SKILL_ICE3);
			if (member->stats.level == 12) character_learn_skill(member, SKILL_BOLT3);
			break;
					
		default:
			break;
	}
}
// Skill database
// Format: {skill_id, name, type, scaling_stat, mp_cost, power, target_enemy, target_all, description}
static const Skill skill_database[] = {
    // Format: skill_id, name, type, scaling_stat, mp_cost, power, target_enemy, target_all, status_effect, status_chance, status_duration, description

    // Knight skills (scale on STRENGTH)
    {SKILL_POWER_STRIKE, "Power Strike", SKILL_TYPE_ATTACK, SCALE_STRENGTH, 4, 30, 1, 0, 0, 0, 0, "Strong physical attack"},
    {SKILL_SHIELD_BASH, "Shield Bash", SKILL_TYPE_ATTACK, SCALE_STRENGTH, 3, 20, 1, 0, 0, 0, 0, "Bash enemy with shield"},
    {SKILL_TAUNT, "Taunt", SKILL_TYPE_DEBUFF, SCALE_STRENGTH, 2, 0, 1, 0, 0, 0, 0, "Draw enemy attention"},
    {SKILL_GUARD, "Guard", SKILL_TYPE_BUFF, SCALE_STRENGTH, 0, 0, 0, 0, 0, 0, 0, "Double defense for 1 turn"},

    // Black Belt skills (scale on AGILITY)
    {SKILL_FOCUS_STRIKE, "Focus Strike", SKILL_TYPE_ATTACK, SCALE_AGILITY, 5, 35, 1, 0, 0, 0, 0, "Concentrated attack"},
    {SKILL_COUNTER_STANCE, "Counter", SKILL_TYPE_BUFF, SCALE_AGILITY, 4, 0, 0, 0, 0, 0, 0, "Counter next attack"},
    {SKILL_MEDITATION, "Meditation", SKILL_TYPE_BUFF, SCALE_AGILITY, 0, 10, 0, 0, 0, 0, 0, "Restore 10 MP per turn for 3 turns"},

    // Thief skills (scale on LUCK)
    {SKILL_STEAL, "Steal", SKILL_TYPE_STEAL, SCALE_LUCK, 3, 0, 1, 0, 0, 0, 0, "Steal item from enemy"},
    {SKILL_BACKSTAB, "Backstab", SKILL_TYPE_ATTACK, SCALE_LUCK, 6, 40, 1, 0, 0, 0, 0, "Critical strike"},
    {SKILL_SMOKE_BOMB, "Smoke Bomb", SKILL_TYPE_DEBUFF, SCALE_LUCK, 5, 0, 1, 1, 0, 0, 0, "Flee from battle"},
    {SKILL_POISON_BLADE, "Poison Blade", SKILL_TYPE_ATTACK, SCALE_LUCK, 5, 20, 1, 0, STATUS_POISON, 75, 4, "Attack that poisons enemy"},
    {SKILL_FLASH, "Flash", SKILL_TYPE_DEBUFF, SCALE_LUCK, 4, 0, 1, 0, STATUS_BLIND, 80, 3, "Blind enemy to reduce accuracy"},

    // Sage spells (scale on INTELLIGENCE)
    {SKILL_FIRE, "Fire", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 4, 25, 1, 0, 0, 0, 0, "Fire magic attack"},
    {SKILL_FIRA, "Fira", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 8, 45, 1, 0, 0, 0, 0, "Strong fire attack"},
    {SKILL_FIRAGA, "Firaga", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 16, 70, 1, 1, 0, 0, 0, "Massive fire on all"},
    {SKILL_CURE, "Cure", SKILL_TYPE_HEAL, SCALE_INTELLIGENCE, 5, 40, 0, 0, 0, 0, 0, "Restore HP"},
    {SKILL_CURA, "Cura", SKILL_TYPE_HEAL, SCALE_INTELLIGENCE, 10, 80, 0, 0, 0, 0, 0, "Restore more HP"},
    {SKILL_TRANQUILITY, "Tranquility", SKILL_TYPE_BUFF, SCALE_INTELLIGENCE, 0, 8, 0, 0, 0, 0, 0, "Restore 8 MP per turn for 3 turns"},
    {SKILL_TIME_WARP, "Time Warp", SKILL_TYPE_DEBUFF, SCALE_INTELLIGENCE, 8, 0, 1, 0, STATUS_SLOW, 85, 4, "Slow enemy agility"},
    {SKILL_MUTE, "Mute", SKILL_TYPE_DEBUFF, SCALE_INTELLIGENCE, 6, 0, 1, 0, STATUS_SILENCE, 80, 3, "Silence enemy magic"},
    {SKILL_PETRIFY, "Petrify", SKILL_TYPE_DEBUFF, SCALE_INTELLIGENCE, 12, 0, 1, 0, STATUS_STONE, 50, 2, "Turn enemy to stone"},

    // Priest spells (scale on INTELLIGENCE)
    {SKILL_HEAL, "Heal", SKILL_TYPE_HEAL, SCALE_INTELLIGENCE, 4, 50, 0, 0, 0, 0, 0, "Restore HP"},
    {SKILL_HEALA, "Heala", SKILL_TYPE_HEAL, SCALE_INTELLIGENCE, 8, 100, 0, 0, 0, 0, 0, "Restore lots of HP"},
    {SKILL_HEALAGA, "Healaga", SKILL_TYPE_HEAL, SCALE_INTELLIGENCE, 12, 150, 0, 1, 0, 0, 0, "Heal entire party"},
    {SKILL_PROTECT, "Protect", SKILL_TYPE_BUFF, SCALE_INTELLIGENCE, 6, 0, 0, 0, 0, 0, 0, "Increase defense"},
    {SKILL_ESUNA, "Esuna", SKILL_TYPE_HEAL, SCALE_INTELLIGENCE, 5, 0, 0, 0, 0, 0, 0, "Cure status effects"},
    {SKILL_PRAYER, "Prayer", SKILL_TYPE_HEAL, SCALE_INTELLIGENCE, 0, 20, 0, 1, 0, 0, 0, "Small party heal + 5 MP regen for 2 turns"},
    {SKILL_BLINDING_LIGHT, "Blind Light", SKILL_TYPE_DEBUFF, SCALE_INTELLIGENCE, 7, 0, 1, 1, STATUS_BLIND, 70, 3, "Blind all enemies"},

    // Mage spells (scale on INTELLIGENCE)
    {SKILL_BOLT, "Bolt", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 5, 30, 1, 0, 0, 0, 0, "Lightning attack"},
    {SKILL_BOLT2, "Bolt2", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 10, 55, 1, 0, 0, 0, 0, "Strong lightning"},
    {SKILL_BOLT3, "Bolt3", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 18, 70, 1, 1, 0, 0, 0, "Massive lightning"},
    {SKILL_ICE, "Ice", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 5, 30, 1, 0, 0, 0, 0, "Ice attack"},
    {SKILL_ICE2, "Ice2", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 10, 55, 1, 0, 0, 0, 0, "Strong ice attack"},
    {SKILL_ICE3, "Ice3", SKILL_TYPE_ATTACK, SCALE_INTELLIGENCE, 18, 70, 1, 1, 0, 0, 0, "Massive ice attack"},
    {SKILL_FOCUS, "Focus", SKILL_TYPE_BUFF, SCALE_INTELLIGENCE, 0, 12, 0, 0, 0, 0, 0, "Restore 12 MP per turn for 3 turns"},
    {SKILL_SLOW, "Slow", SKILL_TYPE_DEBUFF, SCALE_INTELLIGENCE, 6, 0, 1, 0, STATUS_SLOW, 90, 4, "Slow enemy turn order"},
    {SKILL_SILENCE, "Silence", SKILL_TYPE_DEBUFF, SCALE_INTELLIGENCE, 5, 0, 1, 0, STATUS_SILENCE, 85, 3, "Prevent enemy magic"},
    {SKILL_TOXIC_CLOUD, "Toxic Cloud", SKILL_TYPE_DEBUFF, SCALE_INTELLIGENCE, 10, 0, 1, 1, STATUS_POISON, 60, 5, "Poison all enemies"},
    {SKILL_STONE_GAZE, "Stone Gaze", SKILL_TYPE_DEBUFF, SCALE_INTELLIGENCE, 14, 0, 1, 0, STATUS_STONE, 45, 2, "Petrify enemy"},

    // Terminator
    {SKILL_NONE, "", SKILL_TYPE_ATTACK, SCALE_STRENGTH, 0, 0, 0, 0, 0, 0, 0, ""}
};

const Skill* get_skill_data(uint8_t skill_id) {
    for (int i = 0; skill_database[i].skill_id != SKILL_NONE; i++) {
        if (skill_database[i].skill_id == skill_id) {
            return &skill_database[i];
        }
    }
    return NULL;
}

void character_learn_skill(PartyMember* member, uint8_t skill_id) {
    if (!member || member->skill_count >= MAX_SKILLS) return;
    
    // Check if already known
    for (uint8_t i = 0; i < member->skill_count; i++) {
        if (member->skills[i] == skill_id) {
            return; // Already knows this skill
        }
    }
    
    member->skills[member->skill_count++] = skill_id;
    
    const Skill* skill = get_skill_data(skill_id);
    if (skill) {
        printf("%s learned %s!\n", member->name, skill->name);
    }
}

bool character_can_use_skill(PartyMember* member, uint8_t skill_id) {
    if (!member) return false;
    
    const Skill* skill = get_skill_data(skill_id);
    if (!skill) return false;
    
    // Check if character knows the skill
    bool knows_skill = false;
    for (uint8_t i = 0; i < member->skill_count; i++) {
        if (member->skills[i] == skill_id) {
            knows_skill = true;
            break;
        }
    }
    
    if (!knows_skill) return false;
    
    // Check MP
    if (member->stats.current_mp < skill->mp_cost) return false;
    
    return true;
}

void character_init_starting_skills(PartyMember* member) {
    if (!member) return;

    switch (member->job) {
        case JOB_KNIGHT:
            character_learn_skill(member, SKILL_POWER_STRIKE);
            character_learn_skill(member, SKILL_SHIELD_BASH);
            character_learn_skill(member, SKILL_GUARD);
            break;

        case JOB_BLACK_BELT:
            character_learn_skill(member, SKILL_FOCUS_STRIKE);
            character_learn_skill(member, SKILL_MEDITATION);
            break;

        case JOB_THIEF:
            character_learn_skill(member, SKILL_BACKSTAB);
            character_learn_skill(member, SKILL_STEAL);
            character_learn_skill(member, SKILL_POISON_BLADE);  // Status effect skill
            character_learn_skill(member, SKILL_FLASH);         // Status effect skill
            break;

        case JOB_SAGE:
            character_learn_skill(member, SKILL_FIRE);
            character_learn_skill(member, SKILL_CURE);
            character_learn_skill(member, SKILL_BOLT);
            character_learn_skill(member, SKILL_TRANQUILITY);
            character_learn_skill(member, SKILL_TIME_WARP);     // Status effect skill
            character_learn_skill(member, SKILL_MUTE);          // Status effect skill
            character_learn_skill(member, SKILL_PETRIFY);       // Status effect skill
            break;

        case JOB_PRIEST:
            character_learn_skill(member, SKILL_HEAL);
            character_learn_skill(member, SKILL_HEALA);
            character_learn_skill(member, SKILL_ESUNA);
            character_learn_skill(member, SKILL_PRAYER);
            character_learn_skill(member, SKILL_BLINDING_LIGHT); // Status effect skill
            break;

        case JOB_MAGE:
            character_learn_skill(member, SKILL_FIRE);
            character_learn_skill(member, SKILL_BOLT);
            character_learn_skill(member, SKILL_ICE);
            character_learn_skill(member, SKILL_FOCUS);
            character_learn_skill(member, SKILL_SLOW);          // Status effect skill
            character_learn_skill(member, SKILL_SILENCE);       // Status effect skill
            character_learn_skill(member, SKILL_TOXIC_CLOUD);   // Status effect skill
            character_learn_skill(member, SKILL_STONE_GAZE);    // Status effect skill
            break;

        default:
            break;
    }
}

// Buff/Debuff management functions

void character_add_buff(PartyMember* member, BuffType type, int8_t magnitude, uint8_t duration) {
    if (!member || type == BUFF_NONE || duration == 0) return;

    // Check if buff already exists - if so, refresh duration and magnitude
    for (uint8_t i = 0; i < member->buff_count; i++) {
        if (member->active_buffs[i].type == type) {
            member->active_buffs[i].magnitude = magnitude;
            member->active_buffs[i].duration = duration;
            return;
        }
    }

    // Add new buff if space available
    if (member->buff_count < MAX_BUFFS_PER_CHARACTER) {
        member->active_buffs[member->buff_count].type = type;
        member->active_buffs[member->buff_count].magnitude = magnitude;
        member->active_buffs[member->buff_count].duration = duration;
        member->buff_count++;
    }
}

void character_remove_buff(PartyMember* member, BuffType type) {
    if (!member) return;

    for (uint8_t i = 0; i < member->buff_count; i++) {
        if (member->active_buffs[i].type == type) {
            // Shift remaining buffs down
            for (uint8_t j = i; j < member->buff_count - 1; j++) {
                member->active_buffs[j] = member->active_buffs[j + 1];
            }
            member->buff_count--;
            return;
        }
    }
}

void character_clear_all_buffs(PartyMember* member) {
    if (!member) return;
    member->buff_count = 0;
}

void character_update_buffs(PartyMember* member) {
    if (!member) return;

    // Decrement duration and remove expired buffs
    for (uint8_t i = 0; i < member->buff_count; ) {
        if (member->active_buffs[i].duration > 0) {
            member->active_buffs[i].duration--;
        }

        if (member->active_buffs[i].duration == 0) {
            // Remove expired buff
            for (uint8_t j = i; j < member->buff_count - 1; j++) {
                member->active_buffs[j] = member->active_buffs[j + 1];
            }
            member->buff_count--;
            // Don't increment i - check the same index again
        } else {
            // Apply MP regen if buff exists
            if (member->active_buffs[i].type == BUFF_REGEN_MP) {
                uint16_t regen_amount = member->active_buffs[i].magnitude;
                if (member->stats.current_mp + regen_amount > member->stats.max_mp) {
                    member->stats.current_mp = member->stats.max_mp;
                } else {
                    member->stats.current_mp += regen_amount;
                }
            }
            i++;
        }
    }
}

bool character_has_buff(PartyMember* member, BuffType type) {
    if (!member) return false;

    for (uint8_t i = 0; i < member->buff_count; i++) {
        if (member->active_buffs[i].type == type) {
            return true;
        }
    }
    return false;
}

int16_t character_get_buff_modifier(PartyMember* member, BuffType stat_type) {
    if (!member) return 0;

    int16_t total_modifier = 0;

    for (uint8_t i = 0; i < member->buff_count; i++) {
        if (member->active_buffs[i].type == stat_type) {
            total_modifier += member->active_buffs[i].magnitude;
        }
    }

    return total_modifier;
}
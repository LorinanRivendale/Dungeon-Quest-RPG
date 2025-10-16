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

Party* party_create(void) {
    Party* party = (Party*)malloc(sizeof(Party));
    if (!party) return NULL;
    
    memset(party, 0, sizeof(Party));
    party->member_count = 0;
    party->active_member = 0;
    
    return party;
}

void party_destroy(Party* party) {
    if (party) {
        free(party);
    }
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
static const Skill skill_database[] = {
    // Knight skills
    {SKILL_POWER_STRIKE, "Power Strike", SKILL_TYPE_ATTACK, 4, 30, 1, 0, "Strong physical attack"},
    {SKILL_SHIELD_BASH, "Shield Bash", SKILL_TYPE_ATTACK, 3, 20, 1, 0, "Bash enemy with shield"},
    {SKILL_TAUNT, "Taunt", SKILL_TYPE_DEBUFF, 2, 0, 1, 0, "Draw enemy attention"},
    
    // Black Belt skills
    {SKILL_FOCUS_STRIKE, "Focus Strike", SKILL_TYPE_ATTACK, 5, 35, 1, 0, "Concentrated attack"},
    {SKILL_COUNTER_STANCE, "Counter", SKILL_TYPE_BUFF, 4, 0, 0, 0, "Counter next attack"},
    {SKILL_MEDITATION, "Meditation", SKILL_TYPE_HEAL, 0, 20, 0, 0, "Restore HP"},
    
    // Thief skills
    {SKILL_STEAL, "Steal", SKILL_TYPE_ATTACK, 3, 0, 1, 0, "Steal item from enemy"},
    {SKILL_BACKSTAB, "Backstab", SKILL_TYPE_ATTACK, 6, 40, 1, 0, "Critical strike"},
    {SKILL_SMOKE_BOMB, "Smoke Bomb", SKILL_TYPE_DEBUFF, 5, 0, 1, 1, "Flee from battle"},
    
    // Sage spells (balanced offense and healing)
    {SKILL_FIRE, "Fire", SKILL_TYPE_ATTACK, 4, 25, 1, 0, "Fire magic attack"},
    {SKILL_FIRA, "Fira", SKILL_TYPE_ATTACK, 8, 45, 1, 0, "Strong fire attack"},
    {SKILL_FIRAGA, "Firaga", SKILL_TYPE_ATTACK, 16, 80, 1, 1, "Massive fire on all"},
    {SKILL_CURE, "Cure", SKILL_TYPE_HEAL, 5, 40, 0, 0, "Restore HP"},
    {SKILL_CURA, "Cura", SKILL_TYPE_HEAL, 10, 80, 0, 0, "Restore more HP"},
    
    // Priest spells (focus on healing and support)
    {SKILL_HEAL, "Heal", SKILL_TYPE_HEAL, 4, 50, 0, 0, "Restore HP"},
    {SKILL_HEALA, "Heala", SKILL_TYPE_HEAL, 8, 100, 0, 0, "Restore lots of HP"},
    {SKILL_HEALAGA, "Healaga", SKILL_TYPE_HEAL, 12, 150, 0, 1, "Heal entire party"},
    {SKILL_PROTECT, "Protect", SKILL_TYPE_BUFF, 6, 0, 0, 0, "Increase defense"},
    {SKILL_ESUNA, "Esuna", SKILL_TYPE_HEAL, 5, 0, 0, 0, "Cure status effects"},
    
    // Mage spells (focus on damage)
    {SKILL_BOLT, "Bolt", SKILL_TYPE_ATTACK, 5, 30, 1, 0, "Lightning attack"},
    {SKILL_BOLT2, "Bolt2", SKILL_TYPE_ATTACK, 10, 55, 1, 0, "Strong lightning"},
    {SKILL_BOLT3, "Bolt3", SKILL_TYPE_ATTACK, 18, 95, 1, 1, "Massive lightning"},
    {SKILL_ICE, "Ice", SKILL_TYPE_ATTACK, 5, 30, 1, 0, "Ice attack"},
    {SKILL_ICE2, "Ice2", SKILL_TYPE_ATTACK, 10, 55, 1, 0, "Strong ice attack"},
    {SKILL_ICE3, "Ice3", SKILL_TYPE_ATTACK, 18, 95, 1, 1, "Massive ice attack"},
    
    // Terminator
    {SKILL_NONE, "", SKILL_TYPE_ATTACK, 0, 0, 0, 0, ""}
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
            break;
            
        case JOB_BLACK_BELT:
            character_learn_skill(member, SKILL_FOCUS_STRIKE);
            character_learn_skill(member, SKILL_MEDITATION);
            break;
            
        case JOB_THIEF:
            character_learn_skill(member, SKILL_BACKSTAB);
            character_learn_skill(member, SKILL_STEAL);
            break;
            
        case JOB_SAGE:
            character_learn_skill(member, SKILL_FIRE);
            character_learn_skill(member, SKILL_CURE);
            character_learn_skill(member, SKILL_BOLT);
            break;
            
        case JOB_PRIEST:
            character_learn_skill(member, SKILL_HEAL);
            character_learn_skill(member, SKILL_HEALA);
            character_learn_skill(member, SKILL_ESUNA);
            break;
            
        case JOB_MAGE:
            character_learn_skill(member, SKILL_FIRE);
            character_learn_skill(member, SKILL_BOLT);
            character_learn_skill(member, SKILL_ICE);
            break;
            
        default:
            break;
    }
}
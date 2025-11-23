#include "battle.h"
#include "game_state.h"
#include "inventory.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

BattleState g_battle_state;

const char* enemy_names[ENEMY_TYPE_COUNT] = {
    "Goblin",
    "Orc",
    "Skeleton",
    "Wolf",
    "Dragon",
    "Demon"
};

void battle_init(uint8_t dungeon_level, bool is_boss) {
    memset(&g_battle_state, 0, sizeof(BattleState));
    
    g_battle_state.is_boss_battle = is_boss;
    g_battle_state.battle_won = false;
    g_battle_state.battle_fled = false;
    g_battle_state.turn_count = 0;
    g_battle_state.current_turn = 0;
    
    if (is_boss) {
        g_battle_state.enemy_count = 0;
        // Boss pointer will be set separately
    } else {
        // Generate random enemies based on dungeon level
        uint8_t enemy_count = random_range(1, 4);
        battle_generate_enemies(dungeon_level, enemy_count);
    }
    
    battle_calculate_turn_order();
    
    printf("\n=== BATTLE START ===\n");
}

void battle_init_boss(BossData* boss) {
    if (!boss) return;
    
    g_battle_state.boss = boss;
    printf("A boss battle with %s!\n", boss->name);
}

void battle_cleanup(void) {
    printf("=== BATTLE END ===\n\n");
}

void battle_generate_enemies(uint8_t dungeon_level, uint8_t count) {
    if (count > MAX_ENEMIES) count = MAX_ENEMIES;
    
    g_battle_state.enemy_count = count;
    
    for (uint8_t i = 0; i < count; i++) {
        Enemy* enemy = &g_battle_state.enemies[i];
        
        // Random enemy type based on dungeon level
        EnemyType type = random_range(0, (dungeon_level < 3) ? 3 : ENEMY_TYPE_COUNT - 1);
        enemy->type = type;
        
        safe_string_copy(enemy->name, enemy_names[type], MAX_NAME_LENGTH);
        
        // Stats based on type and level
        enemy->level = dungeon_level + random_range(0, 3);
        enemy->max_hp = 20 + (enemy->level * 5) + (type * 10);
        enemy->current_hp = enemy->max_hp;
        enemy->attack = 5 + (enemy->level * 2) + type;
        enemy->defense = 3 + (enemy->level) + type;
        enemy->agility = 5 + type;
        enemy->exp_reward = 10 + (enemy->level * 5);
        enemy->gold_reward = 5 + (enemy->level * 3);
        enemy->is_alive = true;
        enemy->item_stolen = false; // Initialize steal tracking
        enemy->buff_count = 0;

        printf("Enemy %d: %s (Lv%d)\n", i+1, enemy->name, enemy->level);
    }
}

void battle_calculate_turn_order(void) {
    // Simple implementation: party members first, then enemies
    // In a real implementation, this would be sorted by agility
    
    uint8_t order_index = 0;
    
    // Add party members
    for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
        g_battle_state.turn_order[order_index++] = i;
    }
    
    // Add enemies (offset by party size)
    for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
        g_battle_state.turn_order[order_index++] = MAX_PARTY_SIZE + i;
    }
    
    g_battle_state.turn_count = order_index;
}

void battle_execute_turn(void) {
    if (battle_is_over()) return;
    
    uint8_t actor = g_battle_state.turn_order[g_battle_state.current_turn];
    
    // Determine if actor is party member or enemy
    if (actor < MAX_PARTY_SIZE) {
        // Party member turn - wait for player input
        PartyMember* member = party_get_member(g_game_state.party, actor);
        if (member && member->stats.current_hp > 0) {
            printf("\n%s's turn!\n", member->name);
            // Player will input action through main game loop
        }
    } else {
        // Enemy turn - AI action
        uint8_t enemy_index = actor - MAX_PARTY_SIZE;
        if (g_battle_state.is_boss_battle && g_battle_state.boss) {
            // Boss AI
            // Note: Boss buff updates would go here if implemented
            printf("\n%s attacks!\n", g_battle_state.boss->name);

            // Choose random party member to attack
            uint8_t target;
            do {
                target = random_range(0, g_game_state.party->member_count - 1);
            } while (g_game_state.party->members[target].stats.current_hp == 0);
            
            PartyMember* target_member = &g_game_state.party->members[target];
            uint16_t damage = battle_calculate_damage(g_battle_state.boss->attack, 
                                         character_get_total_defense(target_member), 
                                         false);
            character_take_damage(target_member, damage);
            
        } else if (enemy_index < g_battle_state.enemy_count) {
            Enemy* enemy = &g_battle_state.enemies[enemy_index];
            if (enemy->is_alive) {
                printf("\n%s attacks!\n", enemy->name);
                
                // Choose random party member to attack
                uint8_t target;
                do {
                    target = random_range(0, g_game_state.party->member_count - 1);
                } while (g_game_state.party->members[target].stats.current_hp == 0);
                
                PartyMember* target_member = &g_game_state.party->members[target];
                uint16_t damage = battle_calculate_damage(enemy->attack, 
                                         character_get_total_defense(target_member), 
                                         false);
                character_take_damage(target_member, damage);
            }
        }
        
        // Auto-advance to next turn for enemies
        g_battle_state.current_turn = (g_battle_state.current_turn + 1) % g_battle_state.turn_count;
    }
}

// Stealable item tables for each enemy type
typedef struct {
    ConsumableItem item_type;
    uint8_t chance;  // Percentage chance to steal this item (out of 100)
    uint8_t quantity_min;
    uint8_t quantity_max;
} StealableItem;

static const StealableItem stealable_items_by_enemy[ENEMY_TYPE_COUNT][3] = {
    // ENEMY_GOBLIN
    {
        {ITEM_POTION, 50, 1, 1},
        {ITEM_ANTIDOTE, 30, 1, 1},
        {ITEM_CONSUMABLE_COUNT, 20, 0, 0}  // 20% chance of nothing (using COUNT as sentinel)
    },
    // ENEMY_ORC
    {
        {ITEM_POTION, 40, 1, 2},
        {ITEM_HI_POTION, 35, 1, 1},
        {ITEM_CONSUMABLE_COUNT, 25, 0, 0}
    },
    // ENEMY_SKELETON
    {
        {ITEM_ANTIDOTE, 45, 1, 1},
        {ITEM_ETHER, 30, 1, 1},
        {ITEM_CONSUMABLE_COUNT, 25, 0, 0}
    },
    // ENEMY_WOLF
    {
        {ITEM_POTION, 55, 1, 1},
        {ITEM_TENT, 25, 1, 1},
        {ITEM_CONSUMABLE_COUNT, 20, 0, 0}
    },
    // ENEMY_DRAGON
    {
        {ITEM_HI_POTION, 40, 1, 2},
        {ITEM_ETHER, 35, 1, 1},
        {ITEM_ELIXIR, 15, 1, 1}
    },
    // ENEMY_DEMON
    {
        {ITEM_ELIXIR, 45, 1, 1},
        {ITEM_ETHER, 35, 1, 2},
        {ITEM_HI_POTION, 20, 1, 1}
    }
};

// Process steal attempt
void battle_process_steal(PartyMember* actor, uint8_t target_index) {
    if (!actor || target_index >= g_battle_state.enemy_count) return;

    Enemy* enemy = &g_battle_state.enemies[target_index];
    if (!enemy->is_alive) {
        printf("%s is already defeated!\n", enemy->name);
        return;
    }

    // Check if already stolen from this enemy
    if (enemy->item_stolen) {
        printf("%s has already been stolen from!\n", enemy->name);
        return;
    }

    // Base success rate: 50% + (Luck / 2)%
    uint8_t base_success = 50 + (character_get_total_luck(actor) / 2);
    // Penalty based on enemy level
    uint8_t level_penalty = enemy->level * 2;
    uint8_t final_success = (base_success > level_penalty) ? (base_success - level_penalty) : 10;

    // Cap at 90%
    if (final_success > 90) final_success = 90;

    // Roll for success
    if (random_range(1, 100) > final_success) {
        printf("%s's steal attempt failed!\n", actor->name);
        return;
    }

    // Determine what to steal based on enemy type
    const StealableItem* steal_table = stealable_items_by_enemy[enemy->type];
    uint8_t roll = random_range(1, 100);
    uint8_t cumulative_chance = 0;

    for (uint8_t i = 0; i < 3; i++) {
        cumulative_chance += steal_table[i].chance;
        if (roll <= cumulative_chance) {
            if (steal_table[i].item_type == ITEM_CONSUMABLE_COUNT) {
                printf("%s found nothing to steal!\n", actor->name);
                return;
            }

            // Determine quantity
            uint8_t quantity = random_range(steal_table[i].quantity_min, steal_table[i].quantity_max);

            // Add item to inventory
            if (inventory_add_item(g_game_state.inventory, steal_table[i].item_type, quantity)) {
                Item item = item_create_consumable(steal_table[i].item_type);
                printf("%s stole %s x%d!\n", actor->name, item.name, quantity);
                enemy->item_stolen = true; // Mark enemy as stolen from
            } else {
                printf("%s's inventory is full!\n", actor->name);
            }
            return;
        }
    }

    // Shouldn't reach here, but just in case
    printf("%s found nothing to steal!\n", actor->name);
}

void battle_use_skill(PartyMember* actor, uint8_t skill_id, uint8_t target_index) {
    if (!actor) return;
    
    const Skill* skill = get_skill_data(skill_id);
    if (!skill) {
        printf("Invalid skill!\n");
        return;
    }
    
    // Check if can use
    if (!character_can_use_skill(actor, skill_id)) {
        printf("%s doesn't have enough MP!\n", actor->name);
        return;
    }
    
    // Use MP
    character_use_mp(actor, skill->mp_cost);
    
    printf("%s uses %s!\n", actor->name, skill->name);
    
    // Process skill effect
    switch (skill->type) {
        case SKILL_TYPE_ATTACK: {
            // Get the appropriate scaling stat
            uint8_t scaling_value = 0;
            bool is_physical = false;

            switch (skill->scaling_stat) {
                case SCALE_STRENGTH:
                    scaling_value = character_get_total_attack(actor);
                    is_physical = true;
                    break;
                case SCALE_AGILITY:
                    scaling_value = character_get_total_agility(actor);
                    is_physical = true;
                    break;
                case SCALE_LUCK:
                    scaling_value = character_get_total_luck(actor);
                    is_physical = true;
                    break;
                case SCALE_INTELLIGENCE:
                    scaling_value = character_get_total_intelligence(actor);
                    is_physical = false;
                    break;
            }

            uint16_t damage = 0;
            uint8_t enemy_def = 0;

            // Get enemy defense for physical skills
            if (is_physical) {
                if (g_battle_state.is_boss_battle) {
                    enemy_def = g_battle_state.boss->defense;
                } else {
                    if (target_index < g_battle_state.enemy_count) {
                        enemy_def = g_battle_state.enemies[target_index].defense;
                    }
                }

                // Physical skill formula: (scaling_stat * 2) + power - defense
                int raw_damage = (scaling_value * 2) + skill->power - enemy_def;
                if (raw_damage < 1) raw_damage = 1;
                damage = (uint16_t)raw_damage;
            } else {
                // Magic skill formula: (scaling_stat * power) / 10 (ignores defense)
                damage = (scaling_value * skill->power) / 10;
                if (damage < 1) damage = 1;
            }

            // Add variance (±15%)
            int variance = random_range(85, 115);
            damage = (damage * variance) / 100;
            if (damage < 1) damage = 1;
            
            if (skill->target_all) {
                // Hit all enemies
                printf("Hits all enemies!\n");
                for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
                    if (g_battle_state.enemies[i].is_alive) {
                        Enemy* enemy = &g_battle_state.enemies[i];
                        if (damage >= enemy->current_hp) {
                            enemy->current_hp = 0;
                            enemy->is_alive = false;
                            printf("%s takes %d damage and is defeated!\n", enemy->name, damage);
                        } else {
                            enemy->current_hp -= damage;
                            printf("%s takes %d damage! (%d HP remaining)\n",
                                   enemy->name, damage, enemy->current_hp);

                            // Apply status effect if skill has one and enemy survived
                            if (skill->status_effect != STATUS_NONE && skill->status_chance > 0) {
                                if (random_range(1, 100) <= skill->status_chance) {
                                    // Add to bitfield
                                    enemy->status_effects |= skill->status_effect;

                                    // Add to duration tracker
                                    bool found = false;
                                    for (uint8_t j = 0; j < enemy->status_effect_count; j++) {
                                        if (enemy->active_status_effects[j].status_type == skill->status_effect) {
                                            enemy->active_status_effects[j].duration = skill->status_duration;
                                            found = true;
                                            break;
                                        }
                                    }

                                    if (!found && enemy->status_effect_count < MAX_STATUS_EFFECTS_PER_CHARACTER) {
                                        enemy->active_status_effects[enemy->status_effect_count].status_type = skill->status_effect;
                                        enemy->active_status_effects[enemy->status_effect_count].duration = skill->status_duration;
                                        enemy->status_effect_count++;
                                    }

                                    printf("%s is afflicted!\n", enemy->name);
                                }
                            }
                        }
                    }
                }
            } else {
                // Single target
                if (g_battle_state.is_boss_battle) {
                    if (damage >= g_battle_state.boss->current_hp) {
                        g_battle_state.boss->current_hp = 0;
                        g_battle_state.battle_won = true;
                        printf("%s takes %d damage and is defeated!\n", g_battle_state.boss->name, damage);
                    } else {
                        g_battle_state.boss->current_hp -= damage;
                        printf("%s takes %d damage! (%d HP remaining)\n", 
                               g_battle_state.boss->name, damage, g_battle_state.boss->current_hp);
                    }
                } else {
                    // Validate target, find next valid if dead
                    if (target_index >= g_battle_state.enemy_count ||
                        !g_battle_state.enemies[target_index].is_alive) {
                        target_index = battle_find_valid_enemy_target();
                    }

                    Enemy* enemy = &g_battle_state.enemies[target_index];
                    if (enemy->is_alive) {
                        if (damage >= enemy->current_hp) {
                            enemy->current_hp = 0;
                            enemy->is_alive = false;
                            printf("%s takes %d damage and is defeated!\n", enemy->name, damage);
                        } else {
                            enemy->current_hp -= damage;
                            printf("%s takes %d damage! (%d HP remaining)\n",
                                   enemy->name, damage, enemy->current_hp);

                            // Apply status effect if skill has one and enemy survived
                            if (skill->status_effect != STATUS_NONE && skill->status_chance > 0) {
                                if (random_range(1, 100) <= skill->status_chance) {
                                    // Add to bitfield
                                    enemy->status_effects |= skill->status_effect;

                                    // Add to duration tracker
                                    bool found = false;
                                    for (uint8_t j = 0; j < enemy->status_effect_count; j++) {
                                        if (enemy->active_status_effects[j].status_type == skill->status_effect) {
                                            enemy->active_status_effects[j].duration = skill->status_duration;
                                            found = true;
                                            break;
                                        }
                                    }

                                    if (!found && enemy->status_effect_count < MAX_STATUS_EFFECTS_PER_CHARACTER) {
                                        enemy->active_status_effects[enemy->status_effect_count].status_type = skill->status_effect;
                                        enemy->active_status_effects[enemy->status_effect_count].duration = skill->status_duration;
                                        enemy->status_effect_count++;
                                    }

                                    printf("%s is afflicted with a status effect!\n", enemy->name);
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        
        case SKILL_TYPE_HEAL: {
            uint16_t heal_amount = skill->power;

            if (skill->target_all) {
                // Heal entire party
                printf("Heals entire party!\n");
                for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
                    PartyMember* member = &g_game_state.party->members[i];
                    if (member->stats.current_hp > 0) {
                        character_heal(member, heal_amount);
                    }
                }

                // Prayer also grants MP regen to all party members
                if (skill->skill_id == SKILL_PRAYER) {
                    printf("%s grants MP regeneration to the party!\n", actor->name);
                    for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
                        PartyMember* member = &g_game_state.party->members[i];
                        if (member->stats.current_hp > 0) {
                            character_add_buff(member, BUFF_REGEN_MP, 5, 2); // +5 MP per turn for 2 turns
                        }
                    }
                }
            } else {
                // Single target heal
                PartyMember* target = &g_game_state.party->members[target_index];
                character_heal(target, heal_amount);
            }
            break;
        }
        
        case SKILL_TYPE_BUFF: {
            // Apply buff based on skill ID
            if (skill->skill_id == SKILL_PROTECT) {
                character_add_buff(actor, BUFF_DEF_UP, 50, 3); // +50% DEF for 3 turns
                printf("%s's defense increased!\n", actor->name);
            } else if (skill->skill_id == SKILL_COUNTER_STANCE) {
                character_add_buff(actor, BUFF_COUNTER, 0, 2); // Counter state for 2 turns
                printf("%s enters counter stance!\n", actor->name);
            } else if (skill->skill_id == SKILL_GUARD) {
                character_add_buff(actor, BUFF_DEFEND, 0, 1); // Double defense for 1 turn
                printf("%s takes a defensive stance!\n", actor->name);
            } else if (skill->skill_id == SKILL_MEDITATION) {
                character_add_buff(actor, BUFF_REGEN_MP, 10, 3); // +10 MP per turn for 3 turns
                printf("%s meditates to restore MP!\n", actor->name);
            } else if (skill->skill_id == SKILL_TRANQUILITY) {
                character_add_buff(actor, BUFF_REGEN_MP, 8, 3); // +8 MP per turn for 3 turns
                printf("%s achieves tranquility!\n", actor->name);
            } else if (skill->skill_id == SKILL_FOCUS) {
                character_add_buff(actor, BUFF_REGEN_MP, 12, 3); // +12 MP per turn for 3 turns
                printf("%s focuses their mind!\n", actor->name);
            }
            break;
        }

        case SKILL_TYPE_DEBUFF: {
            // Apply status effect if skill has one
            if (skill->status_effect != STATUS_NONE && skill->status_chance > 0) {
                if (skill->target_all) {
                    // Apply to all enemies
                    for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
                        if (g_battle_state.enemies[i].is_alive) {
                            Enemy* enemy = &g_battle_state.enemies[i];

                            if (random_range(1, 100) <= skill->status_chance) {
                                // Add to bitfield
                                enemy->status_effects |= skill->status_effect;

                                // Add to duration tracker
                                bool found = false;
                                for (uint8_t j = 0; j < enemy->status_effect_count; j++) {
                                    if (enemy->active_status_effects[j].status_type == skill->status_effect) {
                                        enemy->active_status_effects[j].duration = skill->status_duration;
                                        found = true;
                                        break;
                                    }
                                }

                                if (!found && enemy->status_effect_count < MAX_STATUS_EFFECTS_PER_CHARACTER) {
                                    enemy->active_status_effects[enemy->status_effect_count].status_type = skill->status_effect;
                                    enemy->active_status_effects[enemy->status_effect_count].duration = skill->status_duration;
                                    enemy->status_effect_count++;
                                }

                                printf("%s is afflicted!\n", enemy->name);
                            } else {
                                printf("%s resisted!\n", enemy->name);
                            }
                        }
                    }
                } else {
                    // Single target
                    if (target_index < g_battle_state.enemy_count && g_battle_state.enemies[target_index].is_alive) {
                        Enemy* enemy = &g_battle_state.enemies[target_index];

                        if (random_range(1, 100) <= skill->status_chance) {
                            // Add to bitfield
                            enemy->status_effects |= skill->status_effect;

                            // Add to duration tracker
                            bool found = false;
                            for (uint8_t j = 0; j < enemy->status_effect_count; j++) {
                                if (enemy->active_status_effects[j].status_type == skill->status_effect) {
                                    enemy->active_status_effects[j].duration = skill->status_duration;
                                    found = true;
                                    break;
                                }
                            }

                            if (!found && enemy->status_effect_count < MAX_STATUS_EFFECTS_PER_CHARACTER) {
                                enemy->active_status_effects[enemy->status_effect_count].status_type = skill->status_effect;
                                enemy->active_status_effects[enemy->status_effect_count].duration = skill->status_duration;
                                enemy->status_effect_count++;
                            }

                            printf("%s is afflicted with a status effect!\n", enemy->name);
                        } else {
                            printf("%s resisted the status effect!\n", enemy->name);
                        }
                    }
                }
            } else if (skill->skill_id == SKILL_TAUNT && g_battle_state.is_boss_battle == false) {
                // Legacy taunt (TODO: convert to status effect system)
                if (target_index < g_battle_state.enemy_count) {
                    printf("Enemy is taunted!\n");
                }
            }
            break;
        }

        case SKILL_TYPE_STEAL: {
            // Process steal attempt
            if (g_battle_state.is_boss_battle) {
                printf("You can't steal from a boss!\n");
            } else {
                battle_process_steal(actor, target_index);
            }
            break;
        }
    }
}

void battle_process_action(BattleAction* action) {
    if (!action) return;

    PartyMember* actor = party_get_member(g_game_state.party, action->actor_index);
    if (!actor || actor->stats.current_hp == 0) {
        // Skip dead members
        g_battle_state.current_turn = (g_battle_state.current_turn + 1) % g_battle_state.turn_count;
        return;
    }

    // Update buffs at start of turn (decrement durations, apply effects like MP regen)
    character_update_buffs(actor);

    switch (action->type) {
        case ACTION_ATTACK: {
            if (g_battle_state.is_boss_battle) {
                printf("%s attacks %s!\n", actor->name, g_battle_state.boss->name);
                uint16_t damage = battle_calculate_damage(character_get_total_attack(actor),
                                         g_battle_state.boss->defense,
                                         random_chance(actor->stats.luck));

                if (damage >= g_battle_state.boss->current_hp) {
                    g_battle_state.boss->current_hp = 0;
                    g_battle_state.battle_won = true;
                    printf("%s takes %d damage and is defeated!\n", g_battle_state.boss->name, damage);
                } else {
                    g_battle_state.boss->current_hp -= damage;
                    printf("%s takes %d damage! (%d HP remaining)\n",
                           g_battle_state.boss->name, damage, g_battle_state.boss->current_hp);
                }
            } else {
                // Validate target is alive, otherwise find next valid target
                uint8_t target_index = action->target_index;
                if (target_index >= g_battle_state.enemy_count ||
                    !g_battle_state.enemies[target_index].is_alive) {
                    target_index = battle_find_valid_enemy_target();
                }

                Enemy* target = &g_battle_state.enemies[target_index];
                if (target->is_alive) {
                    printf("%s attacks %s!\n", actor->name, target->name);
                    uint16_t damage = battle_calculate_damage(character_get_total_attack(actor),
                                         target->defense,
                                         random_chance(actor->stats.luck));

                    if (damage >= target->current_hp) {
                        target->current_hp = 0;
                        target->is_alive = false;
                        printf("%s takes %d damage and is defeated!\n", target->name, damage);
                    } else {
                        target->current_hp -= damage;
                        printf("%s takes %d damage! (%d HP remaining)\n",
                               target->name, damage, target->current_hp);
                    }
                }
            }
            break;
        }
        case ACTION_SKILL: {
			battle_use_skill(actor, action->item_or_skill_id, action->target_index);
			break;
		}
        case ACTION_DEFEND:
            printf("%s defends!\n", actor->name);
            character_add_buff(actor, BUFF_DEFEND, 0, 1); // Doubles defense for 1 turn
            break;
            
        case ACTION_FLEE:
            if (battle_attempt_flee()) {
                printf("Successfully fled from battle!\n");
                g_battle_state.battle_fled = true;
            } else {
                printf("Cannot escape!\n");
            }
            break;
            
        default:
            printf("Action not yet implemented\n");
            break;
    }
    
    // Advance turn
    g_battle_state.current_turn = (g_battle_state.current_turn + 1) % g_battle_state.turn_count;
}

uint16_t battle_calculate_damage(uint8_t attacker_atk, uint8_t defender_def, bool is_critical) {
    // Simple damage formula
    int damage = attacker_atk * 2 - defender_def;
    
    if (damage < 1) damage = 1;
    
    // Random variance (±25%)
    int variance = random_range(75, 125);
    damage = (damage * variance) / 100;
    
    // Critical hit
    if (is_critical) {
        damage *= 2;
        printf("Critical hit! ");
    }
    
    return (uint16_t)damage;
}

bool battle_attempt_flee(void) {
    // Cannot flee from boss battles
    if (g_battle_state.is_boss_battle) {
        return false;
    }
    
    // 50% chance to flee
    return random_chance(50);
}

bool battle_is_over(void) {
    // Check if party is defeated
    if (party_is_defeated(g_game_state.party)) {
        return true;
    }
    
    // Check if battle was fled
    if (g_battle_state.battle_fled) {
        return true;
    }
    
    // Check if battle was won
    if (g_battle_state.battle_won) {
        return true;
    }
    
    // Check if all enemies defeated
    if (g_battle_state.is_boss_battle) {
        return g_battle_state.boss && g_battle_state.boss->current_hp == 0;
    } else {
        for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
            if (g_battle_state.enemies[i].is_alive) {
                return false;
            }
        }
        return true;
    }
}

bool battle_is_victory(void) {
    if (!battle_is_over()) return false;
    
    if (g_battle_state.battle_fled || party_is_defeated(g_game_state.party)) {
        return false;
    }
    
    return true;
}

void battle_distribute_rewards(void) {
    if (!battle_is_victory()) return;

    uint32_t total_exp = 0;
    uint16_t total_gold = 0;

    if (g_battle_state.is_boss_battle) {
        total_exp = g_battle_state.boss->level * 100;
        total_gold = g_battle_state.boss->level * 100; // Increased from *50 for better economy
    } else {
        for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
            total_exp += g_battle_state.enemies[i].exp_reward;
            total_gold += g_battle_state.enemies[i].gold_reward;
        }
    }

    printf("\n=== VICTORY! ===\n");
    printf("Gained %d EXP and %d Gold\n", total_exp, total_gold);

    g_game_state.gold += total_gold;

    // Distribute EXP to all living party members
    for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
        PartyMember* member = &g_game_state.party->members[i];
        if (member->stats.current_hp > 0) {
            character_gain_experience(member, total_exp);
        }
    }

    // Item drops from enemies (only if not boss battle and not already stolen from)
    if (!g_battle_state.is_boss_battle) {
        for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
            Enemy* enemy = &g_battle_state.enemies[i];

            // Skip if this enemy was already stolen from
            if (enemy->item_stolen) {
                continue;
            }

            // 25% chance to drop an item
            if (random_range(1, 100) <= 25) {
                // Determine item based on enemy level
                uint8_t item_id;
                if (enemy->level <= 3) {
                    // Low level: Basic items only (Potion, Antidote)
                    uint8_t low_tier_items[] = {ITEM_POTION, ITEM_ANTIDOTE};
                    item_id = low_tier_items[random_range(0, 1)];
                } else if (enemy->level <= 7) {
                    // Mid level: Common and uncommon items (Potion, Hi-Potion, Ether, Antidote)
                    uint8_t mid_tier_items[] = {ITEM_POTION, ITEM_HI_POTION, ITEM_ETHER, ITEM_ANTIDOTE};
                    item_id = mid_tier_items[random_range(0, 3)];
                } else {
                    // High level: All items including rare (Hi-Potion, Ether, Elixir)
                    uint8_t high_tier_items[] = {ITEM_HI_POTION, ITEM_ETHER, ITEM_ELIXIR};
                    item_id = high_tier_items[random_range(0, 2)];
                }

                // Add item to inventory (inventory_add_item prints "Obtained" message)
                if (inventory_add_item(g_game_state.inventory, item_id, 1)) {
                    printf("  %s dropped it!\n", enemy->name);
                }
            }
        }
    }
}

uint8_t battle_find_valid_enemy_target(void) {
    // Find the first alive enemy
    for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
        if (g_battle_state.enemies[i].is_alive) {
            return i;
        }
    }
    return 0; // Fallback (shouldn't happen if battle isn't over)
}
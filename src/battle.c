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
            // Calculate magic damage based on intelligence
            uint8_t magic_attack = character_get_total_intelligence(actor);
            uint16_t damage = (skill->power * magic_attack) / 10;
            
            // Add variance
            int variance = random_range(85, 115);
            damage = (damage * variance) / 100;
            
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
            } else {
                // Single target heal
                PartyMember* target = &g_game_state.party->members[target_index];
                character_heal(target, heal_amount);
            }
            break;
        }
        
        case SKILL_TYPE_BUFF:
            printf("%s is protected!\n", actor->name);
            // TODO: Implement buff system
            break;
            
        case SKILL_TYPE_DEBUFF:
            printf("Enemy is weakened!\n");
            // TODO: Implement debuff system
            break;
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
                    printf("*** %s defeated! ***\n", g_battle_state.boss->name);
                } else {
                    g_battle_state.boss->current_hp -= damage;
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
                        printf("*** %s defeated! ***\n", target->name);
                    } else {
                        target->current_hp -= damage;
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
            // Defense bonus would be applied on damage calculation
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
        total_gold = g_battle_state.boss->level * 50;
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
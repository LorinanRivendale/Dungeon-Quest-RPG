#include "game_state.h"
#include "party.h"
#include "dungeon.h"
#include "battle.h"
#include "inventory.h"
#include "save_system.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void handle_party_selection(void);
void handle_dungeon_selection(void);
void handle_dungeon_exploration(void);
void handle_battle_phase(void);
void handle_inventory_menu(void);
void handle_save_menu(void);
void handle_load_menu(void);

int main(void) {
    // Initialize game
    game_state_init();
    random_seed((uint32_t)time(NULL));
    
    printf("Welcome to Dungeon Quest RPG!\n");
    printf("A test scenario for our GameBoy RPG\n\n");

    printf("1. New Game\n");
    printf("2. Load Game\n");
    printf("\nSelect: ");

    int title_choice;
    scanf("%d", &title_choice);

    if (title_choice == 2) {
        // Load game
        handle_load_menu();

        // If no game was loaded (cancelled or failed), start new game
        if (!g_game_state.party || g_game_state.party->member_count == 0) {
            printf("\nNo game loaded. Starting new game...\n");
            input_wait_for_key();
            game_state_change(STATE_PARTY_SELECT);
            handle_party_selection();
        }
    } else {
        // New game
        game_state_change(STATE_PARTY_SELECT);
        handle_party_selection();
    }
    
    if (!g_game_state.party || g_game_state.party->member_count == 0) {
        printf("No party members selected. Exiting.\n");
        game_state_cleanup();
        return 0;
    }
    
    // Create inventory
	g_game_state.inventory = inventory_create();

	// Give starting equipment to party
	inventory_give_starting_equipment(g_game_state.inventory, g_game_state.party);

	// Add starting items
	inventory_add_item(g_game_state.inventory, ITEM_POTION, 10);
	inventory_add_item(g_game_state.inventory, ITEM_HI_POTION, 3);
	inventory_add_item(g_game_state.inventory, ITEM_ETHER, 5);
	inventory_add_item(g_game_state.inventory, ITEM_ANTIDOTE, 3);
	inventory_add_item(g_game_state.inventory, ITEM_TENT, 2);
    
    // Main game loop
    bool game_running = true;
    
    while (game_running) {
        switch (g_game_state.current_state) {
            case STATE_DUNGEON_SELECT:
                handle_dungeon_selection();
                break;
                
            case STATE_DUNGEON_EXPLORE:
                handle_dungeon_exploration();
                break;
                
            case STATE_BATTLE:
            case STATE_BOSS_BATTLE:
                handle_battle_phase();
                break;
                
            case STATE_INVENTORY:
                handle_inventory_menu();
                break;
                
            case STATE_VICTORY:
                printf("\n\n======================\n");
                printf("  CONGRATULATIONS!\n");
                printf("======================\n");
                printf("You have completed all dungeons and defeated the final boss!\n");
                printf("The world is saved!\n");
                game_running = false;
                break;
                
            case STATE_GAME_OVER:
				printf("\n\n======================\n");
				printf("     GAME OVER\n");
				printf("======================\n");
				printf("Your party has been defeated...\n\n");
				printf("Return to title? (Y/N): ");
				
				char restart;
				scanf(" %c", &restart);
				if (restart == 'Y' || restart == 'y') {
					game_state_cleanup();
					game_state_init();
					game_state_change(STATE_PARTY_SELECT);
					handle_party_selection();
					
					if (!g_game_state.party || g_game_state.party->member_count == 0) {
						game_running = false;
					} else {
						g_game_state.inventory = inventory_create();
						inventory_give_starting_equipment(g_game_state.inventory, g_game_state.party);
						inventory_add_item(g_game_state.inventory, ITEM_POTION, 10);
						inventory_add_item(g_game_state.inventory, ITEM_HI_POTION, 3);
						inventory_add_item(g_game_state.inventory, ITEM_ETHER, 5);
						inventory_add_item(g_game_state.inventory, ITEM_ANTIDOTE, 3);
						inventory_add_item(g_game_state.inventory, ITEM_TENT, 2);
						game_state_change(STATE_DUNGEON_SELECT);
					}
				} else {
					game_running = false;
				}
				break;
                
            default:
                game_running = false;
                break;
        }
        
        // Check for party defeat
        if (party_is_defeated(g_game_state.party)) {
            game_state_change(STATE_GAME_OVER);
        }
    }
    
    input_wait_for_key();
    
    // Cleanup
    game_state_cleanup();
    
    return 0;
}

void handle_party_selection(void) {
    clear_screen();
    printf("\n=== PARTY SELECTION ===\n");
    printf("Select 4 party members from the following jobs:\n\n");
    
    for (int i = 0; i < MAX_JOB_TYPES; i++) {
        printf("%d. %s\n", i + 1, job_names[i]);
    }
    
    printf("\nEnter job number for each party member (1-%d):\n", MAX_JOB_TYPES);
    
    g_game_state.party = party_create();
    
    for (int i = 0; i < MAX_PARTY_SIZE; i++) {
        int job_choice;
        char name[MAX_NAME_LENGTH];
        
        printf("\nParty Member %d:\n", i + 1);
        printf("Job: ");
        scanf("%d", &job_choice);
        
        if (job_choice < 1 || job_choice > MAX_JOB_TYPES) {
            printf("Invalid job choice. Using Knight.\n");
            job_choice = 1;
        }
        
        printf("Name: ");
        scanf("%s", name);
        
        party_add_member(g_game_state.party, (JobType)(job_choice - 1), name);
    }
    
    printf("\n");
    display_party_status();
    input_wait_for_key();
    
    game_state_change(STATE_DUNGEON_SELECT);
}

void handle_dungeon_selection(void) {
    clear_screen();
    printf("\n=== DUNGEON SELECTION ===\n");
    printf("Select a dungeon to explore:\n\n");
    
    // Show available dungeons
    for (int i = 0; i < MAX_DUNGEONS; i++) {
        printf("%d. %s ", i + 1, dungeon_names[i]);
        if (g_game_state.dungeons_completed[i]) {
            printf("[COMPLETED]");
        }
        printf("\n");
    }
    
    // Show final dungeon if unlocked
    if (is_final_dungeon_unlocked()) {
        printf("%d. %s [FINAL DUNGEON]\n", MAX_DUNGEONS + 1, dungeon_names[MAX_DUNGEONS]);
    }
    
    printf("\nKey Items Collected: %d/4\n", __builtin_popcount(g_game_state.key_items_collected));

    printf("\nSelect dungeon (1-%d), I=Inventory, P=Party, S=Save, L=Load, or 0 to quit: ",
           is_final_dungeon_unlocked() ? MAX_DUNGEONS + 1 : MAX_DUNGEONS);

    char input_str[10];
    scanf("%s", input_str);

    if (input_str[0] == 'I' || input_str[0] == 'i') {
        handle_inventory_menu();
        return;
    } else if (input_str[0] == 'P' || input_str[0] == 'p') {
        display_party_status();
        input_wait_for_key();
        return;
    } else if (input_str[0] == 'S' || input_str[0] == 's') {
        handle_save_menu();
        return;
    } else if (input_str[0] == 'L' || input_str[0] == 'l') {
        handle_load_menu();
        return;
    }

    int choice = atoi(input_str);

    if (choice == 0) {
        printf("Exiting to title...\n");
        input_wait_for_key();
        game_state_change(STATE_GAME_OVER);
        return;
    }
    
    choice--; // Convert to 0-based index
    
    if (choice < 0 || choice > MAX_DUNGEONS) {
        printf("Invalid selection.\n");
        input_wait_for_key();
        return;
    }
    
    // Check if final dungeon
    if (choice == MAX_DUNGEONS) {
        if (!is_final_dungeon_unlocked()) {
            printf("The Final Sanctum is still sealed!\n");
            input_wait_for_key();
            return;
        }
    }
    
    // Initialize dungeon if it hasn't been initialized yet
    if (!g_game_state.dungeon_initialized[choice]) {
        uint8_t floor_count = (choice == MAX_DUNGEONS) ? 5 : 3;
        dungeon_init(&g_game_state.dungeons[choice], choice, dungeon_names[choice], floor_count);
        g_game_state.dungeon_initialized[choice] = true;
    }

    g_game_state.current_dungeon_index = choice;
    game_state_change(STATE_DUNGEON_EXPLORE);
}

void handle_dungeon_exploration(void) {
    Dungeon* current_dungeon = &g_game_state.dungeons[g_game_state.current_dungeon_index];

    if (!g_game_state.dungeon_initialized[g_game_state.current_dungeon_index]) {
        game_state_change(STATE_DUNGEON_SELECT);
        return;
    }
    
    bool in_dungeon = true;
    
    while (in_dungeon) {
        clear_screen();
        display_dungeon();
        display_party_status();
        
        InputButton input = INPUT_NONE;
        
        // Wait for input
        while (input == INPUT_NONE) {
            input = input_get_key();
        }
        
        int8_t dx = 0, dy = 0;
        bool moved = false;
        
        switch (input) {
            case INPUT_UP:
                dy = -1;
                break;
            case INPUT_DOWN:
                dy = 1;
                break;
            case INPUT_LEFT:
                dx = -1;
                break;
            case INPUT_RIGHT:
                dx = 1;
                break;
            case INPUT_A:
                // Check current tile for special actions
                {
                    TileType current_tile = dungeon_get_current_tile(current_dungeon);
                    
                    if (current_tile == TILE_STAIRS_DOWN) {
                        if (dungeon_change_floor(current_dungeon, true)) {
                            printf("Descended to next floor!\n");
                            input_wait_for_key();
                        }
                    } else if (current_tile == TILE_STAIRS_UP) {
                        if (dungeon_change_floor(current_dungeon, false)) {
                            printf("Ascended to previous floor!\n");
                            input_wait_for_key();
                        } else {
                            printf("Exiting dungeon...\n");
                            in_dungeon = false;
                            game_state_change(STATE_DUNGEON_SELECT);
                            input_wait_for_key();
                        }
                    } else if (current_tile == TILE_TREASURE) {
                        printf("\nFound treasure chest!\n");
                        
                        // Random treasure
                        if (random_chance(70)) {
                            uint16_t gold = random_range(50, 200);
                            g_game_state.gold += gold;
                            printf("Found %d gold!\n", gold);
                        } else {
                            uint8_t item_id = random_range(ITEM_POTION, ITEM_ETHER);
                            inventory_add_item(g_game_state.inventory, item_id, 1);
                        }
                        
                        // Mark treasure as taken
                        DungeonFloor* floor = &current_dungeon->floors[current_dungeon->current_floor];
                        floor->tiles[floor->player_y][floor->player_x].type = TILE_FLOOR;
                        
                        input_wait_for_key();
                    } else if (current_tile == TILE_BOSS_ROOM) {
                        if (!current_dungeon->boss_defeated) {
                            printf("\nThe dungeon boss appears!\n");
                            input_wait_for_key();
                            
                            // Initiate boss battle
                            battle_init(current_dungeon->current_floor + 5, true);
                            battle_init_boss(&current_dungeon->boss);
                            game_state_change(STATE_BOSS_BATTLE);
                            in_dungeon = false;
                        } else {
                            printf("The boss chamber is empty.\n");
                            input_wait_for_key();
                        }
                    }
                }
                break;
                
            case INPUT_B:
                printf("\nReturn to dungeon selection? (Y/N): ");
                char confirm;
                scanf(" %c", &confirm);
                if (confirm == 'Y' || confirm == 'y') {
                    in_dungeon = false;
                    game_state_change(STATE_DUNGEON_SELECT);
                }
                break;
                
            case INPUT_SELECT:
				// Open inventory
				handle_inventory_menu();
				break;
                
            case INPUT_START:
    // Camp menu
			printf("\n=== CAMP ===\n");
			printf("1. View Party Status\n");
			printf("2. Use Tent\n");
			printf("3. Save Game\n");
			printf("4. Return\n");
			printf("Select: ");

			int camp_choice;
			scanf("%d", &camp_choice);

			if (camp_choice == 1) {
				display_party_status();
				input_wait_for_key();
			} else if (camp_choice == 2) {
				int8_t tent_index = inventory_find_item(g_game_state.inventory, ITEM_TENT);
				if (tent_index >= 0) {
					printf("Use tent to rest? (Y/N): ");
					char confirm;
					scanf(" %c", &confirm);
					if (confirm == 'Y' || confirm == 'y') {
						party_heal_all(g_game_state.party);
						inventory_remove_item(g_game_state.inventory, tent_index, 1);
						printf("Party is fully rested!\n");
						input_wait_for_key();
					}
				} else {
					printf("No tents available!\n");
					input_wait_for_key();
				}
			} else if (camp_choice == 3) {
				handle_save_menu();
			}
			break;
                
            default:
                break;
        }
        
        // Handle movement
        if (dx != 0 || dy != 0) {
            if (dungeon_move_player(current_dungeon, dx, dy)) {
                moved = true;
                
                // Check for random encounter after moving
                if (dungeon_check_encounter(current_dungeon)) {
                    printf("\nMonsters appear!\n");
                    input_wait_for_key();
                    
                    battle_init(current_dungeon->current_floor + 1, false);
                    game_state_change(STATE_BATTLE);
                    in_dungeon = false;
                }
            } else {
                printf("\nCan't move that way!\n");
                // Brief pause instead of waiting for key
            }
        }
    }
}

void handle_battle_phase(void) {
    bool battle_active = true;
    
    while (battle_active && !battle_is_over()) {
        clear_screen();
        display_battle_scene();
        
        // Execute enemy turns automatically
        while (g_battle_state.current_turn >= MAX_PARTY_SIZE && 
               g_battle_state.current_turn < g_battle_state.turn_count &&
               !battle_is_over()) {
            battle_execute_turn();
        }
        
        if (battle_is_over()) break;
        
        // Player's turn
        uint8_t current_member_index = g_battle_state.turn_order[g_battle_state.current_turn];
        PartyMember* current_member = party_get_member(g_game_state.party, current_member_index);
        
        if (!current_member || current_member->stats.current_hp == 0) {
            // Skip dead member
            g_battle_state.current_turn = (g_battle_state.current_turn + 1) % g_battle_state.turn_count;
            continue;
        }
        
        printf("\n%s's turn!\n", current_member->name);
		printf("\n1. Attack\n");
		printf("2. Skill/Magic\n");
		printf("3. Item\n");
		printf("4. Defend\n");
		printf("5. Flee\n");
		printf("\nSelect action: ");

		int action_choice;
		scanf("%d", &action_choice);

		BattleAction action;
		action.actor_index = current_member_index;

		switch (action_choice) {
			case 1: // Attack
				action.type = ACTION_ATTACK;
				
				// Select target
				if (g_battle_state.is_boss_battle) {
					action.target_index = 0;
				} else {
					printf("\nSelect target:\n");
					for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
						if (g_battle_state.enemies[i].is_alive) {
							printf("%d. %s (HP: %d)\n", i + 1, 
								   g_battle_state.enemies[i].name,
								   g_battle_state.enemies[i].current_hp);
						}
					}
					
					int target;
					scanf("%d", &target);
					target--;
					
					if (target < 0 || target >= g_battle_state.enemy_count ||
						!g_battle_state.enemies[target].is_alive) {
						printf("Invalid target! Attacking next valid enemy.\n");
						target = battle_find_valid_enemy_target();
					}
					
					action.target_index = target;
				}
				
				battle_process_action(&action);
				input_wait_for_key();
				break;
				
			case 2: // Skill/Magic
				if (current_member->skill_count == 0) {
					printf("%s has no skills!\n", current_member->name);
					input_wait_for_key();
					continue;
				}
				
				printf("\nSkills/Magic:\n");
				for (uint8_t i = 0; i < current_member->skill_count; i++) {
					const Skill* skill = get_skill_data(current_member->skills[i]);
					if (skill) {
						printf("%d. %s (MP: %d) - %s\n", 
							   i + 1, skill->name, skill->mp_cost, skill->description);
					}
				}
				printf("Current MP: %d/%d\n", current_member->stats.current_mp, current_member->stats.max_mp);
				
				printf("\nSelect skill (0 to cancel): ");
				int skill_choice;
				scanf("%d", &skill_choice);
				
				if (skill_choice <= 0 || skill_choice > current_member->skill_count) {
					printf("Cancelled.\n");
					input_wait_for_key();
					continue;
				}
				
				uint8_t skill_id = current_member->skills[skill_choice - 1];
				const Skill* selected_skill = get_skill_data(skill_id);
				
				if (!selected_skill) {
					printf("Invalid skill!\n");
					input_wait_for_key();
					continue;
				}
				
				// Check MP
				if (current_member->stats.current_mp < selected_skill->mp_cost) {
					printf("Not enough MP!\n");
					input_wait_for_key();
					continue;
				}
				
				action.type = ACTION_SKILL;
				action.item_or_skill_id = skill_id;
				
				// Select target
				if (selected_skill->target_enemy) {
					if (selected_skill->target_all) {
						action.target_index = 0; // Doesn't matter for AoE
					} else {
						// Select enemy target
						if (g_battle_state.is_boss_battle) {
							action.target_index = 0;
						} else {
							printf("\nSelect target:\n");
							for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
								if (g_battle_state.enemies[i].is_alive) {
									printf("%d. %s (HP: %d)\n", i + 1,
										   g_battle_state.enemies[i].name,
										   g_battle_state.enemies[i].current_hp);
								}
							}
							
							int target;
							scanf("%d", &target);
							target--;

							if (target < 0 || target >= g_battle_state.enemy_count ||
								!g_battle_state.enemies[target].is_alive) {
								printf("Invalid target! Targeting next valid enemy.\n");
								target = battle_find_valid_enemy_target();
							}

							action.target_index = target;
						}
					}
				} else {
					// Select ally target
					if (selected_skill->target_all) {
						action.target_index = 0; // Doesn't matter for AoE
					} else {
						printf("\nSelect party member:\n");
						for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
							PartyMember* member = &g_game_state.party->members[i];
							printf("%d. %s (HP: %d/%d)\n", i + 1, member->name,
								   member->stats.current_hp, member->stats.max_hp);
						}
						
						int target;
						scanf("%d", &target);
						target--;
						
						if (target < 0 || target >= g_game_state.party->member_count) {
							target = current_member_index;
						}
						
						action.target_index = target;
					}
				}
				
				battle_process_action(&action);
				input_wait_for_key();
				break;
				
			case 3: // Item
				printf("\nItems:\n");
				if (g_game_state.inventory->item_count == 0) {
					printf("No items!\n");
					input_wait_for_key();
					continue;
				}
				
				for (uint8_t i = 0; i < g_game_state.inventory->item_count; i++) {
					Item* item = &g_game_state.inventory->items[i];
					printf("%d. %s x%d\n", i + 1, item->name, item->quantity);
				}
				
				printf("\nSelect item (0 to cancel): ");
				int item_choice;
				scanf("%d", &item_choice);
				
				if (item_choice > 0 && item_choice <= g_game_state.inventory->item_count) {
					printf("\nUse on which party member?\n");
					for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
						PartyMember* member = &g_game_state.party->members[i];
						printf("%d. %s (HP: %d/%d)\n", i + 1, member->name,
							   member->stats.current_hp, member->stats.max_hp);
					}
					
					int member_choice;
					scanf("%d", &member_choice);
					
					if (member_choice > 0 && member_choice <= g_game_state.party->member_count) {
						inventory_use_item(g_game_state.inventory, item_choice - 1, member_choice - 1);
					}
				}
				
				// Using item doesn't advance turn in this implementation
				input_wait_for_key();
				continue; // Don't advance turn
				
			case 4: // Defend
				action.type = ACTION_DEFEND;
				battle_process_action(&action);
				input_wait_for_key();
				break;
				
			case 5: // Flee
				action.type = ACTION_FLEE;
				battle_process_action(&action);
				input_wait_for_key();
				
				if (g_battle_state.battle_fled) {
					battle_active = false;
				}
				break;
				
			default:
				printf("Invalid action!\n");
				input_wait_for_key();
				continue; // Don't advance turn
		}
	}	
	
    
    // Battle ended
    clear_screen();
    
    if (party_is_defeated(g_game_state.party)) {
        printf("\nYour party has been defeated!\n");
        game_state_change(STATE_GAME_OVER);
    } else if (g_battle_state.battle_fled) {
        printf("\nEscaped from battle!\n");
        battle_cleanup();
        game_state_change(STATE_DUNGEON_EXPLORE);
    } else if (battle_is_victory()) {
        printf("\nVICTORY!\n");
        battle_distribute_rewards();
        
        // Check if it was a boss battle
        if (g_battle_state.is_boss_battle) {
            Dungeon* current_dungeon = &g_game_state.dungeons[g_game_state.current_dungeon_index];

            if (g_game_state.dungeon_initialized[g_game_state.current_dungeon_index]) {
                current_dungeon->boss_defeated = true;
                
                // Award key item for regular dungeons
                if (g_game_state.current_dungeon_index < MAX_DUNGEONS) {
                    collect_key_item(current_dungeon->boss.key_item_reward);
                    dungeon_mark_completed(current_dungeon);
                    g_game_state.dungeons_completed[g_game_state.current_dungeon_index] = true;
                    
                    battle_cleanup();
                    game_state_change(STATE_DUNGEON_SELECT);
                } else {
                    // Final boss defeated
                    printf("\n*** THE FINAL BOSS HAS BEEN DEFEATED! ***\n");
                    battle_cleanup();
                    game_state_change(STATE_VICTORY);
                }
            }
        } else {
            battle_cleanup();
            game_state_change(STATE_DUNGEON_EXPLORE);
        }
        
        input_wait_for_key();
    }
}

void handle_inventory_menu(void) {
    bool in_inventory = true;
    
    while (in_inventory) {
        clear_screen();
        printf("\n=== INVENTORY ===\n");
        printf("Gold: %d\n\n", g_game_state.gold);
        
        printf("Items:\n");
        if (g_game_state.inventory->item_count == 0) {
            printf("  (No items)\n");
        } else {
            for (uint8_t i = 0; i < g_game_state.inventory->item_count; i++) {
                Item* item = &g_game_state.inventory->items[i];
                printf("  %d. %s x%d\n", i + 1, item->name, item->quantity);
            }
        }
        
        printf("\nEquipment:\n");
        if (g_game_state.inventory->equipment_count == 0) {
            printf("  (No equipment)\n");
        } else {
            for (uint8_t i = 0; i < g_game_state.inventory->equipment_count; i++) {
                Item* equip = &g_game_state.inventory->equipment[i];
                printf("  %d. %s", i + 1, equip->name);
                
                // Show stats
                if (equip->attack_bonus > 0) printf(" (ATK+%d)", equip->attack_bonus);
                if (equip->defense_bonus > 0) printf(" (DEF+%d)", equip->defense_bonus);
                if (equip->intelligence_bonus > 0) printf(" (INT+%d)", equip->intelligence_bonus);
                if (equip->agility_bonus > 0) printf(" (AGI+%d)", equip->agility_bonus);
                
                if (equip->is_equipped) {
                    printf(" [EQUIPPED]");
                }
                printf("\n");
            }
        }
        
        printf("\nKey Items:\n");
        if (g_game_state.key_items_collected & KEY_ITEM_EARTH_CRYSTAL)
            printf("  Earth Crystal\n");
        if (g_game_state.key_items_collected & KEY_ITEM_WATER_CRYSTAL)
            printf("  Water Crystal\n");
        if (g_game_state.key_items_collected & KEY_ITEM_FIRE_CRYSTAL)
            printf("  Fire Crystal\n");
        if (g_game_state.key_items_collected & KEY_ITEM_WIND_CRYSTAL)
            printf("  Wind Crystal\n");
        
        printf("\n1. Use Item\n");
        printf("2. View Party Status\n");
        printf("3. View Equipment Details\n");
        printf("4. Return\n");
        printf("\nSelect: ");
        
        int choice;
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                if (g_game_state.inventory->item_count == 0) {
                    printf("No items to use!\n");
                    input_wait_for_key();
                    break;
                }
                
                printf("\nSelect item: ");
                int item_choice;
                scanf("%d", &item_choice);
                
                if (item_choice > 0 && item_choice <= g_game_state.inventory->item_count) {
                    printf("\nUse on which party member?\n");
                    for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
                        PartyMember* member = &g_game_state.party->members[i];
                        printf("%d. %s (HP: %d/%d, MP: %d/%d)\n", i + 1, member->name,
                               member->stats.current_hp, member->stats.max_hp,
                               member->stats.current_mp, member->stats.max_mp);
                    }
                    
                    int member_choice;
                    scanf("%d", &member_choice);
                    
                    if (member_choice > 0 && member_choice <= g_game_state.party->member_count) {
                        inventory_use_item(g_game_state.inventory, item_choice - 1, member_choice - 1);
                    }
                }
                
                input_wait_for_key();
                break;
                
            case 2:
                display_party_status();
                input_wait_for_key();
                break;
                
            case 3:
                // Equipment details
                clear_screen();
                printf("\n=== EQUIPMENT DETAILS ===\n\n");
                
                for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
                    PartyMember* member = &g_game_state.party->members[i];
                    printf("%s the %s:\n", member->name, job_names[member->job]);
                    
                    // Show equipped items
                    for (uint8_t slot = 0; slot < EQUIP_SLOT_COUNT; slot++) {
                        const char* slot_names[] = {"Weapon", "Armor", "Helmet", "Accessory"};
                        printf("  %s: ", slot_names[slot]);
                        
                        uint8_t equip_idx = member->equipped_items[slot];
                        if (equip_idx != 0xFF && equip_idx < g_game_state.inventory->equipment_count) {
                            Item* equip = &g_game_state.inventory->equipment[equip_idx];
                            printf("%s", equip->name);
                            if (equip->attack_bonus > 0) printf(" (ATK+%d)", equip->attack_bonus);
                            if (equip->defense_bonus > 0) printf(" (DEF+%d)", equip->defense_bonus);
                            if (equip->intelligence_bonus > 0) printf(" (INT+%d)", equip->intelligence_bonus);
                            if (equip->agility_bonus > 0) printf(" (AGI+%d)", equip->agility_bonus);
                        } else {
                            printf("(None)");
                        }
                        printf("\n");
                    }
                    
                    // Show total stats
                    printf("  Total ATK: %d, DEF: %d, INT: %d, AGI: %d\n\n",
                           character_get_total_attack(member),
                           character_get_total_defense(member),
                           character_get_total_intelligence(member),
                           character_get_total_agility(member));
                }
                
                input_wait_for_key();
                break;
                
            case 4:
                in_inventory = false;
                game_state_change(STATE_DUNGEON_EXPLORE);
                break;
                
            default:
                printf("Invalid choice!\n");
                input_wait_for_key();
                break;
        }
    }
}

void handle_save_menu(void) {
    clear_screen();
    printf("\n=== SAVE GAME ===\n");
    display_save_slots();
    printf("\n0. Cancel\n");
    printf("\nSelect save slot (0-2, or S for suspend save): ");

    char choice_str[10];
    scanf("%s", choice_str);

    if (choice_str[0] == '0') {
        return;
    } else if (choice_str[0] == 'S' || choice_str[0] == 's') {
        // Suspend save
        if (save_suspend_game()) {
            printf("\nSuspend save created! (Will be deleted when loaded)\n");
        } else {
            printf("\nFailed to create suspend save!\n");
        }
        input_wait_for_key();
    } else {
        int slot = atoi(choice_str) - 1;
        if (slot >= 0 && slot < MAX_SAVE_SLOTS) {
            if (save_slot_exists(slot)) {
                printf("\nOverwrite existing save? (Y/N): ");
                char confirm;
                scanf(" %c", &confirm);
                if (confirm != 'Y' && confirm != 'y') {
                    return;
                }
            }

            if (save_game_to_slot(slot)) {
                printf("\nGame saved successfully!\n");
            } else {
                printf("\nFailed to save game!\n");
            }
            input_wait_for_key();
        } else {
            printf("\nInvalid slot!\n");
            input_wait_for_key();
        }
    }
}

void handle_load_menu(void) {
    clear_screen();
    printf("\n=== LOAD GAME ===\n");
    display_save_slots();
    printf("\n0. Cancel\n");
    printf("\nSelect save slot to load (0-2, or S for suspend save): ");

    char choice_str[10];
    scanf("%s", choice_str);

    if (choice_str[0] == '0') {
        return;
    } else if (choice_str[0] == 'S' || choice_str[0] == 's') {
        // Load suspend save
        if (suspend_save_exists()) {
            if (load_suspend_game()) {
                printf("\nSuspend save loaded! (Save file deleted)\n");
                input_wait_for_key();
                // Game state is now loaded, will continue from loaded state
            } else {
                printf("\nFailed to load suspend save!\n");
                input_wait_for_key();
            }
        } else {
            printf("\nNo suspend save found!\n");
            input_wait_for_key();
        }
    } else {
        int slot = atoi(choice_str) - 1;
        if (slot >= 0 && slot < MAX_SAVE_SLOTS) {
            if (save_slot_exists(slot)) {
                if (load_game_from_slot(slot)) {
                    printf("\nGame loaded successfully!\n");
                    input_wait_for_key();
                    // Game state is now loaded, will continue from loaded state
                } else {
                    printf("\nFailed to load game!\n");
                    input_wait_for_key();
                }
            } else {
                printf("\nNo save data in this slot!\n");
                input_wait_for_key();
            }
        } else {
            printf("\nInvalid slot!\n");
            input_wait_for_key();
        }
    }
}
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

    bool game_loaded = false;

    if (title_choice == 2) {
        // Load game
        handle_load_menu();

        // Check if game was actually loaded
        if (g_game_state.party && g_game_state.party->member_count > 0 && g_game_state.inventory) {
            game_loaded = true;
        } else {
            // If no game was loaded (cancelled or failed), start new game
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

    // Only create inventory and give starting equipment for NEW games (not loaded games)
    if (!game_loaded) {
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
    }
    
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

void handle_town(void) {
    bool in_town = true;

    while (in_town) {
        clear_screen();
        printf("\n=== TOWN ===\n");
        printf("Welcome to the town!\n\n");

        const char* town_options[] = {
            "Inn - Rest and recover (Cost: 50 Gold)",
            "Item Shop - Buy and sell items",
            "Tavern - Talk to locals",
            "Leave Town"
        };

        int8_t choice = cursor_menu("TOWN", town_options, 4);

        switch (choice) {
            case 0: // Inn
                handle_inn();
                break;
            case 1: // Item Shop
                handle_item_shop();
                break;
            case 2: // Tavern
                handle_tavern();
                break;
            case 3: // Leave Town
            case -1: // Cancelled
                in_town = false;
                break;
        }
    }
}

void handle_inn(void) {
    clear_screen();
    printf("\n=== INN ===\n");
    printf("Welcome to the inn!\n\n");
    printf("Rest and recover all HP/MP for 50 Gold?\n");
    printf("Current Gold: %d\n\n", g_game_state.gold);

    if (g_game_state.gold < 50) {
        printf("You don't have enough gold!\n");
        input_wait_for_key();
        return;
    }

    printf("Press Z/Enter to confirm, X/Esc to cancel\n");

    InputButton confirm = INPUT_NONE;
    while (confirm == INPUT_NONE) {
        confirm = input_get_key();
    }

    if (confirm == INPUT_A || confirm == INPUT_START) {
        g_game_state.gold -= 50;

        // Heal all party members
        for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
            PartyMember* member = &g_game_state.party->members[i];
            member->stats.current_hp = member->stats.max_hp;
            member->stats.current_mp = member->stats.max_mp;
        }

        printf("\nYou rest at the inn. Your party is fully recovered!\n");
        input_wait_for_key();
    }
}

void handle_item_shop(void) {
    clear_screen();
    printf("\n=== ITEM SHOP ===\n");
    printf("Item shop is not yet implemented.\n");
    printf("(Coming soon: Buy and sell consumable items)\n");
    input_wait_for_key();
}

void handle_tavern(void) {
    clear_screen();
    printf("\n=== TAVERN ===\n");
    printf("The tavern is quiet today.\n");
    printf("(Coming soon: Talk to NPCs and receive quests)\n");
    input_wait_for_key();
}

void handle_dungeon_selection(void) {
    bool in_dungeon_select = true;

    while (in_dungeon_select) {
        clear_screen();
        printf("\n=== DUNGEON SELECTION ===\n");
        printf("Key Items Collected: %d/4\n\n", __builtin_popcount(g_game_state.key_items_collected));

        // Build menu options
        const char* menu_options[12]; // Max: town + 4 dungeons + final + separator + inventory + party + save + load + quit
        uint8_t option_count = 0;
        int8_t dungeon_indices[6]; // Map menu index to dungeon index

        // Add Town option first
        menu_options[option_count++] = "Town";

        // Add separator
        menu_options[option_count++] = "---";

        // Add dungeons (indices will be offset by 2 due to Town and separator)
        uint8_t dungeon_option_start = option_count;
        for (int i = 0; i < MAX_DUNGEONS; i++) {
            static char dungeon_labels[4][50];
            if (g_game_state.dungeons_completed[i]) {
                snprintf(dungeon_labels[i], 50, "%s [COMPLETED]", dungeon_names[i]);
            } else {
                snprintf(dungeon_labels[i], 50, "%s", dungeon_names[i]);
            }
            menu_options[option_count] = dungeon_labels[i];
            dungeon_indices[option_count - dungeon_option_start] = i;
            option_count++;
        }

        // Add final dungeon if unlocked
        static char final_label[50];
        if (is_final_dungeon_unlocked()) {
            snprintf(final_label, 50, "%s [FINAL DUNGEON]", dungeon_names[MAX_DUNGEONS]);
            menu_options[option_count] = final_label;
            dungeon_indices[option_count - dungeon_option_start] = MAX_DUNGEONS;
            option_count++;
        }

        // Add menu options
        menu_options[option_count++] = "---";
        menu_options[option_count++] = "Inventory";
        menu_options[option_count++] = "Party Status";
        menu_options[option_count++] = "Save Game";
        menu_options[option_count++] = "Load Game";
        menu_options[option_count++] = "Quit Game";

        int8_t choice = cursor_menu("DUNGEON SELECTION", menu_options, option_count);

        // Handle Town selection
        if (choice == 0) {
            // Town selected - call town handler
            handle_town();
            continue;
        }

        // Skip separator at index 1
        // Dungeons start at index 2

        // Determine which dungeon was selected (if any)
        // Account for Town (index 0) and separator (index 1) - dungeons start at index 2
        uint8_t dungeon_start_index = 2;
        uint8_t dungeon_menu_count = is_final_dungeon_unlocked() ? MAX_DUNGEONS + 1 : MAX_DUNGEONS;

        if (choice >= dungeon_start_index && choice < dungeon_start_index + dungeon_menu_count) {
            // A dungeon was selected
            int dungeon_idx = dungeon_indices[choice - dungeon_start_index];

            // Initialize dungeon if needed
            if (!g_game_state.dungeon_initialized[dungeon_idx]) {
                uint8_t floor_count = (dungeon_idx == MAX_DUNGEONS) ? 5 : 3;
                dungeon_init(&g_game_state.dungeons[dungeon_idx], dungeon_idx,
                           dungeon_names[dungeon_idx], floor_count);
                g_game_state.dungeon_initialized[dungeon_idx] = true;
            }

            g_game_state.current_dungeon_index = dungeon_idx;
            game_state_change(STATE_DUNGEON_EXPLORE);
            in_dungeon_select = false;
        } else if (choice == dungeon_start_index + dungeon_menu_count + 1) {
            // Inventory (skip separator)
            handle_inventory_menu();
        } else if (choice == dungeon_start_index + dungeon_menu_count + 2) {
            // Party Status
            clear_screen();
            display_party_status();
            input_wait_for_key();
        } else if (choice == dungeon_start_index + dungeon_menu_count + 3) {
            // Save Game
            handle_save_menu();
        } else if (choice == dungeon_start_index + dungeon_menu_count + 4) {
            // Load Game
            handle_load_menu();
        } else if (choice == dungeon_start_index + dungeon_menu_count + 5) {
            // Quit Game
            printf("\nReally quit? (Press Z/Enter to confirm, X/Esc to cancel)\n");
            InputButton confirm = INPUT_NONE;
            while (confirm == INPUT_NONE) {
                confirm = input_get_key();
            }
            if (confirm == INPUT_A || confirm == INPUT_START) {
                game_state_change(STATE_GAME_OVER);
                in_dungeon_select = false;
            }
        } else {
            // Cancelled (pressed X/Esc at top level)
            // Just loop back to menu
        }
    }
}

void handle_dungeon_exploration(void) {
    Dungeon* current_dungeon = &g_game_state.dungeons[g_game_state.current_dungeon_index];

    if (!g_game_state.dungeon_initialized[g_game_state.current_dungeon_index]) {
        game_state_change(STATE_DUNGEON_SELECT);
        return;
    }

    // Flush input buffer to prevent leftover newlines from scanf triggering INPUT_START
    input_flush_buffer();

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
            case INPUT_START:
				// Open dungeon menu (cursor-based, no scanf!)
				{
					bool in_dungeon_menu = true;
					while (in_dungeon_menu) {
						const char* menu_options[] = {
							"Camp",
							"Inventory",
							"Party Status",
							"Return to Dungeon"
						};

						int8_t choice = cursor_menu("DUNGEON MENU", menu_options, 4);

						if (choice == 0) {
							// Camp submenu - loop until user returns
							bool in_camp = true;
							while (in_camp) {
								const char* camp_options[] = {
									"View Party Status",
									"Use Tent",
									"Save Game",
									"Return"
								};

								int8_t camp_choice = cursor_menu("CAMP", camp_options, 4);

								if (camp_choice == 0) {
									// View party status
									clear_screen();
									display_party_status();
									input_wait_for_key();
								} else if (camp_choice == 1) {
									// Use tent
									int8_t tent_index = inventory_find_item(g_game_state.inventory, ITEM_TENT);
									if (tent_index >= 0) {
										party_heal_all(g_game_state.party);
										inventory_remove_item(g_game_state.inventory, tent_index, 1);
										printf("\nParty is fully rested!\n");
										input_wait_for_key();
									} else {
										printf("\nNo tents available!\n");
										input_wait_for_key();
									}
								} else if (camp_choice == 2) {
									// Save game - still uses scanf for now
									handle_save_menu();
								} else {
									// camp_choice == 3 (Return) or -1 (cancelled)
									in_camp = false;
								}
							}
							// After exiting camp, loop back to dungeon menu
						} else if (choice == 1) {
							// Inventory - still uses scanf for now
							handle_inventory_menu();
							// After inventory, loop back to dungeon menu
						} else if (choice == 2) {
							// Party Status from dungeon menu
							clear_screen();
							display_party_status();
							input_wait_for_key();
							// Loop back to dungeon menu
						} else {
							// choice == 3 (Return to Dungeon) or -1 (cancelled)
							in_dungeon_menu = false;
						}
					}
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

        // Show whose turn it is
        display_battle_turn_indicator(current_member->name);

		// Build action menu (inline, no screen clear)
		const char* action_options[] = {
			"Attack",
			"Skill/Magic",
			"Item",
			"Defend",
			"Flee"
		};

		// Display action menu inline
		printf("\n=== BATTLE ACTION ===\n\n");
		uint8_t action_cursor = 0;
		int8_t action_choice = -1;

		while (action_choice == -1) {
			// Show menu options
			for (uint8_t i = 0; i < 5; i++) {
				if (i == action_cursor) {
					printf("> %s\n", action_options[i]);
				} else {
					printf("  %s\n", action_options[i]);
				}
			}
			printf("\nW/S=Move, Enter/Z=Select\n");

			InputButton input = INPUT_NONE;
			while (input == INPUT_NONE) {
				input = input_get_key();
			}

			switch (input) {
				case INPUT_UP:
					if (action_cursor > 0) {
						action_cursor--;
						// Redraw entire battle screen
						clear_screen();
						display_battle_scene();
						display_battle_turn_indicator(current_member->name);
						printf("\n=== BATTLE ACTION ===\n\n");
					}
					break;
				case INPUT_DOWN:
					if (action_cursor < 4) {
						action_cursor++;
						// Redraw entire battle screen
						clear_screen();
						display_battle_scene();
						display_battle_turn_indicator(current_member->name);
						printf("\n=== BATTLE ACTION ===\n\n");
					}
					break;
				case INPUT_A: // Z
				case INPUT_START: // Enter
					action_choice = action_cursor;
					break;
				default:
					break;
			}
		}

		if (action_choice == -1) {
			// Cancelled - shouldn't happen in battle, just continue
			continue;
		}

		BattleAction action;
		action.actor_index = current_member_index;

		switch (action_choice) {
			case 0: // Attack
				action.type = ACTION_ATTACK;

				// Select target
				if (g_battle_state.is_boss_battle) {
					action.target_index = 0;
				} else {
					// Build target menu with alive enemies (inline display)
					static char enemy_labels[MAX_ENEMIES][50];
					uint8_t enemy_indices[MAX_ENEMIES];
					uint8_t alive_count = 0;

					for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
						if (g_battle_state.enemies[i].is_alive) {
							snprintf(enemy_labels[alive_count], 50, "%s (HP: %d)",
									 g_battle_state.enemies[i].name,
									 g_battle_state.enemies[i].current_hp);
							enemy_indices[alive_count] = i;
							alive_count++;
						}
					}

					// Inline target selection
					printf("\n=== SELECT TARGET ===\n\n");
					uint8_t target_cursor = 0;
					int8_t target_choice = -1;

					while (target_choice == -1) {
						for (uint8_t i = 0; i < alive_count; i++) {
							if (i == target_cursor) {
								printf("> %s\n", enemy_labels[i]);
							} else {
								printf("  %s\n", enemy_labels[i]);
							}
						}
						printf("\nW/S=Move, Enter/Z=Select, X/Esc=Cancel\n");

						InputButton input = INPUT_NONE;
						while (input == INPUT_NONE) {
							input = input_get_key();
						}

						switch (input) {
							case INPUT_UP:
								if (target_cursor > 0) {
									target_cursor--;
									clear_screen();
									display_battle_scene();
									display_battle_turn_indicator(current_member->name);
									printf("\n=== SELECT TARGET ===\n\n");
								}
								break;
							case INPUT_DOWN:
								if (target_cursor < alive_count - 1) {
									target_cursor++;
									clear_screen();
									display_battle_scene();
									display_battle_turn_indicator(current_member->name);
									printf("\n=== SELECT TARGET ===\n\n");
								}
								break;
							case INPUT_A:
							case INPUT_START:
								target_choice = target_cursor;
								break;
							case INPUT_B:
								// Cancelled - go back to action menu
								target_choice = -2;
								break;
							default:
								break;
						}
					}

					if (target_choice == -2) {
						// User cancelled, go back to battle action selection
						continue;
					}

					action.target_index = enemy_indices[target_choice];
				}

				battle_process_action(&action);
				input_wait_for_key();
				break;
				
			case 1: // Skill/Magic
				if (current_member->skill_count == 0) {
					printf("%s has no skills!\n", current_member->name);
					input_wait_for_key();
					continue;
				}

				// Build skill menu (inline display)
				static char skill_labels[MAX_SKILLS][80];

				for (uint8_t i = 0; i < current_member->skill_count; i++) {
					const Skill* skill = get_skill_data(current_member->skills[i]);
					if (skill) {
						snprintf(skill_labels[i], 80, "%s (MP: %d) - %s",
								 skill->name, skill->mp_cost, skill->description);
					}
				}

				// Inline skill selection
				printf("\n=== SELECT SKILL === (MP: %d/%d)\n\n",
					   current_member->stats.current_mp, current_member->stats.max_mp);
				uint8_t skill_cursor = 0;
				int8_t skill_choice = -1;

				while (skill_choice == -1) {
					for (uint8_t i = 0; i < current_member->skill_count; i++) {
						if (i == skill_cursor) {
							printf("> %s\n", skill_labels[i]);
						} else {
							printf("  %s\n", skill_labels[i]);
						}
					}
					printf("\nW/S=Move, Enter/Z=Select, X/Esc=Cancel\n");

					InputButton input = INPUT_NONE;
					while (input == INPUT_NONE) {
						input = input_get_key();
					}

					switch (input) {
						case INPUT_UP:
							if (skill_cursor > 0) {
								skill_cursor--;
								clear_screen();
								display_battle_scene();
								display_battle_turn_indicator(current_member->name);
								printf("\n=== SELECT SKILL === (MP: %d/%d)\n\n",
									   current_member->stats.current_mp, current_member->stats.max_mp);
							}
							break;
						case INPUT_DOWN:
							if (skill_cursor < current_member->skill_count - 1) {
								skill_cursor++;
								clear_screen();
								display_battle_scene();
								display_battle_turn_indicator(current_member->name);
								printf("\n=== SELECT SKILL === (MP: %d/%d)\n\n",
									   current_member->stats.current_mp, current_member->stats.max_mp);
							}
							break;
						case INPUT_A:
						case INPUT_START:
							skill_choice = skill_cursor;
							break;
						case INPUT_B:
							// Cancelled
							skill_choice = -2;
							break;
						default:
							break;
					}
				}

				if (skill_choice == -2) {
					// User cancelled, go back to action menu
					continue;
				}

				uint8_t skill_id = current_member->skills[skill_choice];
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
							// Build enemy target menu (inline)
							static char enemy_skill_labels[MAX_ENEMIES][50];
							uint8_t enemy_skill_indices[MAX_ENEMIES];
							uint8_t alive_count = 0;

							for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
								if (g_battle_state.enemies[i].is_alive) {
									snprintf(enemy_skill_labels[alive_count], 50, "%s (HP: %d)",
											 g_battle_state.enemies[i].name,
											 g_battle_state.enemies[i].current_hp);
									enemy_skill_indices[alive_count] = i;
									alive_count++;
								}
							}

							printf("\n=== SELECT TARGET ===\n\n");
							uint8_t target_cursor = 0;
							int8_t target_choice = -1;

							while (target_choice == -1) {
								for (uint8_t i = 0; i < alive_count; i++) {
									if (i == target_cursor) {
										printf("> %s\n", enemy_skill_labels[i]);
									} else {
										printf("  %s\n", enemy_skill_labels[i]);
									}
								}
								printf("\nW/S=Move, Enter/Z=Select, X/Esc=Cancel\n");

								InputButton input = INPUT_NONE;
								while (input == INPUT_NONE) {
									input = input_get_key();
								}

								switch (input) {
									case INPUT_UP:
										if (target_cursor > 0) {
											target_cursor--;
											clear_screen();
											display_battle_scene();
											display_battle_turn_indicator(current_member->name);
											printf("\n=== SELECT TARGET ===\n\n");
										}
										break;
									case INPUT_DOWN:
										if (target_cursor < alive_count - 1) {
											target_cursor++;
											clear_screen();
											display_battle_scene();
											display_battle_turn_indicator(current_member->name);
											printf("\n=== SELECT TARGET ===\n\n");
										}
										break;
									case INPUT_A:
									case INPUT_START:
										target_choice = target_cursor;
										break;
									case INPUT_B:
										target_choice = -2;
										break;
									default:
										break;
								}
							}

							if (target_choice == -2) {
								continue;
							}

							action.target_index = enemy_skill_indices[target_choice];
						}
					}
				} else {
					// Select ally target
					if (selected_skill->target_all) {
						action.target_index = 0; // Doesn't matter for AoE
					} else {
						// Build party member target menu (inline)
						static char party_labels[MAX_PARTY_SIZE][50];

						for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
							PartyMember* member = &g_game_state.party->members[i];
							snprintf(party_labels[i], 50, "%s (HP: %d/%d)",
									 member->name,
									 member->stats.current_hp,
									 member->stats.max_hp);
						}

						printf("\n=== SELECT TARGET ===\n\n");
						uint8_t target_cursor = 0;
						int8_t target_choice = -1;

						while (target_choice == -1) {
							for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
								if (i == target_cursor) {
									printf("> %s\n", party_labels[i]);
								} else {
									printf("  %s\n", party_labels[i]);
								}
							}
							printf("\nW/S=Move, Enter/Z=Select, X/Esc=Cancel\n");

							InputButton input = INPUT_NONE;
							while (input == INPUT_NONE) {
								input = input_get_key();
							}

							switch (input) {
								case INPUT_UP:
									if (target_cursor > 0) {
										target_cursor--;
										clear_screen();
										display_battle_scene();
										display_battle_turn_indicator(current_member->name);
										printf("\n=== SELECT TARGET ===\n\n");
									}
									break;
								case INPUT_DOWN:
									if (target_cursor < g_game_state.party->member_count - 1) {
										target_cursor++;
										clear_screen();
										display_battle_scene();
										display_battle_turn_indicator(current_member->name);
										printf("\n=== SELECT TARGET ===\n\n");
									}
									break;
								case INPUT_A:
								case INPUT_START:
									target_choice = target_cursor;
									break;
								case INPUT_B:
									// Cancelled - target self
									target_choice = -2;
									break;
								default:
									break;
							}
						}

						if (target_choice == -2) {
							action.target_index = current_member_index;
						} else {
							action.target_index = target_choice;
						}
					}
				}
				
				battle_process_action(&action);
				input_wait_for_key();
				break;
				
			case 2: // Item
				if (g_game_state.inventory->item_count == 0) {
					printf("No items!\n");
					input_wait_for_key();
					continue;
				}

				// Build item menu (inline)
				static char item_labels[MAX_INVENTORY_ITEMS][50];

				for (uint8_t i = 0; i < g_game_state.inventory->item_count; i++) {
					Item* item = &g_game_state.inventory->items[i];
					snprintf(item_labels[i], 50, "%s x%d", item->name, item->quantity);
				}

				printf("\n=== SELECT ITEM ===\n\n");
				uint8_t item_cursor = 0;
				int8_t item_choice = -1;

				while (item_choice == -1) {
					for (uint8_t i = 0; i < g_game_state.inventory->item_count; i++) {
						if (i == item_cursor) {
							printf("> %s\n", item_labels[i]);
						} else {
							printf("  %s\n", item_labels[i]);
						}
					}
					printf("\nW/S=Move, Enter/Z=Select, X/Esc=Cancel\n");

					InputButton input = INPUT_NONE;
					while (input == INPUT_NONE) {
						input = input_get_key();
					}

					switch (input) {
						case INPUT_UP:
							if (item_cursor > 0) {
								item_cursor--;
								clear_screen();
								display_battle_scene();
								display_battle_turn_indicator(current_member->name);
								printf("\n=== SELECT ITEM ===\n\n");
							}
							break;
						case INPUT_DOWN:
							if (item_cursor < g_game_state.inventory->item_count - 1) {
								item_cursor++;
								clear_screen();
								display_battle_scene();
								display_battle_turn_indicator(current_member->name);
								printf("\n=== SELECT ITEM ===\n\n");
							}
							break;
						case INPUT_A:
						case INPUT_START:
							item_choice = item_cursor;
							break;
						case INPUT_B:
							item_choice = -2;
							break;
						default:
							break;
					}
				}

				if (item_choice == -2) {
					// Cancelled
					continue;
				}

				// Select party member to use item on (inline)
				static char member_labels[MAX_PARTY_SIZE][50];

				for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
					PartyMember* member = &g_game_state.party->members[i];
					snprintf(member_labels[i], 50, "%s (HP: %d/%d)",
							 member->name,
							 member->stats.current_hp,
							 member->stats.max_hp);
				}

				printf("\n=== USE ON ===\n\n");
				uint8_t member_cursor = 0;
				int8_t member_choice = -1;

				while (member_choice == -1) {
					for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
						if (i == member_cursor) {
							printf("> %s\n", member_labels[i]);
						} else {
							printf("  %s\n", member_labels[i]);
						}
					}
					printf("\nW/S=Move, Enter/Z=Select, X/Esc=Cancel\n");

					InputButton input = INPUT_NONE;
					while (input == INPUT_NONE) {
						input = input_get_key();
					}

					switch (input) {
						case INPUT_UP:
							if (member_cursor > 0) {
								member_cursor--;
								clear_screen();
								display_battle_scene();
								display_battle_turn_indicator(current_member->name);
								printf("\n=== USE ON ===\n\n");
							}
							break;
						case INPUT_DOWN:
							if (member_cursor < g_game_state.party->member_count - 1) {
								member_cursor++;
								clear_screen();
								display_battle_scene();
								display_battle_turn_indicator(current_member->name);
								printf("\n=== USE ON ===\n\n");
							}
							break;
						case INPUT_A:
						case INPUT_START:
							member_choice = member_cursor;
							break;
						case INPUT_B:
							member_choice = -2;
							break;
						default:
							break;
					}
				}

				if (member_choice != -2) {
					inventory_use_item(g_game_state.inventory, item_choice, member_choice);
				}

				// Using item doesn't advance turn in this implementation
				input_wait_for_key();
				continue; // Don't advance turn
				
			case 3: // Defend
				action.type = ACTION_DEFEND;
				battle_process_action(&action);
				input_wait_for_key();
				break;

			case 4: // Flee
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
        if (scanf("%d", &choice) != 1) {
            // Clear invalid input
            input_flush_buffer();
            continue;
        }

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
                input_flush_buffer();
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
                input_flush_buffer();
                break;

            case 4:
                printf("[DEBUG] Exiting inventory (choice=4)\n");
                in_inventory = false;
                game_state_change(STATE_DUNGEON_EXPLORE);
                break;

            default:
                printf("Invalid choice!\n");
                input_wait_for_key();
                input_flush_buffer();
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
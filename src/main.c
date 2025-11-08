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
#include <string.h>
#include <time.h>

void handle_party_selection(void);
void handle_dungeon_selection(void);
void handle_dungeon_exploration(void);
void handle_battle_phase(void);
void handle_inventory_menu(void);
void handle_save_menu(void);
void handle_load_menu(void);
void handle_town(void);
void handle_inn(void);
void handle_item_shop(void);
void handle_shop_buy(void);
void handle_shop_sell(void);
void handle_equipment_shop(void);
void handle_equipment_buy(void);
void handle_equipment_sell(void);
void handle_equipment_management(void);
void handle_tavern(void);

int main(void) {
    // Initialize game
    game_state_init();
    random_seed((uint32_t)time(NULL));
    
    printf("Welcome to Dungeon Quest RPG!\n");
    printf("A test scenario for our GameBoy RPG\n\n");
    input_wait_for_key();

    const char* title_options[] = {"New Game", "Load Game"};
    int8_t title_choice = cursor_menu("TITLE SCREEN", title_options, 2);

    // If cancelled, default to New Game
    if (title_choice < 0) title_choice = 0;

    bool game_loaded = false;

    if (title_choice == 1) {
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
				input_wait_for_key();

				const char* restart_options[] = {"Return to Title", "Quit Game"};
				int8_t restart = cursor_menu("GAME OVER", restart_options, 2);

				if (restart == 0) {
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
    
    g_game_state.party = party_create();

    for (int i = 0; i < MAX_PARTY_SIZE; i++) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "SELECT JOB FOR MEMBER %d", i + 1);

        // Job selection using cursor menu
        int8_t job_choice = cursor_menu(prompt, job_names, MAX_JOB_TYPES);

        // If cancelled, default to Knight
        if (job_choice < 0) job_choice = 0;

        // Name entry using virtual keyboard
        char name[MAX_NAME_LENGTH];
        snprintf(prompt, sizeof(prompt), "NAME FOR MEMBER %d", i + 1);

        if (!virtual_keyboard(prompt, name, MAX_NAME_LENGTH)) {
            // If cancelled, use default name
            snprintf(name, MAX_NAME_LENGTH, "%s%d", job_names[job_choice], i + 1);
        }

        party_add_member(g_game_state.party, (JobType)job_choice, name);
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
            "Item Shop - Buy and sell consumables",
            "Equipment Shop - Buy and sell equipment",
            "Tavern - Talk to locals",
            "Leave Town"
        };

        int8_t choice = cursor_menu("TOWN", town_options, 5);

        switch (choice) {
            case 0: // Inn
                handle_inn();
                break;
            case 1: // Item Shop
                handle_item_shop();
                break;
            case 2: // Equipment Shop
                handle_equipment_shop();
                break;
            case 3: // Tavern
                handle_tavern();
                break;
            case 4: // Leave Town
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

// Shop item data structure
typedef struct {
    uint8_t item_id;
    const char* name;
    uint16_t buy_price;
    uint16_t sell_price;
    const char* description;
} ShopItem;

// Shop inventory (static data)
static const ShopItem shop_items[] = {
    {ITEM_POTION, "Potion", 30, 15, "Restores 50 HP"},
    {ITEM_HI_POTION, "Hi-Potion", 150, 75, "Restores 150 HP"},
    {ITEM_ETHER, "Ether", 100, 50, "Restores 30 MP"},
    {ITEM_ELIXIR, "Elixir", 1000, 500, "Fully restores HP/MP"},
    {ITEM_ANTIDOTE, "Antidote", 30, 15, "Cures poison"},
    {ITEM_TENT, "Tent", 150, 75, "Fully heals party"}
};
#define SHOP_ITEM_COUNT 6

void handle_item_shop(void) {
    bool in_shop = true;

    while (in_shop) {
        clear_screen();
        printf("\n=== ITEM SHOP ===\n");
        printf("Welcome! What can I do for you?\n");
        printf("Gold: %d\n\n", g_game_state.gold);

        const char* shop_options[] = {
            "Buy Items",
            "Sell Items",
            "Leave Shop"
        };

        int8_t choice = cursor_menu("ITEM SHOP", shop_options, 3);

        switch (choice) {
            case 0: // Buy
                handle_shop_buy();
                break;
            case 1: // Sell
                handle_shop_sell();
                break;
            case 2: // Leave
            case -1: // Cancelled
                in_shop = false;
                break;
        }
    }
}

void handle_shop_buy(void) {
    bool buying = true;

    while (buying) {
        clear_screen();
        printf("\n=== BUY ITEMS ===\n");
        printf("Gold: %d\n\n", g_game_state.gold);

        // Build buy menu with prices
        static char buy_options_strings[SHOP_ITEM_COUNT + 1][80];
        static const char* buy_options[SHOP_ITEM_COUNT + 1];

        for (uint8_t i = 0; i < SHOP_ITEM_COUNT; i++) {
            snprintf(buy_options_strings[i], 80, "%s - %d Gold (%s)",
                     shop_items[i].name,
                     shop_items[i].buy_price,
                     shop_items[i].description);
            buy_options[i] = buy_options_strings[i];
        }
        snprintf(buy_options_strings[SHOP_ITEM_COUNT], 80, "Back");
        buy_options[SHOP_ITEM_COUNT] = buy_options_strings[SHOP_ITEM_COUNT];

        int8_t choice = cursor_menu("BUY", buy_options, SHOP_ITEM_COUNT + 1);

        if (choice == -1 || choice == SHOP_ITEM_COUNT) {
            // Cancelled or Back
            buying = false;
        } else if (choice >= 0 && choice < SHOP_ITEM_COUNT) {
            // Item selected
            const ShopItem* selected = &shop_items[choice];

            // Check if player has enough gold
            if (g_game_state.gold < selected->buy_price) {
                printf("\nNot enough gold!\n");
                input_wait_for_key();
                continue;
            }

            // Check inventory space
            if (inventory_is_full(g_game_state.inventory)) {
                printf("\nInventory is full!\n");
                input_wait_for_key();
                continue;
            }

            // Ask for quantity
            clear_screen();
            printf("\n=== BUY %s ===\n", selected->name);
            printf("Price: %d Gold each\n", selected->buy_price);
            printf("You have: %d Gold\n\n", g_game_state.gold);

            uint16_t max_afford = g_game_state.gold / selected->buy_price;
            if (max_afford == 0) max_afford = 1; // At least show 1

            printf("How many? (Max: %d)\n", max_afford);
            printf("Press Z/Enter to buy 1, or X/Esc to cancel\n");

            // For now, just buy 1 at a time (cursor-based)
            // TODO: Add quantity selection interface later
            InputButton confirm = INPUT_NONE;
            while (confirm == INPUT_NONE) {
                confirm = input_get_key();
            }

            if (confirm == INPUT_A || confirm == INPUT_START) {
                // Buy 1
                uint8_t quantity = 1;
                uint16_t total_cost = selected->buy_price * quantity;

                g_game_state.gold -= total_cost;
                inventory_add_item(g_game_state.inventory, selected->item_id, quantity);

                printf("\nPurchased %d x %s for %d Gold!\n", quantity, selected->name, total_cost);
                printf("Remaining Gold: %d\n", g_game_state.gold);
                input_wait_for_key();
            }
        }
    }
}

void handle_shop_sell(void) {
    bool selling = true;

    while (selling) {
        clear_screen();
        printf("\n=== SELL ITEMS ===\n");
        printf("Gold: %d\n\n", g_game_state.gold);

        if (g_game_state.inventory->item_count == 0) {
            printf("You have no items to sell!\n");
            input_wait_for_key();
            return;
        }

        // Build sell menu from player's inventory
        static char sell_options_strings[MAX_INVENTORY_ITEMS + 1][80];
        static const char* sell_options[MAX_INVENTORY_ITEMS + 1];
        uint8_t sell_count = 0;

        for (uint8_t i = 0; i < g_game_state.inventory->item_count; i++) {
            Item* item = &g_game_state.inventory->items[i];

            // Find shop price
            uint16_t sell_price = 0;
            for (uint8_t j = 0; j < SHOP_ITEM_COUNT; j++) {
                if (shop_items[j].item_id == item->item_id) {
                    sell_price = shop_items[j].sell_price;
                    break;
                }
            }

            snprintf(sell_options_strings[sell_count], 80, "%s x%d - %d Gold each",
                     item->name, item->quantity, sell_price);
            sell_options[sell_count] = sell_options_strings[sell_count];
            sell_count++;
        }
        snprintf(sell_options_strings[sell_count], 80, "Back");
        sell_options[sell_count] = sell_options_strings[sell_count];

        int8_t choice = cursor_menu("SELL", sell_options, sell_count + 1);

        if (choice == -1 || choice == sell_count) {
            // Cancelled or Back
            selling = false;
        } else if (choice >= 0 && choice < sell_count) {
            // Item selected
            Item* selected_item = &g_game_state.inventory->items[choice];

            // Find sell price
            uint16_t sell_price = 0;
            for (uint8_t j = 0; j < SHOP_ITEM_COUNT; j++) {
                if (shop_items[j].item_id == selected_item->item_id) {
                    sell_price = shop_items[j].sell_price;
                    break;
                }
            }

            clear_screen();
            printf("\n=== SELL %s ===\n", selected_item->name);
            printf("Sell Price: %d Gold each\n", sell_price);
            printf("You have: %d\n\n", selected_item->quantity);

            printf("Sell 1 for %d Gold?\n", sell_price);
            printf("Press Z/Enter to confirm, or X/Esc to cancel\n");

            InputButton confirm = INPUT_NONE;
            while (confirm == INPUT_NONE) {
                confirm = input_get_key();
            }

            if (confirm == INPUT_A || confirm == INPUT_START) {
                // Sell 1
                g_game_state.gold += sell_price;
                inventory_remove_item(g_game_state.inventory, choice, 1);

                printf("\nSold %s for %d Gold!\n", selected_item->name, sell_price);
                printf("Total Gold: %d\n", g_game_state.gold);
                input_wait_for_key();
            }
        }
    }
}

// Equipment shop data structure
typedef struct {
    uint8_t equip_id;
    const char* name;
    uint16_t buy_price;
    uint16_t sell_price;
    const char* description;
    EquipmentType type;
} ShopEquipment;

// Equipment shop inventory (static data)
static const ShopEquipment shop_equipment[] = {
    // Weapons
    {0, "Dagger", 100, 50, "ATK+5 (All)", EQUIP_TYPE_WEAPON},
    {1, "Short Sword", 200, 100, "ATK+8 (All)", EQUIP_TYPE_WEAPON},
    {2, "Long Sword", 500, 250, "ATK+12 (Knight)", EQUIP_TYPE_WEAPON},
    {3, "Great Sword", 1200, 600, "ATK+18 (Knight)", EQUIP_TYPE_WEAPON},
    {4, "Staff", 150, 75, "ATK+4, INT+3 (All)", EQUIP_TYPE_WEAPON},
    {5, "Iron Staff", 400, 200, "ATK+7, INT+5 (All)", EQUIP_TYPE_WEAPON},
    {6, "Wooden Rod", 250, 125, "ATK+3, INT+5 (Mage/Sage)", EQUIP_TYPE_WEAPON},
    {7, "Iron Rod", 600, 300, "ATK+5, INT+8 (Mage/Sage)", EQUIP_TYPE_WEAPON},
    {8, "Nunchaku", 400, 200, "ATK+10 (Black Belt)", EQUIP_TYPE_WEAPON},
    {9, "Iron Nunchaku", 800, 400, "ATK+15 (Black Belt)", EQUIP_TYPE_WEAPON},

    // Armor
    {20, "Cloth Armor", 80, 40, "DEF+3 (All)", EQUIP_TYPE_ARMOR},
    {21, "Leather Armor", 180, 90, "DEF+6 (All)", EQUIP_TYPE_ARMOR},
    {22, "Chain Mail", 450, 225, "DEF+10 (Knight)", EQUIP_TYPE_ARMOR},
    {23, "Plate Mail", 1000, 500, "DEF+15 (Knight)", EQUIP_TYPE_ARMOR},
    {24, "Robe", 200, 100, "DEF+4, INT+2 (Mage/Sage/Priest)", EQUIP_TYPE_ARMOR},
    {25, "Silk Robe", 500, 250, "DEF+7, INT+4 (Mage/Sage/Priest)", EQUIP_TYPE_ARMOR},

    // Helmets
    {40, "Leather Cap", 60, 30, "DEF+2 (All)", EQUIP_TYPE_HELMET},
    {41, "Iron Helm", 300, 150, "DEF+5 (Knight)", EQUIP_TYPE_HELMET},
    {42, "Wizard Hat", 250, 125, "DEF+2, INT+3 (Mage/Sage)", EQUIP_TYPE_HELMET},

    // Accessories
    {60, "Power Ring", 400, 200, "ATK+3 (All)", EQUIP_TYPE_ACCESSORY},
    {61, "Defense Ring", 400, 200, "DEF+3 (All)", EQUIP_TYPE_ACCESSORY},
    {62, "Luck Ring", 400, 200, "AGI+3 (All)", EQUIP_TYPE_ACCESSORY},
};
#define SHOP_EQUIPMENT_COUNT 22

void handle_equipment_shop(void) {
    bool in_shop = true;

    while (in_shop) {
        clear_screen();
        printf("\n=== EQUIPMENT SHOP ===\n");
        printf("Welcome to the armory!\n");
        printf("Gold: %d\n\n", g_game_state.gold);

        const char* shop_options[] = {
            "Buy Equipment",
            "Sell Equipment",
            "Leave Shop"
        };

        int8_t choice = cursor_menu("EQUIPMENT SHOP", shop_options, 3);

        switch (choice) {
            case 0: // Buy
                handle_equipment_buy();
                break;
            case 1: // Sell
                handle_equipment_sell();
                break;
            case 2: // Leave
            case -1: // Cancelled
                in_shop = false;
                break;
        }
    }
}

void handle_equipment_buy(void) {
    bool buying = true;

    while (buying) {
        clear_screen();
        printf("\n=== BUY EQUIPMENT ===\n");
        printf("Gold: %d\n\n", g_game_state.gold);

        // Build category menu
        const char* category_options[] = {
            "Weapons",
            "Armor",
            "Helmets",
            "Accessories",
            "Back"
        };

        int8_t category = cursor_menu("SELECT CATEGORY", category_options, 5);

        if (category == -1 || category == 4) {
            // Cancelled or Back
            buying = false;
            continue;
        }

        // Filter equipment by category
        EquipmentType filter_type;
        switch (category) {
            case 0: filter_type = EQUIP_TYPE_WEAPON; break;
            case 1: filter_type = EQUIP_TYPE_ARMOR; break;
            case 2: filter_type = EQUIP_TYPE_HELMET; break;
            case 3: filter_type = EQUIP_TYPE_ACCESSORY; break;
            default: continue;
        }

        // Build filtered equipment list
        static char equip_options_strings[SHOP_EQUIPMENT_COUNT + 1][80];
        static const char* equip_options[SHOP_EQUIPMENT_COUNT + 1];
        uint8_t equip_indices[SHOP_EQUIPMENT_COUNT];
        uint8_t filtered_count = 0;

        for (uint8_t i = 0; i < SHOP_EQUIPMENT_COUNT; i++) {
            if (shop_equipment[i].type == filter_type) {
                snprintf(equip_options_strings[filtered_count], 80, "%s - %d Gold (%s)",
                         shop_equipment[i].name,
                         shop_equipment[i].buy_price,
                         shop_equipment[i].description);
                equip_options[filtered_count] = equip_options_strings[filtered_count];
                equip_indices[filtered_count] = i;
                filtered_count++;
            }
        }
        snprintf(equip_options_strings[filtered_count], 80, "Back");
        equip_options[filtered_count] = equip_options_strings[filtered_count];

        int8_t choice = cursor_menu("BUY EQUIPMENT", equip_options, filtered_count + 1);

        if (choice == -1 || choice == filtered_count) {
            // Cancelled or Back
            continue;
        }

        const ShopEquipment* selected = &shop_equipment[equip_indices[choice]];

        // Check if player has enough gold
        if (g_game_state.gold < selected->buy_price) {
            printf("\nNot enough gold!\n");
            input_wait_for_key();
            continue;
        }

        // Check equipment inventory space
        if (g_game_state.inventory->equipment_count >= MAX_EQUIPMENT_SLOTS) {
            printf("\nEquipment inventory is full!\n");
            input_wait_for_key();
            continue;
        }

        // Confirm purchase
        clear_screen();
        printf("\n=== BUY %s ===\n", selected->name);
        printf("Price: %d Gold\n", selected->buy_price);
        printf("You have: %d Gold\n", g_game_state.gold);
        printf("\n%s\n", selected->description);
        printf("\nPurchase for %d Gold?\n", selected->buy_price);
        printf("Press Z/Enter to confirm, or X/Esc to cancel\n");

        InputButton confirm = INPUT_NONE;
        while (confirm == INPUT_NONE) {
            confirm = input_get_key();
        }

        if (confirm == INPUT_A || confirm == INPUT_START) {
            g_game_state.gold -= selected->buy_price;
            uint8_t new_equip_idx = g_game_state.inventory->equipment_count;
            inventory_add_equipment(g_game_state.inventory, selected->equip_id);

            printf("\nPurchased %s for %d Gold!\n", selected->name, selected->buy_price);
            printf("Remaining Gold: %d\n", g_game_state.gold);

            // Offer to equip the item immediately
            printf("\nEquip %s now?\n", selected->name);
            printf("Press Z/Enter to equip, or X/Esc to skip\n");

            InputButton equip_confirm = INPUT_NONE;
            while (equip_confirm == INPUT_NONE) {
                equip_confirm = input_get_key();
            }

            if (equip_confirm == INPUT_A || equip_confirm == INPUT_START) {
                // Show party member selection
                clear_screen();
                printf("\n=== EQUIP %s ===\n\n", selected->name);

                static char member_equip_options[MAX_PARTY_SIZE + 1][80];
                static const char* member_equip_menu[MAX_PARTY_SIZE + 1];

                for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
                    PartyMember* member = &g_game_state.party->members[i];

                    // Check if this member can use this equipment
                    Item* new_item = &g_game_state.inventory->equipment[new_equip_idx];
                    bool can_equip = (new_item->usable_by_job == 0) ||
                                     (new_item->usable_by_job & (1 << member->job));

                    if (can_equip) {
                        snprintf(member_equip_options[i], 80, "%s - %s (Lv %d)",
                                 member->name, job_names[member->job], member->stats.level);
                    } else {
                        snprintf(member_equip_options[i], 80, "%s - %s (Lv %d) [CANNOT USE]",
                                 member->name, job_names[member->job], member->stats.level);
                    }
                    member_equip_menu[i] = member_equip_options[i];
                }
                snprintf(member_equip_options[g_game_state.party->member_count], 80, "Cancel");
                member_equip_menu[g_game_state.party->member_count] = member_equip_options[g_game_state.party->member_count];

                int8_t member_choice = cursor_menu("SELECT PARTY MEMBER", member_equip_menu, g_game_state.party->member_count + 1);

                if (member_choice >= 0 && member_choice < g_game_state.party->member_count) {
                    Item* new_item = &g_game_state.inventory->equipment[new_equip_idx];
                    PartyMember* selected_member = &g_game_state.party->members[member_choice];

                    // Check if member can use this equipment
                    if (new_item->usable_by_job != 0 && !(new_item->usable_by_job & (1 << selected_member->job))) {
                        printf("\n%s cannot equip %s!\n", selected_member->name, new_item->name);
                        input_wait_for_key();
                    } else {
                        // Equip the item
                        inventory_equip_item(g_game_state.inventory, new_equip_idx, member_choice);
                        input_wait_for_key();
                    }
                }
            }
        }
    }
}

void handle_equipment_sell(void) {
    bool selling = true;

    while (selling) {
        clear_screen();
        printf("\n=== SELL EQUIPMENT ===\n");
        printf("Gold: %d\n\n", g_game_state.gold);

        if (g_game_state.inventory->equipment_count == 0) {
            printf("You have no equipment to sell!\n");
            input_wait_for_key();
            return;
        }

        // Build sell menu from player's equipment inventory
        static char sell_options_strings[MAX_EQUIPMENT_SLOTS + 1][80];
        static const char* sell_options[MAX_EQUIPMENT_SLOTS + 1];
        uint8_t sell_count = 0;

        for (uint8_t i = 0; i < g_game_state.inventory->equipment_count; i++) {
            Item* equip = &g_game_state.inventory->equipment[i];

            // Find shop price
            uint16_t sell_price = 0;
            for (uint8_t j = 0; j < SHOP_EQUIPMENT_COUNT; j++) {
                if (shop_equipment[j].equip_id == equip->item_id) {
                    sell_price = shop_equipment[j].sell_price;
                    break;
                }
            }

            // Show equipped status
            const char* equipped_status = equip->is_equipped ? " [EQUIPPED]" : "";
            snprintf(sell_options_strings[sell_count], 80, "%s - %d Gold%s",
                     equip->name, sell_price, equipped_status);
            sell_options[sell_count] = sell_options_strings[sell_count];
            sell_count++;
        }
        snprintf(sell_options_strings[sell_count], 80, "Back");
        sell_options[sell_count] = sell_options_strings[sell_count];

        int8_t choice = cursor_menu("SELL EQUIPMENT", sell_options, sell_count + 1);

        if (choice == -1 || choice == sell_count) {
            // Cancelled or Back
            selling = false;
        } else if (choice >= 0 && choice < sell_count) {
            // Equipment selected
            Item* selected_equip = &g_game_state.inventory->equipment[choice];

            // Check if equipped
            if (selected_equip->is_equipped) {
                printf("\nCannot sell equipped items! Unequip it first.\n");
                input_wait_for_key();
                continue;
            }

            // Find sell price
            uint16_t sell_price = 0;
            for (uint8_t j = 0; j < SHOP_EQUIPMENT_COUNT; j++) {
                if (shop_equipment[j].equip_id == selected_equip->item_id) {
                    sell_price = shop_equipment[j].sell_price;
                    break;
                }
            }

            clear_screen();
            printf("\n=== SELL %s ===\n", selected_equip->name);
            printf("Sell Price: %d Gold\n\n", sell_price);

            printf("Sell for %d Gold?\n", sell_price);
            printf("Press Z/Enter to confirm, or X/Esc to cancel\n");

            InputButton confirm = INPUT_NONE;
            while (confirm == INPUT_NONE) {
                confirm = input_get_key();
            }

            if (confirm == INPUT_A || confirm == INPUT_START) {
                g_game_state.gold += sell_price;
                inventory_remove_equipment(g_game_state.inventory, choice);

                printf("\nSold %s for %d Gold!\n", selected_equip->name, sell_price);
                printf("Total Gold: %d\n", g_game_state.gold);
                input_wait_for_key();
            }
        }
    }
}

void handle_equipment_management(void) {
    bool managing = true;

    while (managing) {
        clear_screen();
        printf("\n=== EQUIPMENT MANAGEMENT ===\n\n");

        // Build party member selection menu
        static char member_options_strings[MAX_PARTY_SIZE + 1][80];
        static const char* member_options[MAX_PARTY_SIZE + 1];

        for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
            PartyMember* member = &g_game_state.party->members[i];
            snprintf(member_options_strings[i], 80, "%s - %s (Lv %d)",
                     member->name,
                     job_names[member->job],
                     member->stats.level);
            member_options[i] = member_options_strings[i];
        }
        snprintf(member_options_strings[g_game_state.party->member_count], 80, "Back");
        member_options[g_game_state.party->member_count] = member_options_strings[g_game_state.party->member_count];

        int8_t member_choice = cursor_menu("SELECT PARTY MEMBER", member_options, g_game_state.party->member_count + 1);

        if (member_choice == -1 || member_choice == g_game_state.party->member_count) {
            // Cancelled or Back
            managing = false;
            continue;
        }

        // Manage equipment for selected party member
        PartyMember* selected_member = &g_game_state.party->members[member_choice];
        bool managing_member = true;

        while (managing_member) {
            clear_screen();
            printf("\n=== %s's EQUIPMENT ===\n", selected_member->name);
            printf("%s - Level %d\n\n", job_names[selected_member->job], selected_member->stats.level);

            // Show current stats
            printf("Stats: ATK %d | DEF %d | INT %d | AGI %d\n\n",
                   character_get_total_attack(selected_member),
                   character_get_total_defense(selected_member),
                   character_get_total_intelligence(selected_member),
                   character_get_total_agility(selected_member));

            // Build equipment slot menu
            const char* slot_names[] = {"Weapon", "Armor", "Helmet", "Accessory"};
            static char slot_options_strings[5][80];
            static const char* slot_options[5];

            for (uint8_t slot = 0; slot < EQUIP_SLOT_COUNT; slot++) {
                uint8_t equip_idx = selected_member->equipped_items[slot];
                if (equip_idx != 0xFF && equip_idx < g_game_state.inventory->equipment_count) {
                    Item* equip = &g_game_state.inventory->equipment[equip_idx];
                    snprintf(slot_options_strings[slot], 80, "%s: %s", slot_names[slot], equip->name);
                } else {
                    snprintf(slot_options_strings[slot], 80, "%s: (None)", slot_names[slot]);
                }
                slot_options[slot] = slot_options_strings[slot];
            }
            snprintf(slot_options_strings[4], 80, "Back");
            slot_options[4] = slot_options_strings[4];

            int8_t slot_choice = cursor_menu("SELECT EQUIPMENT SLOT", slot_options, 5);

            if (slot_choice == -1 || slot_choice == 4) {
                // Cancelled or Back
                managing_member = false;
                continue;
            }

            EquipmentSlot selected_slot = (EquipmentSlot)slot_choice;

            // Build available equipment menu for this slot
            static char equip_list_strings[MAX_EQUIPMENT_SLOTS + 2][80];
            static const char* equip_list[MAX_EQUIPMENT_SLOTS + 2];
            uint8_t equip_indices[MAX_EQUIPMENT_SLOTS];
            uint8_t available_count = 0;
            uint8_t equip_count = 0;  // Separate counter for equip_indices

            // Add "Unequip" option if something is equipped
            uint8_t current_equip_idx = selected_member->equipped_items[selected_slot];
            if (current_equip_idx != 0xFF) {
                snprintf(equip_list_strings[0], 80, "(Unequip)");
                equip_list[0] = equip_list_strings[0];
                available_count = 1;
            }

            // Add available equipment for this slot
            for (uint8_t i = 0; i < g_game_state.inventory->equipment_count; i++) {
                Item* equip = &g_game_state.inventory->equipment[i];

                // Check if this equipment matches the slot type
                if ((EquipmentSlot)equip->equip_type != selected_slot) continue;

                // Check if already equipped
                if (equip->is_equipped) continue;

                // Check job restrictions
                if (equip->usable_by_job != 0 && !(equip->usable_by_job & (1 << selected_member->job))) {
                    // Show but mark as unusable
                    snprintf(equip_list_strings[available_count], 80, "%s [CANNOT USE]", equip->name);
                } else {
                    // Show with stat bonuses
                    char bonus_str[40] = "";
                    if (equip->attack_bonus > 0) snprintf(bonus_str + strlen(bonus_str), 40 - strlen(bonus_str), " ATK+%d", equip->attack_bonus);
                    if (equip->defense_bonus > 0) snprintf(bonus_str + strlen(bonus_str), 40 - strlen(bonus_str), " DEF+%d", equip->defense_bonus);
                    if (equip->intelligence_bonus > 0) snprintf(bonus_str + strlen(bonus_str), 40 - strlen(bonus_str), " INT+%d", equip->intelligence_bonus);
                    if (equip->agility_bonus > 0) snprintf(bonus_str + strlen(bonus_str), 40 - strlen(bonus_str), " AGI+%d", equip->agility_bonus);

                    snprintf(equip_list_strings[available_count], 80, "%s%s", equip->name, bonus_str);
                }

                equip_list[available_count] = equip_list_strings[available_count];
                equip_indices[equip_count] = i;  // Use equip_count instead of available_count
                available_count++;
                equip_count++;
            }

            snprintf(equip_list_strings[available_count], 80, "Back");
            equip_list[available_count] = equip_list_strings[available_count];

            int8_t equip_choice = cursor_menu("SELECT EQUIPMENT", equip_list, available_count + 1);

            if (equip_choice == -1 || equip_choice == available_count) {
                // Cancelled or Back
                continue;
            }

            // Handle unequip
            if (current_equip_idx != 0xFF && equip_choice == 0) {
                inventory_unequip_item(member_choice, selected_slot);
                printf("\nUnequipped!\n");
                input_wait_for_key();
                continue;
            }

            // Handle equip
            uint8_t offset = (current_equip_idx != 0xFF) ? 1 : 0;
            uint8_t inv_idx = equip_indices[equip_choice - offset];
            Item* selected_equip = &g_game_state.inventory->equipment[inv_idx];

            // Check job restrictions
            if (selected_equip->usable_by_job != 0 && !(selected_equip->usable_by_job & (1 << selected_member->job))) {
                printf("\n%s cannot equip %s!\n", selected_member->name, selected_equip->name);
                input_wait_for_key();
                continue;
            }

            // Show comparison screen
            clear_screen();
            printf("\n=== EQUIPMENT COMPARISON ===\n");
            printf("%s - %s\n\n", selected_member->name, job_names[selected_member->job]);

            // Get current stats
            uint8_t current_atk = character_get_total_attack(selected_member);
            uint8_t current_def = character_get_total_defense(selected_member);
            uint8_t current_int = character_get_total_intelligence(selected_member);
            uint8_t current_agi = character_get_total_agility(selected_member);

            // Calculate new stats (simulate equipping the new item)
            uint8_t new_atk = current_atk;
            uint8_t new_def = current_def;
            uint8_t new_int = current_int;
            uint8_t new_agi = current_agi;

            // Get current equipment in this slot
            Item* current_equip = NULL;
            if (current_equip_idx != 0xFF && current_equip_idx < g_game_state.inventory->equipment_count) {
                current_equip = &g_game_state.inventory->equipment[current_equip_idx];
                // Subtract current equipment bonuses
                new_atk -= current_equip->attack_bonus;
                new_def -= current_equip->defense_bonus;
                new_int -= current_equip->intelligence_bonus;
                new_agi -= current_equip->agility_bonus;
            }

            // Add new equipment bonuses
            new_atk += selected_equip->attack_bonus;
            new_def += selected_equip->defense_bonus;
            new_int += selected_equip->intelligence_bonus;
            new_agi += selected_equip->agility_bonus;

            // Display current equipment
            printf("Current %s:\n", slot_names[selected_slot]);
            if (current_equip) {
                printf("  %s", current_equip->name);
                if (current_equip->attack_bonus > 0) printf(" (ATK+%d)", current_equip->attack_bonus);
                if (current_equip->defense_bonus > 0) printf(" (DEF+%d)", current_equip->defense_bonus);
                if (current_equip->intelligence_bonus > 0) printf(" (INT+%d)", current_equip->intelligence_bonus);
                if (current_equip->agility_bonus > 0) printf(" (AGI+%d)", current_equip->agility_bonus);
                printf("\n");
            } else {
                printf("  (None)\n");
            }

            // Display new equipment
            printf("\nNew %s:\n", slot_names[selected_slot]);
            printf("  %s", selected_equip->name);
            if (selected_equip->attack_bonus > 0) printf(" (ATK+%d)", selected_equip->attack_bonus);
            if (selected_equip->defense_bonus > 0) printf(" (DEF+%d)", selected_equip->defense_bonus);
            if (selected_equip->intelligence_bonus > 0) printf(" (INT+%d)", selected_equip->intelligence_bonus);
            if (selected_equip->agility_bonus > 0) printf(" (AGI+%d)", selected_equip->agility_bonus);
            printf("\n\n");

            // Display stat changes
            printf("--- STAT CHANGES ---\n");

            // Attack
            printf("ATK: %d -> %d", current_atk, new_atk);
            if (new_atk > current_atk) {
                printf(" (+%d)", new_atk - current_atk);
            } else if (new_atk < current_atk) {
                printf(" (-%d)", current_atk - new_atk);
            }
            printf("\n");

            // Defense
            printf("DEF: %d -> %d", current_def, new_def);
            if (new_def > current_def) {
                printf(" (+%d)", new_def - current_def);
            } else if (new_def < current_def) {
                printf(" (-%d)", current_def - new_def);
            }
            printf("\n");

            // Intelligence
            printf("INT: %d -> %d", current_int, new_int);
            if (new_int > current_int) {
                printf(" (+%d)", new_int - current_int);
            } else if (new_int < current_int) {
                printf(" (-%d)", current_int - new_int);
            }
            printf("\n");

            // Agility
            printf("AGI: %d -> %d", current_agi, new_agi);
            if (new_agi > current_agi) {
                printf(" (+%d)", new_agi - current_agi);
            } else if (new_agi < current_agi) {
                printf(" (-%d)", current_agi - new_agi);
            }
            printf("\n\n");

            // Confirmation
            printf("Equip %s?\n", selected_equip->name);
            printf("Press Z/Enter to confirm, or X/Esc to cancel\n");

            InputButton confirm = INPUT_NONE;
            while (confirm == INPUT_NONE) {
                confirm = input_get_key();
            }

            if (confirm == INPUT_A || confirm == INPUT_START) {
                // Equip the item
                inventory_equip_item(g_game_state.inventory, inv_idx, member_choice);
                input_wait_for_key();
            }
        }
    }
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
            // Party Status submenu
            bool in_party_menu = true;
            while (in_party_menu) {
                const char* party_menu_options[] = {
                    "View Party Status",
                    "Manage Equipment",
                    "Back"
                };

                int8_t party_choice = cursor_menu("PARTY MENU", party_menu_options, 3);

                if (party_choice == 0) {
                    // View party status
                    clear_screen();
                    display_party_status();
                    input_wait_for_key();
                } else if (party_choice == 1) {
                    // Manage equipment
                    handle_equipment_management();
                } else {
                    // Back or cancelled
                    in_party_menu = false;
                }
            }
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
                            input_wait_for_key();
                            input_flush_buffer(); // Clear any lingering input
                            in_dungeon = false;
                            game_state_change(STATE_DUNGEON_SELECT);
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
                const char* exit_options[] = {"Return to Dungeon Selection", "Cancel"};
                int8_t confirm = cursor_menu("EXIT DUNGEON", exit_options, 2);

                if (confirm == 0) {
                    input_flush_buffer(); // Clear any lingering input from scanf
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
									"Manage Equipment",
									"Use Tent",
									"Save Game",
									"Return"
								};

								int8_t camp_choice = cursor_menu("CAMP", camp_options, 5);

								if (camp_choice == 0) {
									// View party status
									clear_screen();
									display_party_status();
									input_wait_for_key();
								} else if (camp_choice == 1) {
									// Manage equipment
									handle_equipment_management();
								} else if (camp_choice == 2) {
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
								} else if (camp_choice == 3) {
									// Save game - still uses scanf for now
									handle_save_menu();
								} else {
									// camp_choice == 4 (Return) or -1 (cancelled)
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
        
        const char* inv_options[] = {"Use Item", "View Party Status", "View Equipment Details", "Return"};
        int8_t choice = cursor_menu("INVENTORY", inv_options, 4);

        if (choice < 0 || choice == 3) {
            in_inventory = false;
            continue;
        }

        switch (choice) {
            case 0:
                if (g_game_state.inventory->item_count == 0) {
                    printf("No items to use!\n");
                    input_wait_for_key();
                    break;
                }
                
                // Build item list
                char* item_options[MAX_INVENTORY_ITEMS + 1];
                for (uint8_t i = 0; i < g_game_state.inventory->item_count; i++) {
                    Item* item = &g_game_state.inventory->items[i];
                    item_options[i] = malloc(64);
                    snprintf(item_options[i], 64, "%s x%d", item->name, item->quantity);
                }
                item_options[g_game_state.inventory->item_count] = "Cancel";

                int8_t item_choice = cursor_menu("SELECT ITEM", (const char**)item_options, g_game_state.inventory->item_count + 1);

                // Free allocated strings
                for (uint8_t i = 0; i < g_game_state.inventory->item_count; i++) {
                    free(item_options[i]);
                }

                if (item_choice >= 0 && item_choice < g_game_state.inventory->item_count) {
                    // Build member list
                    char* member_options[MAX_PARTY_SIZE + 1];
                    for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
                        PartyMember* member = &g_game_state.party->members[i];
                        member_options[i] = malloc(64);
                        snprintf(member_options[i], 64, "%s (HP:%d/%d MP:%d/%d)",
                                member->name, member->stats.current_hp, member->stats.max_hp,
                                member->stats.current_mp, member->stats.max_mp);
                    }
                    member_options[g_game_state.party->member_count] = "Cancel";

                    int8_t member_choice = cursor_menu("USE ON", (const char**)member_options, g_game_state.party->member_count + 1);

                    // Free allocated strings
                    for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
                        free(member_options[i]);
                    }

                    if (member_choice >= 0 && member_choice < g_game_state.party->member_count) {
                        inventory_use_item(g_game_state.inventory, item_choice, member_choice);
                    }
                }
                
                input_wait_for_key();
                break;
                
            case 1:
                display_party_status();
                input_wait_for_key();
                input_flush_buffer();
                break;

            case 2:
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
    const char* save_options[] = {"Slot 1", "Slot 2", "Suspend Save", "Cancel"};

    int8_t choice = cursor_menu("SAVE GAME", save_options, 4);

    if (choice < 0 || choice == 3) {
        return; // Cancelled or selected Cancel
    }

    if (choice == 2) {
        // Suspend save
        if (save_suspend_game()) {
            clear_screen();
            printf("\n Suspend save created! (Will be deleted when loaded)\n");
        } else {
            clear_screen();
            printf("\nFailed to create suspend save!\n");
        }
        input_wait_for_key();
    } else {
        // Slot 1 or Slot 2
        int slot = choice; // 0 or 1

        if (save_slot_exists(slot)) {
            const char* confirm_options[] = {"Yes", "No"};
            clear_screen();
            printf("\nOverwrite existing save in Slot %d?\n", slot + 1);
            int8_t confirm = cursor_menu("CONFIRM OVERWRITE", confirm_options, 2);

            if (confirm != 0) {
                return; // Not confirmed
            }
        }

        if (save_game_to_slot(slot)) {
            clear_screen();
            printf("\nGame saved to Slot %d successfully!\n", slot + 1);
        } else {
            clear_screen();
            printf("\nFailed to save game!\n");
        }
        input_wait_for_key();
    }
}

void handle_load_menu(void) {
    const char* load_options[] = {"Slot 1", "Slot 2", "Suspend Save", "Cancel"};

    int8_t choice = cursor_menu("LOAD GAME", load_options, 4);

    if (choice < 0 || choice == 3) {
        return; // Cancelled or selected Cancel
    }

    if (choice == 2) {
        // Load suspend save
        if (suspend_save_exists()) {
            if (load_suspend_game()) {
                clear_screen();
                printf("\nSuspend save loaded! (Save file deleted)\n");
                input_wait_for_key();
                // Game state is now loaded, will continue from loaded state
            } else {
                clear_screen();
                printf("\nFailed to load suspend save!\n");
                input_wait_for_key();
            }
        } else {
            clear_screen();
            printf("\nNo suspend save found!\n");
            input_wait_for_key();
        }
    } else {
        // Slot 1 or Slot 2
        int slot = choice; // 0 or 1

        if (save_slot_exists(slot)) {
            if (load_game_from_slot(slot)) {
                clear_screen();
                printf("\nGame loaded from Slot %d successfully!\n", slot + 1);
                input_wait_for_key();
                // Game state is now loaded, will continue from loaded state
            } else {
                clear_screen();
                printf("\nFailed to load game!\n");
                input_wait_for_key();
            }
        } else {
            clear_screen();
            printf("\nNo save data in Slot %d!\n", slot + 1);
            input_wait_for_key();
        }
    }
}
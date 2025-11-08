#include "game_state.h"
#include "party.h"
#include "dungeon.h"
#include "inventory.h"
#include "battle.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GameStateData g_game_state;

const char* dungeon_names[MAX_DUNGEONS + 1] = {
    "Cave of Earth",
    "Water Temple",
    "Volcano Keep",
    "Sky Tower",
    "Final Sanctum"
};

void game_state_init(void) {
    memset(&g_game_state, 0, sizeof(GameStateData));
    
    g_game_state.current_state = STATE_TITLE;
    g_game_state.party = NULL;
    g_game_state.inventory = NULL;
    g_game_state.key_items_collected = 0;
    g_game_state.current_dungeon_index = 0;
    g_game_state.final_dungeon_unlocked = false;
    g_game_state.game_time = 0;
    g_game_state.gold = 200; // Starting gold (increased from 100 for better early economy)
    
    // Initialize all dungeons as not initialized
    for (int i = 0; i <= MAX_DUNGEONS; i++) {
        g_game_state.dungeon_initialized[i] = false;
        if (i < MAX_DUNGEONS) {
            g_game_state.dungeons_completed[i] = false;
        }
    }
    
    // Initialize random seed
    random_seed(12345); // Use a fixed seed for testing, or use time() for random
    
    printf("=== DUNGEON QUEST RPG ===\n");
    printf("Game Initialized\n\n");
}

void game_state_update(void) {
    g_game_state.game_time++;
    
    switch (g_game_state.current_state) {
        case STATE_TITLE:
            display_text("Press START to begin");
            if (input_get_key() == INPUT_START) {
                game_state_change(STATE_PARTY_SELECT);
            }
            break;
            
        case STATE_PARTY_SELECT:
            // Handle party selection (implemented in main loop)
            break;
            
        case STATE_DUNGEON_SELECT:
            // Handle dungeon selection (implemented in main loop)
            break;
            
        case STATE_DUNGEON_EXPLORE:
            // Handle dungeon exploration (implemented in main loop)
            break;
            
        case STATE_BATTLE:
        case STATE_BOSS_BATTLE:
            // Handle battle (implemented in main loop)
            break;
            
        case STATE_INVENTORY:
            // Handle inventory management (implemented in main loop)
            break;
            
        case STATE_VICTORY:
            display_text("Congratulations! You have saved the world!");
            break;
            
        case STATE_GAME_OVER:
            display_text("Game Over");
            break;
    }
}

void game_state_cleanup(void) {
    if (g_game_state.party) {
        party_destroy(g_game_state.party);
    }

    if (g_game_state.inventory) {
        inventory_destroy(g_game_state.inventory);
    }

    // Dungeons are statically allocated, no cleanup needed

    printf("\nGame Cleanup Complete\n");
}

void game_state_change(GameState new_state) {
    printf("\n[State Change: %d -> %d]\n", g_game_state.current_state, new_state);
    g_game_state.current_state = new_state;
}

bool is_final_dungeon_unlocked(void) {
    // Check if all 4 key items have been collected
    uint8_t required_items = KEY_ITEM_EARTH_CRYSTAL | KEY_ITEM_WATER_CRYSTAL | 
                            KEY_ITEM_FIRE_CRYSTAL | KEY_ITEM_WIND_CRYSTAL;
    
    return (g_game_state.key_items_collected & required_items) == required_items;
}

void collect_key_item(KeyItem item) {
    g_game_state.key_items_collected |= item;
    
    const char* item_name = "Unknown";
    
    switch (item) {
        case KEY_ITEM_EARTH_CRYSTAL: item_name = "Earth Crystal"; break;
        case KEY_ITEM_WATER_CRYSTAL: item_name = "Water Crystal"; break;
        case KEY_ITEM_FIRE_CRYSTAL:  item_name = "Fire Crystal"; break;
        case KEY_ITEM_WIND_CRYSTAL:  item_name = "Wind Crystal"; break;
        default: break;
    }
    
    printf("\n*** Obtained %s! ***\n", item_name);
    
    if (is_final_dungeon_unlocked() && !g_game_state.final_dungeon_unlocked) {
        g_game_state.final_dungeon_unlocked = true;
        printf("\n*** The Final Sanctum has been unlocked! ***\n");
    }
}
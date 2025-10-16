#include "utils.h"
#include "game_state.h"
#include "party.h"
#include "battle.h"
#include "dungeon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

// External references
extern BattleState g_battle_state;
extern const char* job_names[MAX_JOB_TYPES];

// Simple LCG random number generator
static uint32_t random_seed_value = 12345;

void random_seed(uint32_t seed) {
    random_seed_value = seed;
}

uint8_t random_range(uint8_t min, uint8_t max) {
    if (min >= max) return min;
    
    random_seed_value = random_seed_value * 1103515245 + 12345;
    uint32_t range = max - min + 1;
    return min + ((random_seed_value / 65536) % range);
}

bool random_chance(uint8_t percentage) {
    if (percentage >= 100) return true;
    if (percentage == 0) return false;
    
    return random_range(0, 99) < percentage;
}

void safe_string_copy(char* dest, const char* src, uint8_t max_length) {
    if (!dest || !src) return;
    
    strncpy(dest, src, max_length - 1);
    dest[max_length - 1] = '\0';
}

void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

InputButton input_get_key(void) {
#ifdef _WIN32
    if (_kbhit()) {
        int ch = _getch();
        
        // Arrow keys are two-byte sequences on Windows
        if (ch == 224) {
            ch = _getch();
            switch (ch) {
                case 72: return INPUT_UP;
                case 80: return INPUT_DOWN;
                case 75: return INPUT_LEFT;
                case 77: return INPUT_RIGHT;
            }
        }
        
        switch (ch) {
            case 'w': case 'W': return INPUT_UP;
            case 's': case 'S': return INPUT_DOWN;
            case 'a': case 'A': return INPUT_LEFT;
            case 'd': case 'D': return INPUT_RIGHT;
            case 'z': case 'Z': case ' ': return INPUT_A;
            case 'x': case 'X': case 27: return INPUT_B; // ESC
            case 13: return INPUT_START; // Enter
            case '\t': return INPUT_SELECT;
        }
    }
#else
    // Non-blocking input for Unix-like systems
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if (ch != EOF) {
        switch (ch) {
            case 'w': case 'W': return INPUT_UP;
            case 's': case 'S': return INPUT_DOWN;
            case 'a': case 'A': return INPUT_LEFT;
            case 'd': case 'D': return INPUT_RIGHT;
            case 'z': case 'Z': case ' ': return INPUT_A;
            case 'x': case 'X': case 27: return INPUT_B;
            case '\n': return INPUT_START;
            case '\t': return INPUT_SELECT;
        }
    }
#endif
    
    return INPUT_NONE;
}

void input_wait_for_key(void) {
    printf("\nPress any key to continue...");
    fflush(stdout);
    
#ifdef _WIN32
    _getch();
#else
    getchar();
#endif
    
    printf("\n");
}

void display_text(const char* text) {
    printf("%s\n", text);
}

void display_menu(const char* title, const char** options, uint8_t option_count, uint8_t selected) {
    printf("\n=== %s ===\n", title);
    
    for (uint8_t i = 0; i < option_count; i++) {
        if (i == selected) {
            printf("> %s\n", options[i]);
        } else {
            printf("  %s\n", options[i]);
        }
    }
}

void display_party_status(void) {
    if (!g_game_state.party) return;
    
    printf("\n=== PARTY STATUS ===\n");
    printf("Gold: %d\n\n", g_game_state.gold);
    
    for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
        PartyMember* member = &g_game_state.party->members[i];
        printf("%s (Lv%d %s)\n", member->name, member->stats.level, job_names[member->job]);
        printf("  HP: %d/%d  MP: %d/%d\n", 
               member->stats.current_hp, member->stats.max_hp,
               member->stats.current_mp, member->stats.max_mp);
        
        if (member->status_effects != STATUS_NONE) {
            printf("  Status: ");
            if (member->status_effects & STATUS_POISON) printf("POISON ");
            if (member->status_effects & STATUS_PARALYSIS) printf("PARALYSIS ");
            if (member->status_effects & STATUS_SLEEP) printf("SLEEP ");
            if (member->status_effects & STATUS_DEAD) printf("DEAD ");
            printf("\n");
        }
    }
}

void display_battle_scene(void) {
    printf("\n=== BATTLE ===\n");
    
    // Display enemies
    if (g_battle_state.is_boss_battle && g_battle_state.boss) {
        printf("\nBOSS: %s\n", g_battle_state.boss->name);
        printf("HP: %d/%d\n", g_battle_state.boss->current_hp, g_battle_state.boss->max_hp);
    } else {
        printf("\nEnemies:\n");
        for (uint8_t i = 0; i < g_battle_state.enemy_count; i++) {
            Enemy* enemy = &g_battle_state.enemies[i];
            if (enemy->is_alive) {
                printf("%d. %s (HP: %d/%d)\n", i+1, enemy->name, 
                       enemy->current_hp, enemy->max_hp);
            } else {
                printf("%d. %s (DEFEATED)\n", i+1, enemy->name);
            }
        }
    }
    
    // Display party
    printf("\nYour Party:\n");
    for (uint8_t i = 0; i < g_game_state.party->member_count; i++) {
        PartyMember* member = &g_game_state.party->members[i];
        if (member->stats.current_hp > 0) {
            printf("%d. %s (HP: %d/%d)\n", i+1, member->name,
                   member->stats.current_hp, member->stats.max_hp);
        } else {
            printf("%d. %s (DOWN)\n", i+1, member->name);
        }
    }
}

void display_dungeon(void) {
    if (!g_game_state.dungeon_initialized[g_game_state.current_dungeon_index]) return;

    Dungeon* dungeon = &g_game_state.dungeons[g_game_state.current_dungeon_index];
    DungeonFloor* floor = &dungeon->floors[dungeon->current_floor];
    
    printf("\n=== %s - Floor %d ===\n", dungeon->name, dungeon->current_floor + 1);
    
    // Display map with better visibility
    int view_range = 5;
    
    for (int y = floor->player_y - view_range; y <= floor->player_y + view_range; y++) {
        for (int x = floor->player_x - view_range; x <= floor->player_x + view_range; x++) {
            // Player position
            if (x == floor->player_x && y == floor->player_y) {
                printf("@");
            }
            // Out of bounds
            else if (x < 0 || x >= DUNGEON_WIDTH || y < 0 || y >= DUNGEON_HEIGHT) {
                printf(" ");
            }
            // Show adjacent tiles even if not explored
            else {
                int dx = abs(x - floor->player_x);
                int dy = abs(y - floor->player_y);
                bool is_adjacent = (dx <= 1 && dy <= 1);
                
                // Show tile if explored OR adjacent to player
                if (floor->tiles[y][x].explored || is_adjacent) {
                    switch (floor->tiles[y][x].type) {
                        case TILE_WALL: 
                            printf("#"); 
                            break;
                        case TILE_FLOOR: 
                            printf("."); 
                            break;
                        case TILE_DOOR: 
                            printf("+"); 
                            break;
                        case TILE_STAIRS_UP: 
                            printf("<"); 
                            break;
                        case TILE_STAIRS_DOWN: 
                            printf(">"); 
                            break;
                        case TILE_TREASURE: 
                            printf("$"); 
                            break;
                        case TILE_BOSS_ROOM: 
                            printf("B"); 
                            break;
                        case TILE_ENTRANCE: 
                            printf("E"); 
                            break;
                        default: 
                            printf(" "); 
                            break;
                    }
                } else {
                    // Unexplored and not adjacent
                    printf("?");
                }
            }
        }
        printf("\n");
    }
    
    // Map legend
    printf("\nLegend: @ = You, # = Wall, . = Floor, > = Down, < = Up\n");
    printf("        $ = Treasure, B = Boss, E = Entrance\n");
    printf("\nControls: WASD=Move, Z=Interact, X=Back, Enter=Party, Tab=Inventory\n");
}
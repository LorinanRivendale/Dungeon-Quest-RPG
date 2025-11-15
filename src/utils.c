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

// GameBoy Pocket Color Palette (4 shades of gray)
// ANSI color codes: \033[XYm where X is style, Y is color
#define GB_COLOR_DARKEST   "\033[90m"  // Bright Black (Dark Gray) - for walls
#define GB_COLOR_DARK      "\033[37m"  // White (appears as light gray) - for floors
#define GB_COLOR_LIGHT     "\033[97m"  // Bright White - for interactive elements
#define GB_COLOR_LIGHTEST  "\033[1;97m" // Bold Bright White - for player
#define GB_COLOR_RESET     "\033[0m"   // Reset to default

// Unicode Tile Graphics
// These characters create a more visual representation similar to GameBoy tiles
#define TILE_CHAR_WALL      "█"  // Full block for walls
#define TILE_CHAR_FLOOR     "·"  // Middle dot for floor
#define TILE_CHAR_DOOR      "▒"  // Medium shade for doors
#define TILE_CHAR_STAIRS_UP "△"  // Triangle up
#define TILE_CHAR_STAIRS_DN "▽"  // Triangle down
#define TILE_CHAR_TREASURE  "◇"  // Diamond for treasure
#define TILE_CHAR_BOSS      "♜"  // Chess rook for boss
#define TILE_CHAR_ENTRANCE  "▣"  // Squared square for entrance
#define TILE_CHAR_PLAYER    "◉"  // Fisheye for player
#define TILE_CHAR_UNKNOWN   "░"  // Light shade for unexplored

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

bool safe_scanf_int(int* value) {
    if (!value) return false;

    if (scanf("%d", value) != 1) {
        // Clear input buffer on error
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return false;
    }

    // Clear remaining input
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    return true;
}

bool safe_scanf_char(char* value) {
    if (!value) return false;

    if (scanf(" %c", value) != 1) {
        // Clear input buffer on error
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return false;
    }

    // Clear remaining input
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    return true;
}

bool safe_scanf_string(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return false;

    // Create format string with size limit
    char format[32];
    snprintf(format, sizeof(format), "%%%zus", buffer_size - 1);

    if (scanf(format, buffer) != 1) {
        // Clear input buffer on error
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return false;
    }

    // Clear remaining input
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    return true;
}

void clear_screen(void) {
#ifdef _WIN32
    int result = system("cls");
    (void)result; // Intentionally unused
#else
    int result = system("clear");
    (void)result; // Intentionally unused
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
            case '\t': case 'i': case 'I': return INPUT_SELECT;
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
            case '\t': case 'i': case 'I': return INPUT_SELECT;
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

void input_flush_buffer(void) {
    // Flush stdin to clear any leftover input (like newlines from scanf)
#ifdef _WIN32
    while (_kbhit()) {
        _getch();
    }
#else
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
#endif
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
    // GameBoy-style battle screen layout (4 quadrants)
    // Top: Enemy sprites (left) | Enemy HP/Stats (right)
    // Bottom: Party sprites (left) | Party HP/MP + Menu (right)

    printf("\n");
    printf("┌─────────────────────────────────┬─────────────────────────────────┐\n");
    printf("│ ENEMY SPRITES                   │ ENEMY HP / STATS                │\n");
    printf("├─────────────────────────────────┼─────────────────────────────────┤\n");

    // Display enemies (both sides)
    if (g_battle_state.is_boss_battle && g_battle_state.boss) {
        // Boss battle
        printf("│                                 │ BOSS: %-25s │\n", g_battle_state.boss->name);
        printf("│         [BOSS SPRITE]           │ HP: %4d/%4d                     │\n",
               g_battle_state.boss->current_hp, g_battle_state.boss->max_hp);
        printf("│                                 │                                 │\n");
        printf("│                                 │                                 │\n");
    } else {
        // Regular enemies (show up to 4)
        for (uint8_t i = 0; i < 4; i++) {
            if (i < g_battle_state.enemy_count) {
                Enemy* enemy = &g_battle_state.enemies[i];
                if (enemy->is_alive) {
                    printf("│   [%c]%-26s │ %d.%-15s HP:%4d/%4d │\n",
                           'A' + i, enemy->name, i+1, enemy->name,
                           enemy->current_hp, enemy->max_hp);
                } else {
                    printf("│   [X]%-26s │ %d.%-15s [DEFEATED]    │\n",
                           enemy->name, i+1, enemy->name);
                }
            } else {
                printf("│                                 │                                 │\n");
            }
        }
    }

    printf("├─────────────────────────────────┼─────────────────────────────────┤\n");
    printf("│ PARTY SPRITES                   │ PARTY HP / MP                   │\n");
    printf("├─────────────────────────────────┼─────────────────────────────────┤\n");

    // Display party members (both sides)
    for (uint8_t i = 0; i < 4; i++) {
        if (i < g_game_state.party->member_count) {
            PartyMember* member = &g_game_state.party->members[i];
            char sprite_char = (member->stats.current_hp > 0) ? '@' : 'X';

            // Left side: sprite representation
            // Right side: HP/MP stats
            if (member->stats.current_hp > 0) {
                printf("│   [%c]%-26s │ %-12s HP:%4d/%4d MP:%3d/%3d│\n",
                       sprite_char, member->name, member->name,
                       member->stats.current_hp, member->stats.max_hp,
                       member->stats.current_mp, member->stats.max_mp);
            } else {
                printf("│   [%c]%-26s │ %-12s [DOWN]                │\n",
                       sprite_char, member->name, member->name);
            }
        } else {
            printf("│                                 │                                 │\n");
        }
    }

    printf("└─────────────────────────────────┴─────────────────────────────────┘\n");
}

void display_battle_turn_indicator(const char* actor_name) {
    printf("\n>>> %s's TURN <<<\n", actor_name);
}

int8_t cursor_menu(const char* title, const char** options, uint8_t option_count) {
    uint8_t cursor = 0;

    while (1) {
        clear_screen();
        printf("\n=== %s ===\n\n", title);

        for (uint8_t i = 0; i < option_count; i++) {
            if (i == cursor) {
                printf("> %s\n", options[i]);
            } else {
                printf("  %s\n", options[i]);
            }
        }

        printf("\nControls: W/S=Move Cursor, Enter/Z=Select, X/Esc=Cancel\n");

        InputButton input = INPUT_NONE;
        while (input == INPUT_NONE) {
            input = input_get_key();
        }

        switch (input) {
            case INPUT_UP:
                if (cursor > 0) cursor--;
                break;
            case INPUT_DOWN:
                if (cursor < option_count - 1) cursor++;
                break;
            case INPUT_A: // Z or Space
            case INPUT_START: // Enter
                return cursor;
            case INPUT_B: // X or Escape
                return -1; // Cancelled
            default:
                break;
        }
    }
}

bool virtual_keyboard(const char* prompt, char* buffer, uint8_t max_length) {
    if (!buffer || max_length < 2) return false;

    // Keyboard layout: 4 rows
    // Row 0: A-J
    // Row 1: K-T
    // Row 2: U-Z and punctuation
    // Row 3: SPACE, BACK, OK
    const char keyboard[4][10] = {
        {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'},
        {'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'},
        {'U', 'V', 'W', 'X', 'Y', 'Z', '.', '-', '\'', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} // Special row for SPACE, BACK, OK
    };

    const uint8_t row_lengths[4] = {10, 10, 10, 3};

    char current_text[32] = "";
    uint8_t text_length = 0;
    uint8_t cursor_row = 0;
    uint8_t cursor_col = 0;

    while (1) {
        clear_screen();
        printf("\n=== %s ===\n\n", prompt);

        // Display current text
        printf("Name: %s", current_text);
        for (uint8_t i = text_length; i < max_length - 1; i++) {
            printf("_");
        }
        printf("\n\n");

        // Display keyboard grid
        for (uint8_t row = 0; row < 3; row++) {
            printf("  ");
            for (uint8_t col = 0; col < row_lengths[row]; col++) {
                if (row == cursor_row && col == cursor_col) {
                    printf("[%c]", keyboard[row][col]);
                } else {
                    printf(" %c ", keyboard[row][col]);
                }
                printf(" ");
            }
            printf("\n");
        }

        // Display special buttons
        printf("\n  ");
        const char* special_buttons[3] = {"SPACE", "BACK", "OK"};
        for (uint8_t i = 0; i < 3; i++) {
            if (cursor_row == 3 && cursor_col == i) {
                printf("[%s]", special_buttons[i]);
            } else {
                printf(" %s ", special_buttons[i]);
            }
            printf("  ");
        }
        printf("\n");

        printf("\nControls: W/A/S/D=Move, Enter/Z=Select, X/Esc=Cancel\n");

        // Get input
        InputButton input = INPUT_NONE;
        while (input == INPUT_NONE) {
            input = input_get_key();
        }

        switch (input) {
            case INPUT_UP:
                if (cursor_row > 0) {
                    cursor_row--;
                    // Adjust column if out of bounds for new row
                    if (cursor_col >= row_lengths[cursor_row]) {
                        cursor_col = row_lengths[cursor_row] - 1;
                    }
                }
                break;

            case INPUT_DOWN:
                if (cursor_row < 3) {
                    cursor_row++;
                    // Adjust column if out of bounds for new row
                    if (cursor_col >= row_lengths[cursor_row]) {
                        cursor_col = row_lengths[cursor_row] - 1;
                    }
                }
                break;

            case INPUT_LEFT:
                if (cursor_col > 0) {
                    cursor_col--;
                }
                break;

            case INPUT_RIGHT:
                if (cursor_col < row_lengths[cursor_row] - 1) {
                    cursor_col++;
                }
                break;

            case INPUT_A: // Z or Space
            case INPUT_START: // Enter
                if (cursor_row == 3) {
                    // Special buttons
                    if (cursor_col == 0) {
                        // SPACE
                        if (text_length < max_length - 1) {
                            current_text[text_length++] = ' ';
                            current_text[text_length] = '\0';
                        }
                    } else if (cursor_col == 1) {
                        // BACK
                        if (text_length > 0) {
                            current_text[--text_length] = '\0';
                        }
                    } else if (cursor_col == 2) {
                        // OK - confirm and return
                        safe_string_copy(buffer, current_text, max_length);
                        return true;
                    }
                } else {
                    // Regular letter
                    if (text_length < max_length - 1) {
                        current_text[text_length++] = keyboard[cursor_row][cursor_col];
                        current_text[text_length] = '\0';
                    }
                }
                break;

            case INPUT_B: // X or Escape
                return false; // Cancelled

            default:
                break;
        }
    }
}

// Helper function to print a tile (supports both ASCII and tile graphics modes)
static void print_tile_character(TileType type, bool is_player, bool is_explored, bool tile_mode) {
    if (tile_mode) {
        // Tile Graphics Mode (GameBoy Pocket style with Unicode + ANSI colors)
        if (is_player) {
            printf("%s%s%s", GB_COLOR_LIGHTEST, TILE_CHAR_PLAYER, GB_COLOR_RESET);
        } else if (!is_explored) {
            printf("%s%s%s", GB_COLOR_DARKEST, TILE_CHAR_UNKNOWN, GB_COLOR_RESET);
        } else {
            switch (type) {
                case TILE_WALL:
                    printf("%s%s%s", GB_COLOR_DARKEST, TILE_CHAR_WALL, GB_COLOR_RESET);
                    break;
                case TILE_FLOOR:
                    printf("%s%s%s", GB_COLOR_DARK, TILE_CHAR_FLOOR, GB_COLOR_RESET);
                    break;
                case TILE_DOOR:
                    printf("%s%s%s", GB_COLOR_DARK, TILE_CHAR_DOOR, GB_COLOR_RESET);
                    break;
                case TILE_STAIRS_UP:
                    printf("%s%s%s", GB_COLOR_LIGHT, TILE_CHAR_STAIRS_UP, GB_COLOR_RESET);
                    break;
                case TILE_STAIRS_DOWN:
                    printf("%s%s%s", GB_COLOR_LIGHT, TILE_CHAR_STAIRS_DN, GB_COLOR_RESET);
                    break;
                case TILE_TREASURE:
                    printf("%s%s%s", GB_COLOR_LIGHT, TILE_CHAR_TREASURE, GB_COLOR_RESET);
                    break;
                case TILE_BOSS_ROOM:
                    printf("%s%s%s", GB_COLOR_LIGHT, TILE_CHAR_BOSS, GB_COLOR_RESET);
                    break;
                case TILE_ENTRANCE:
                    printf("%s%s%s", GB_COLOR_LIGHT, TILE_CHAR_ENTRANCE, GB_COLOR_RESET);
                    break;
                default:
                    printf(" ");
                    break;
            }
        }
    } else {
        // ASCII Mode (classic text mode)
        if (is_player) {
            printf("@");
        } else if (!is_explored) {
            printf("?");
        } else {
            switch (type) {
                case TILE_WALL: printf("#"); break;
                case TILE_FLOOR: printf("."); break;
                case TILE_DOOR: printf("+"); break;
                case TILE_STAIRS_UP: printf("<"); break;
                case TILE_STAIRS_DOWN: printf(">"); break;
                case TILE_TREASURE: printf("$"); break;
                case TILE_BOSS_ROOM: printf("B"); break;
                case TILE_ENTRANCE: printf("E"); break;
                default: printf(" "); break;
            }
        }
    }
}

void display_dungeon(void) {
    if (!g_game_state.dungeon_initialized[g_game_state.current_dungeon_index]) return;

    Dungeon* dungeon = &g_game_state.dungeons[g_game_state.current_dungeon_index];
    DungeonFloor* floor = &dungeon->floors[dungeon->current_floor];
    bool tile_mode = g_game_state.tile_graphics_mode;

    printf("\n=== %s - Floor %d ===\n", dungeon->name, dungeon->current_floor + 1);
    if (tile_mode) {
        printf("[ TILE GRAPHICS MODE ]\n");
    }

    // Display map with better visibility
    int view_range = 5;

    for (int y = floor->player_y - view_range; y <= floor->player_y + view_range; y++) {
        for (int x = floor->player_x - view_range; x <= floor->player_x + view_range; x++) {
            // Player position
            if (x == floor->player_x && y == floor->player_y) {
                print_tile_character(TILE_FLOOR, true, true, tile_mode);
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
                bool explored = floor->tiles[y][x].explored || is_adjacent;
                print_tile_character(floor->tiles[y][x].type, false, explored, tile_mode);
            }
        }
        printf("\n");
    }

    // Map legend
    if (tile_mode) {
        printf("\nLegend: %s%s%s = You, %s%s%s = Wall, %s%s%s = Floor, %s%s%s = Down, %s%s%s = Up\n",
               GB_COLOR_LIGHTEST, TILE_CHAR_PLAYER, GB_COLOR_RESET,
               GB_COLOR_DARKEST, TILE_CHAR_WALL, GB_COLOR_RESET,
               GB_COLOR_DARK, TILE_CHAR_FLOOR, GB_COLOR_RESET,
               GB_COLOR_LIGHT, TILE_CHAR_STAIRS_DN, GB_COLOR_RESET,
               GB_COLOR_LIGHT, TILE_CHAR_STAIRS_UP, GB_COLOR_RESET);
        printf("        %s%s%s = Treasure, %s%s%s = Boss, %s%s%s = Entrance\n",
               GB_COLOR_LIGHT, TILE_CHAR_TREASURE, GB_COLOR_RESET,
               GB_COLOR_LIGHT, TILE_CHAR_BOSS, GB_COLOR_RESET,
               GB_COLOR_LIGHT, TILE_CHAR_ENTRANCE, GB_COLOR_RESET);
    } else {
        printf("\nLegend: @ = You, # = Wall, . = Floor, > = Down, < = Up\n");
        printf("        $ = Treasure, B = Boss, E = Entrance\n");
    }
    printf("\nControls: WASD=Move, Z=Interact, X=Back, Enter/I=Menu\n");
}
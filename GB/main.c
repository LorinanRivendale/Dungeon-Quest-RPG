/*
 * Dungeon Quest RPG - GameBoy Minimal Demo
 * Demonstrates core dungeon exploration on actual GB hardware
 */

#include <gb/gb.h>
#include <stdint.h>
#include <stdbool.h>

// Dungeon constants
#define DUNGEON_WIDTH 16
#define DUNGEON_HEIGHT 16
#define TILE_FLOOR 0x01
#define TILE_WALL  0x02
#define TILE_PLAYER 0x03

// Player state
typedef struct {
    uint8_t x;
    uint8_t y;
} Player;

Player player;

// Simple dungeon map (16x16) - static for demo
uint8_t dungeon_map[DUNGEON_HEIGHT][DUNGEON_WIDTH];

// Tile graphics data (8x8 pixels, 2bpp format)
// Each tile is 16 bytes (8 rows × 2 bytes per row)
const unsigned char tile_data[] = {
    // Tile 0: Empty/Black
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,


    // Tile 1: Floor (dotted pattern - light gray)
    0x00,0x00,0x66,0x66,0x66,0x66,0x00,0x00,
    0x00,0x00,0x66,0x66,0x66,0x66,0x00,0x00,

    // Tile 2: Wall (brick pattern - dark)
    0x77,0x77,0x00,0x00,0xEE,0xEE,0x00,0x00,
    0x77,0x77,0x00,0x00,0xEE,0xEE,0x00,0x00,

    // Tile 3: Player (simple person shape)
    0x00,0x00,0x18,0x18,0x00,0x00,0x3C,0x3C,
    0x5A,0x5A,0x24,0x24,0x24,0x24,0x00,0x00
};

// Initialize a simple dungeon layout
void init_dungeon(void) {
    uint8_t x, y;

    // Create bordered dungeon with open interior
    for (y = 0; y < DUNGEON_HEIGHT; y++) {
        for (x = 0; x < DUNGEON_WIDTH; x++) {
            // Walls on edges
            if (x == 0 || x == DUNGEON_WIDTH-1 || y == 0 || y == DUNGEON_HEIGHT-1) {
                dungeon_map[y][x] = TILE_WALL;
            } else {
                dungeon_map[y][x] = TILE_FLOOR;
            }
        }
    }

    // Add some interior walls for testing
    dungeon_map[3][3] = TILE_WALL;
    dungeon_map[3][4] = TILE_WALL;
    dungeon_map[3][5] = TILE_WALL;
    dungeon_map[8][8] = TILE_WALL;
    dungeon_map[8][9] = TILE_WALL;
    dungeon_map[9][8] = TILE_WALL;
    dungeon_map[9][9] = TILE_WALL;

    // Initialize player in center
    player.x = 7;
    player.y = 7;
}

// Check if a tile is walkable
bool is_walkable(uint8_t x, uint8_t y) {
    if (x >= DUNGEON_WIDTH || y >= DUNGEON_HEIGHT) {
        return false;
    }
    return dungeon_map[y][x] != TILE_WALL;
}

// Render the dungeon to the background
void render_dungeon(void) {
    uint8_t x, y;
    uint8_t tile_index;

    // Render entire visible dungeon (20x18 viewport)
    for (y = 0; y < 18; y++) {
        for (x = 0; x < 20; x++) {
            uint8_t map_x = x;
            uint8_t map_y = y;

            // Bounds check
            if (map_x >= DUNGEON_WIDTH || map_y >= DUNGEON_HEIGHT) {
                tile_index = 0; // Empty tile
            } else {
                // Draw player or dungeon tile
                if (map_x == player.x && map_y == player.y) {
                    tile_index = TILE_PLAYER;
                } else {
                    tile_index = dungeon_map[map_y][map_x];
                }
            }

            // Set the tile (pass tile INDEX, not tile data!)
            set_bkg_tiles(x, y, 1, 1, &tile_index);
        }
    }
}

// Handle player input
void handle_input(void) {
    uint8_t joypad_state = joypad();
    uint8_t new_x = player.x;
    uint8_t new_y = player.y;
    bool moved = false;

    // Check D-pad input
    if (joypad_state & J_UP) {
        new_y--;
        moved = true;
    } else if (joypad_state & J_DOWN) {
        new_y++;
        moved = true;
    } else if (joypad_state & J_LEFT) {
        new_x--;
        moved = true;
    } else if (joypad_state & J_RIGHT) {
        new_x++;
        moved = true;
    }

    // Move if walkable
    if (moved) {
        if (is_walkable(new_x, new_y)) {
            player.x = new_x;
            player.y = new_y;
        }

        // Small delay to prevent too-fast movement
        delay(100);
    }
}

// Main entry point
void main(void) {
    // Set the background palette (4 shades: white to black)
    // 0b11100100 = 11 10 01 00 = darkest, dark, light, lightest
    BGP_REG = 0xE4;

    // Load tile data into VRAM (tiles 0-3)
    set_bkg_data(0, 4, tile_data);

    // Initialize dungeon
    init_dungeon();

    // Initial render
    render_dungeon();

    // Turn on the background layer
    SHOW_BKG;
    DISPLAY_ON;

    // Main game loop
    while(1) {
        // Handle player input
        handle_input();

        // Wait for VBlank (safe time to update VRAM)
        wait_vbl_done();

        // Render dungeon
        render_dungeon();
    }
}

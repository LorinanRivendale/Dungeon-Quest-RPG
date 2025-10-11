---

## PORTING_GUIDE.md
```markdown
# GameBoy Porting Guide

## Overview
This document outlines the steps and considerations for porting this C framework to GameBoy using GBDK (GameBoy Developers Kit).

## Prerequisites

### Tools Needed
1. **GBDK-2020** (recommended) or GBDK-n
   - Download: https://github.com/gbdk-2020/gbdk-2020/releases
2. **Emulator**: BGB, SameBoy, or mGBA
   - BGB (Windows): https://bgb.bircd.org/
   - SameBoy: https://sameboy.github.io/
   - mGBA: https://mgba.io/
3. **Make** (for build system)
4. **Graphics Tools**: 
   - GBTD (GameBoy Tile Designer)
   - GBMB (GameBoy Map Builder)
   - Or modern alternatives like Aseprite

## Memory Constraints

### GameBoy Specifications
CPU:     8-bit Sharp LR35902 @ 4.19 MHz (similar to Z80)
RAM:     8 KB work RAM (WRAM)
VRAM:    8 KB video RAM
ROM:     16 KB - 8 MB (with memory banking)
Screen:  160×144 pixels (20×18 tiles of 8×8 pixels)
Colors:  4 shades of gray (DMG) or 32768 colors (GBC)
Sprites: 40 sprites, max 10 per scanline
Tiles:   384 tiles total (256 BG + 128 OBJ typically)
Copy
### Current Framework Memory Usage
```c
// Approximate static memory sizes:
sizeof(GameStateData):  ~500 bytes
sizeof(Party):          ~800 bytes
sizeof(Dungeon):        ~1500 bytes per dungeon (x5 = 7.5KB)
sizeof(Inventory):      ~1000 bytes
sizeof(BattleState):    ~300 bytes

Total Static RAM:       ~4-5 KB (leaves 3-4KB for stack/heap)
Memory Optimization Needed:

Store only current dungeon floor in RAM
Use ROM banking for dungeon data
Compress tile data
Minimize stack usage

Key Modifications Needed
1. Remove Standard Library Dependencies
Current Code:
cCopy#include <stdio.h>
#include <stdlib.h>
#include <string.h>
GameBoy Version:
cCopy#include <gb/gb.h>
#include <gbdk/console.h>
#include <string.h>  // GBDK includes this

// Remove all malloc/free - use static allocation only
// Replace printf with custom text rendering
2. Graphics System Overhaul
Replace text output with tile/sprite system:
cCopy// PC Version
printf("HP: %d/%d\n", current_hp, max_hp);

// GameBoy Version
void draw_hp_bar(uint8_t x, uint8_t y, uint16_t current, uint16_t max) {
    uint8_t bar_length = (current * 10) / max;
    
    // Draw HP bar using background tiles
    set_bkg_tiles(x, y, 10, 1, hp_bar_empty);
    set_bkg_tiles(x, y, bar_length, 1, hp_bar_full);
    
    // Draw HP numbers
    draw_number(x + 12, y, current);
    draw_text(x + 15, y, "/");
    draw_number(x + 16, y, max);
}
Text Rendering:
cCopy// Map ASCII to tile indices
const uint8_t ascii_to_tile[128] = {
    // Map characters to your font tiles
    [' '] = 0,
    ['A'] = 1,
    ['B'] = 2,
    // ... etc
};

void draw_text(uint8_t x, uint8_t y, const char* text) {
    uint8_t i = 0;
    while (text[i] != '\0') {
        set_bkg_tile_xy(x + i, y, ascii_to_tile[(uint8_t)text[i]]);
        i++;
    }
}
3. Input System (Already Abstracted!)
Just remap the input functions:
cCopy// utils.c - GameBoy version
InputButton input_get_key(void) {
    uint8_t keys = joypad();
    
    if (keys & J_UP)     return INPUT_UP;
    if (keys & J_DOWN)   return INPUT_DOWN;
    if (keys & J_LEFT)   return INPUT_LEFT;
    if (keys & J_RIGHT)  return INPUT_RIGHT;
    if (keys & J_A)      return INPUT_A;
    if (keys & J_B)      return INPUT_B;
    if (keys & J_START)  return INPUT_START;
    if (keys & J_SELECT) return INPUT_SELECT;
    
    return INPUT_NONE;
}

void input_wait_for_key(void) {
    // Wait for button release then press
    while (joypad()) {
        wait_vbl_done();
    }
    while (!joypad()) {
        wait_vbl_done();
    }
}
4. Random Number Generator
Use GameBoy's DIV register for entropy:
cCopy// Use hardware timer for random seed
void random_seed_gb(void) {
    // DIV register increments at 16384 Hz
    random_seed_value = DIV_REG | (DIV_REG << 8);
}

// Alternative: Use R register (refresh register)
void random_seed_r(void) {
    __asm
        ld a, r
        ld (_random_seed_value), a
    __endasm;
}
5. Save System Using SRAM
cCopy// Battery-backed SRAM at 0xA000-0xBFFF
#define SRAM_START 0xA000
#define SAVE_MAGIC 0x5250  // "RP" magic number

typedef struct {
    uint16_t magic;
    uint8_t version;
    GameStateData game_state;
    uint8_t checksum;
} SaveData;

void save_game(void) {
    ENABLE_RAM;
    
    SaveData* save = (SaveData*)SRAM_START;
    
    // Write magic and version
    save->magic = SAVE_MAGIC;
    save->version = 1;
    
    // Copy game state
    memcpy(&save->game_state, &g_game_state, sizeof(GameStateData));
    
    // Calculate checksum
    save->checksum = calculate_checksum((uint8_t*)save, 
                                       sizeof(SaveData) - 1);
    
    DISABLE_RAM;
}

bool load_game(void) {
    ENABLE_RAM;
    
    SaveData* save = (SaveData*)SRAM_START;
    
    // Verify magic number
    if (save->magic != SAVE_MAGIC) {
        DISABLE_RAM;
        return false;
    }
    
    // Verify checksum
    uint8_t check = calculate_checksum((uint8_t*)save, 
                                       sizeof(SaveData) - 1);
    if (check != save->checksum) {
        DISABLE_RAM;
        return false;
    }
    
    // Load data
    memcpy(&g_game_state, &save->game_state, sizeof(GameStateData));
    
    DISABLE_RAM;
    return true;
}

uint8_t calculate_checksum(uint8_t* data, uint16_t length) {
    uint8_t sum = 0;
    for (uint16_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return ~sum; // One's complement
}
Tile and Sprite Planning
Background Tiles Budget (256 tiles for BG)
Font & UI (80 tiles)

Font: A-Z, 0-9, punctuation (50 tiles)
UI borders and windows (20 tiles)
Icons: HP, MP, Gold, etc. (10 tiles)

Dungeon Graphics (100 tiles)

Floor tiles: 4 variations (4 tiles)
Wall tiles: 16 variations for corners/edges (16 tiles)
Door tiles: closed, open (4 tiles)
Stairs: up, down (4 tiles)
Treasure chest: closed, open (2 tiles)
Special tiles: entrance, boss door (4 tiles)
Decorative: torches, cracks, etc. (20 tiles)

Battle Background (50 tiles)

Battle scene backgrounds (30 tiles)
Effect tiles: slash, fire, etc. (20 tiles)

Menu Graphics (26 tiles)

Menu borders and cursors (10 tiles)
Status icons (16 tiles)

Sprite Planning (128 tiles for OBJ)
Player Character (16 tiles)

4 directions × 2 frames × 2 tiles per frame = 16 tiles

Enemy Sprites (60 tiles)

Small enemies: 2×2 tiles × 6 types = 24 tiles
Medium enemies: 3×3 tiles × 2 types = 18 tiles
Boss enemies: 4×4 tiles × 1 type = 16 tiles
Reserve: 2 tiles

UI Sprites (20 tiles)

Cursor/selection indicator (4 tiles)
Party member portraits (16 tiles, 4×4 each when needed)

Effect Sprites (32 tiles)

Battle effects: hits, magic (20 tiles)
Status icons (8 tiles)
Misc effects (4 tiles)

Screen Layout Planning
Dungeon View (160×144 pixels, 20×18 tiles)
Copy┌────────────────────┐
│  CAVE OF EARTH F1  │ 2 tiles - Header
├────────────────────┤
│                    │
│   ┌──────────┐     │
│   │ Dungeon  │     │ 11 tiles - Main view
│   │   Map    │     │ (7×7 visible area)
│   │  (7×7)   │     │
│   └──────────┘     │
│                    │
├────────────────────┤
│ HP:███░░  MP:██░░  │ 3 tiles - Party HP/MP
│ HP:████░  MP:███░  │ (2 rows for 4 members)
├────────────────────┤
│ [A]ct [B]ack [Sel] │ 2 tiles - Controls
└────────────────────┘
Battle View (160×144 pixels, 20×18 tiles)
Copy┌────────────────────┐
│    === BATTLE ===  │ 2 tiles - Header
├────────────────────┤
│                    │
│     [Enemy]        │ 6 tiles - Enemy display
│    Goblin HP:20    │ (sprites + text)
│                    │
├────────────────────┤
│ > Attack           │ 6 tiles - Action menu
│   Magic            │
│   Item             │
│   Defend           │
├────────────────────┤
│ Hero   HP:██░ 50   │ 4 tiles - Party status
│ Mage   HP:█░░ 30   │
│ Priest HP:███ 60   │
│ Thief  HP:██░ 45   │
└────────────────────┘
Menu View (160×144 pixels, 20×18 tiles)
Copy┌────────────────────┐
│   === MENU ===     │ 2 tiles - Header
├────────────────────┤
│ > Items            │
│   Equipment        │ 8 tiles - Main menu
│   Status           │
│   Save             │
│   Exit             │
├────────────────────┤
│ Gold: 1250         │ 2 tiles - Resources
├────────────────────┤
│ Potion      x5     │
│ Hi-Potion   x2     │ 6 tiles - Item list
│ Ether       x3     │
│ Antidote    x4     │
└────────────────────┘
ROM Banking Strategy
Bank 0 (Fixed, always accessible)
cCopy// Core engine code
- VBlank interrupt handler
- Input handling
- Main game loop
- Essential utility functions
- Tile and sprite data loading
Bank 1-4 (Switchable - Dungeon Data)
cCopy// Bank 1: Cave of Earth
const uint8_t dungeon1_layout[] = { /* floor data */ };
const uint8_t dungeon1_encounters[] = { /* enemy data */ };
const uint8_t dungeon1_treasures[] = { /* treasure data */ };

// Bank 2: Water Temple
// Bank 3: Volcano Keep  
// Bank 4: Sky Tower
// Similar structure for each dungeon
Bank 5 (Switchable - Final Dungeon)
cCopy// Bank 5: Final Sanctum
const uint8_t final_dungeon_layout[] = { /* 5 floors */ };
const BossData final_boss = { /* Dark Lord */ };
Bank 6-7 (Switchable - Graphics)
cCopy// Bank 6: Tile Graphics
const uint8_t dungeon_tiles[] = { /* tile data */ };
const uint8_t battle_tiles[] = { /* battle graphics */ };

// Bank 7: Sprite Graphics  
const uint8_t player_sprites[] = { /* player animations */ };
const uint8_t enemy_sprites[] = { /* enemy sprites */ };
const uint8_t boss_sprites[] = { /* boss sprites */ };
Bank 8+ (Switchable - Audio)
cCopy// Bank 8: Music
const uint8_t music_dungeon[] = { /* dungeon theme */ };
const uint8_t music_battle[] = { /* battle theme */ };
const uint8_t music_boss[] = { /* boss theme */ };

// Bank 9: Sound Effects
const uint8_t sfx_hit[] = { /* attack sound */ };
const uint8_t sfx_item[] = { /* item use sound */ };
// etc.
Bank Switching Example:
cCopyvoid load_dungeon_data(uint8_t dungeon_id) {
    // Switch to appropriate bank
    SWITCH_ROM_MBC1(dungeon_id + 1);
    
    // Access dungeon data (now in 0x4000-0x7FFF range)
    memcpy(&current_dungeon_layout, dungeon_layout_ptr, layout_size);
    
    // Switch back to bank 0 if needed
    SWITCH_ROM_MBC1(0);
}
Performance Optimization
VBlank Management
cCopy// Only update graphics during VBlank
volatile uint8_t vbl_flag = 0;

void vbl_interrupt() {
    vbl_flag = 1;
}

void wait_vblank() {
    vbl_flag = 0;
    while (!vbl_flag) {
        // Halt CPU until interrupt
        halt();
    }
}

// In main loop
void game_loop() {
    while (1) {
        // Do game logic
        update_game_state();
        
        // Wait for VBlank before drawing
        wait_vblank();
        
        // Update graphics
        update_sprites();
        update_background();
    }
}
Battle Calculations
cCopy// Pre-calculate damage tables to save CPU cycles
const uint8_t damage_table[16][16] = {
    // [attacker_strength][defender_defense]
    // Pre-computed values
    {5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {8, 7, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    // ... etc
};

uint16_t calculate_damage_fast(uint8_t atk, uint8_t def) {
    // Direct lookup instead of calculation
    uint8_t base = damage_table[atk >> 2][def >> 2];
    
    // Add small random variance
    return base + (random_range(0, 3));
}
Dungeon Generation
cCopy// Instead of runtime generation, use pre-made layouts
// Stored in ROM, loaded as needed

void load_dungeon_floor(uint8_t dungeon_id, uint8_t floor_id) {
    // Switch to dungeon data bank
    SWITCH_ROM_MBC1(dungeon_id + 1);
    
    // Copy pre-made floor layout
    const uint8_t* floor_data = get_floor_ptr(floor_id);
    memcpy(current_floor, floor_data, sizeof(DungeonFloor));
    
    // Switch back
    SWITCH_ROM_MBC1(0);
}
Build System for GBDK
Makefile for GBDK
makefileCopyCC = lcc
CFLAGS = -Wa-l -Wl-m -Wl-j -DGAMEBOY

BINS = rpg_game.gb
OBJS = main.o game_state.o party.o dungeon.o battle.o inventory.o utils_gb.o

all: $(BINS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.gb: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.noi

fix:
	rgbfix -v -p 0xFF $@

.PHONY: all clean fix
Compiling
bashCopy# Compile the game
make

# Fix ROM header
rgbfix -v -p 0xFF rpg_game.gb

# Run in emulator
bgb rpg_game.gb
Testing Strategy
1. Emulator Testing (Primary)
bashCopy# Use BGB for debugging (Windows/Wine)
- Step-through debugging
- Memory viewer
- VRAM viewer
- Register inspection

# Use SameBoy for accuracy (All platforms)
- Most accurate DMG emulation
- Good for final testing

# Use mGBA for convenience (All platforms)
- Fast
- Good debugging tools
- Cross-platform
2. Hardware Testing (Secondary)
bashCopy# Test on real hardware using flash cart
- EverDrive GB
- Analogue Pocket  
- BennVenn Flash Cart

# Things to test on hardware:
- Save/load functionality
- Performance (no slowdown)
- Audio quality
- Display compatibility
3. Automated Testing
cCopy// Build test ROM with assertions
#ifdef DEBUG
    #define ASSERT(x) if (!(x)) { show_error(__LINE__); while(1); }
#else
    #define ASSERT(x)
#endif

void test_battle_system() {
    ASSERT(calculate_damage(10, 5) > 0);
    ASSERT(party_is_defeated(NULL) == true);
    // etc.
}
Gradual Port Strategy
Phase 1: Core Framework (1-2 weeks)
Goal: Get basic structure working

 Port main.c to GBDK
 Implement text rendering system
 Port input handling
 Create simple test scene
 Verify build system works

Deliverable: ROM that shows text and responds to input
Phase 2: Graphics System (2-3 weeks)
Goal: Display game graphics

 Create tile set in GBTD
 Implement tile loading
 Create sprite system
 Port dungeon display
 Add scrolling/camera

Deliverable: ROM showing dungeon with moveable character
Phase 3: Game Logic (2-3 weeks)
Goal: Port core gameplay

 Port party system
 Implement dungeon exploration
 Add collision detection
 Port inventory system
 Add treasure chests

Deliverable: Playable dungeon exploration
Phase 4: Battle System (2-3 weeks)
Goal: Complete combat

 Port battle system
 Create battle graphics
 Implement animations
 Add enemy AI
 Port experience system

Deliverable: Full battle system working
Phase 5: Content & Polish (3-4 weeks)
Goal: Complete game

 Add all 5 dungeons
 Create all enemy sprites
 Implement save system
 Add music and SFX
 Bug fixing and balance
 Optimization

Deliverable: Complete, polished GameBoy game
Phase 6: Testing & Release (1-2 weeks)
Goal: Release-ready ROM

 Extensive testing
 Hardware verification
 Create manual/documentation
 Package release
 Publish

Deliverable: Final ROM ready for distribution
Common Pitfalls to Avoid
1. Stack Overflow
cCopy// DON'T: Large local arrays
void bad_function() {
    uint8_t huge_array[1000]; // Stack overflow!
    // ...
}

// DO: Use global or static storage
static uint8_t huge_array[1000];
void good_function() {
    // Use huge_array
}
2. Unaligned Memory Access
cCopy// DON'T: Assume alignment
uint16_t* ptr = (uint16_t*)(some_array + 1);
*ptr = 0x1234; // May not work correctly

// DO: Use memcpy for unaligned access
uint16_t value = 0x1234;
memcpy(some_array + 1, &value, sizeof(uint16_t));
3. Forgetting VBlank
cCopy// DON'T: Update VRAM outside VBlank
set_bkg_tiles(0, 0, 20, 18, map_data); // May glitch!

// DO: Wait for VBlank first
wait_vbl_done();
set_bkg_tiles(0, 0, 20, 18, map_data);
4. Bank Switching Bugs
cCopy// DON'T: Forget which bank you're in
SWITCH_ROM_MBC1(2);
some_function(); // This might switch banks!
// Now you don't know which bank is active

// DO: Save and restore bank
uint8_t old_bank = current_bank;
SWITCH_ROM_MBC1(2);
// Do stuff
SWITCH_ROM_MBC1(old_bank);
5. Inefficient Loops
cCopy// DON'T: Use expensive operations in loops
for (uint8_t i = 0; i < strlen(text); i++) { // strlen called each time!
    // ...
}

// DO: Cache the result
uint8_t len = strlen(text);
for (uint8_t i = 0; i < len; i++) {
    // ...
}
Resources
Essential Links

GBDK Documentation: https://gbdk-2020.github.io/gbdk-2020/
Pan Docs (GB hardware specs): https://gbdev.io/pandocs/
GB Dev Community: https://gbdev.io/
Awesome GB Development: https://github.com/gbdev/awesome-gbdev

Tutorials

GB ASM Tutorial: https://eldred.fr/gb-asm-tutorial/
GB Studio for non-programmers: https://www.gbstudio.dev/
GBDK Tutorial: https://gbdk-2020.github.io/gbdk-2020/docs/api/

Tools

GBTD/GBMB: http://www.devrs.com/gb/hmgd/gbtd.html
BGB Emulator: https://bgb.bircd.org/
Emulicious: https://emulicious.net/
Aseprite: https://www.aseprite.org/

Example Projects

GB Studio Games: https://itch.io/games/tag-gb-studio
GBDK Example: https://github.com/gbdk-2020/gbdk-2020/tree/develop/gbdk-lib/examples
Open source GB games: https://github.com/topics/gameboy-game

Final Notes
This framework was intentionally designed to make GameBoy porting straightforward:
✅ Already GB-friendly:

No dynamic memory allocation
Simple data structures
Fixed-point math only
State machine architecture
Abstracted input layer
Modular code organization

⚠️ Needs adaptation:

Graphics system (text → tiles)
printf → custom rendering
File I/O → SRAM saves
Random stdlib → GBDK functions

The estimated timeline for a complete port is 10-15 weeks for one experienced developer, or 15-20 weeks for someone learning GBDK simultaneously.
Good luck with your GameBoy RPG!
# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Dungeon Quest RPG is a dungeon-crawling RPG written in C99 as a test framework for eventual GameBoy porting. The game features turn-based combat, party management, dungeon exploration, and boss battles. It's inspired by Final Fantasy Legend and Sword of Hope series.

**Key Design Philosophy**: This codebase is intentionally designed to be GameBoy-portable:
- No dynamic memory allocation (all static)
- Simple data structures
- Fixed-point math only
- State machine architecture
- Abstracted input layer
- Modular code organization

## Build Commands

### Linux/Unix (using Make)
```bash
make              # Build the game
make run          # Build and run
make debug        # Build with debug symbols (-g -DDEBUG)
make clean        # Remove build artifacts
```

### Windows (using build.bat)
```bash
build.bat         # Build with GCC on Windows
```

### Manual Compilation
```bash
gcc -Wall -Wextra -std=c99 -O2 SRC/*.c -o rpg_game
```

**Note**: Source files are in the `SRC/` directory (uppercase), not `src/`. The Makefile references `src/` but the actual directory is `SRC/`.

## Testing

### Run the Game
```bash
./rpg_game        # Linux/Unix
rpg_game.exe      # Windows
```

### Quick Test (5 min)
Create a party, enter Cave of Earth, test combat and treasure collection.

### Full Playthrough (30-45 min)
Complete all 4 dungeons (Cave of Earth, Water Temple, Volcano Keep, Sky Tower) and the Final Sanctum.

## Code Architecture

### State Machine Design

The game uses a central state machine (`GameStateData`) that controls all major game phases:
- `STATE_PARTY_SELECT` → `STATE_DUNGEON_SELECT` → `STATE_DUNGEON_EXPLORE` ↔ `STATE_BATTLE`/`STATE_BOSS_BATTLE`
- Victory/defeat transitions to `STATE_VICTORY` or `STATE_GAME_OVER`

All state changes go through `game_state_change()` in `game_state.c`.

### Module Organization

#### Core Modules (in `SRC/` directory)

1. **game_state.h/c** - Central game state management
   - Global game state: `g_game_state`
   - State transitions and game initialization
   - Key item tracking (4 crystals to unlock final dungeon)

2. **party.h/c** - Party and character system
   - 6 job classes: Knight, Black Belt, Thief, Sage, Priest, Mage
   - Character stats, leveling, and experience system
   - Skills/magic system (per-job abilities)
   - Equipment slots (weapon, armor, helmet, accessory)
   - Status effects (poison, paralysis, sleep, confusion, dead)

3. **dungeon.h/c** - Dungeon generation and exploration
   - Procedural dungeon generation (16x16 tiles per floor)
   - Tile types: floor, wall, stairs, treasure, boss room
   - Random encounter system
   - Boss data structures

4. **battle.h/c** - Turn-based combat system
   - Global battle state: `g_battle_state`
   - Turn order calculation based on agility
   - Action processing (attack, skill, item, defend, flee)
   - Enemy generation and boss battles
   - Damage calculation and critical hits

5. **inventory.h/c** - Item and equipment management
   - Consumables: potions, ethers, antidotes, tents
   - Equipment with stat bonuses
   - Equipment system integrated with party members
   - Item usage during exploration and combat

6. **utils.h/c** - Platform abstraction layer
   - **Input abstraction**: `InputButton` enum maps to keyboard (PC) or will map to GameBoy buttons
   - Random number generation (seeded, portable)
   - Display functions (text-based for PC, will be tile-based for GB)
   - Screen clearing and menu display

### Data Flow

**Dungeon Exploration Loop**:
1. `handle_dungeon_exploration()` in `main.c`
2. Display dungeon via `display_dungeon()` (utils.c)
3. Get input via `input_get_key()` (utils.c)
4. Move player via `dungeon_move_player()` (dungeon.c)
5. Check encounters via `dungeon_check_encounter()` (dungeon.c)
6. Transition to battle if encounter occurs

**Battle Loop**:
1. `handle_battle_phase()` in `main.c`
2. Calculate turn order via `battle_calculate_turn_order()` (battle.c)
3. Process player/enemy actions via `battle_process_action()` (battle.c)
4. Check victory/defeat via `battle_is_over()` (battle.c)
5. Distribute rewards and return to exploration

**Critical Global Variables**:
- `g_game_state` (game_state.c) - Master game state
- `g_battle_state` (battle.c) - Active battle state

## GameBoy Porting Considerations

When modifying code, maintain GameBoy compatibility:

### Memory Constraints
- GameBoy has 8KB RAM total
- Current framework uses ~4-5KB for static structures
- **Do not introduce dynamic allocation** (`malloc`/`free`)
- Keep stack usage minimal (avoid large local arrays)

### Platform Abstraction
The `utils.h/c` module provides abstraction points for GameBoy porting:
- **Input**: `input_get_key()` returns `InputButton` enum (maps to GB buttons)
- **Display**: `display_*` functions are text-based now, will become tile/sprite rendering
- **Random**: `random_seed()` and `random_range()` will use GB's DIV register

When adding features, follow this pattern:
- Keep platform-specific code in `utils.c`
- Use the abstracted functions in game logic modules
- Avoid direct use of `stdio.h` functions outside `main.c` and `utils.c`

### Data Structures
All major structures use fixed-size arrays:
- `Party` has `PartyMember[4]` (not dynamic)
- `Dungeon` has `DungeonFloor[5]` (max floors)
- `Inventory` has `Item[20]` and `Equipment[20]`

When adding features, use fixed-size arrays and static allocation.

## Key Game Systems

### Experience and Leveling
- Experience required for next level: `100 * level * level` (see `character_get_exp_for_next_level()`)
- Stats increase per level based on job class
- Skills learned at specific levels (per-job)

### Dungeon Progression
- 4 main dungeons, each with 3 floors
- Each dungeon boss drops a crystal (key item)
- Final Sanctum unlocks when all 4 crystals collected
- Final Sanctum has 5 floors and the final boss

### Combat Mechanics
- Damage formula in `battle_calculate_damage()`: `(attacker_atk * 2) - defender_def + random(0, 5)`
- Critical hits: 5% base chance, modified by attacker's luck
- Turn order: Sorted by agility (highest first)
- Flee chance: 50% in normal battles, 0% in boss battles

### Skills System
- Each job has unique skills (see SKILL_* defines in party.h)
- Skills cost MP and have varying effects (damage, healing, buffs)
- Skill data in `get_skill_data()` function (party.c)
- Skills learned at character creation via `character_init_starting_skills()`

## Development Workflow

### Adding a New Feature
1. Identify which module(s) need changes
2. Update header file with new function declarations/structures
3. Implement in corresponding .c file
4. Update main.c if new state transitions needed
5. Test with `make run`

### Adding a New Enemy Type
1. Add enum to `EnemyType` in battle.h
2. Add name to `enemy_names[]` array in battle.c
3. Update `battle_generate_enemies()` to include new enemy in spawn logic

### Adding a New Item/Equipment
1. Add enum to `ConsumableItem` or create equipment ID in inventory.h
2. Update `item_create_consumable()` or `item_create_equipment()` in inventory.c
3. Add to starting inventory or dungeon treasure tables

### Adding a New Dungeon
1. Increment `MAX_DUNGEONS` in game_state.h (currently 4)
2. Add name to `dungeon_names[]` in dungeon.c
3. Update `handle_dungeon_selection()` in main.c
4. Create boss data in `dungeon_init_boss()` in dungeon.c

## Important File Locations

- Main game loop: `main.c:52-123`
- State handlers: `main.c` (handle_party_selection, handle_dungeon_selection, etc.)
- Party creation: `party.c:party_create()` and `party_add_member()`
- Dungeon generation: `dungeon.c:dungeon_generate_floor()`
- Battle initialization: `battle.c:battle_init()`
- Turn processing: `battle.c:battle_execute_turn()`
- Cursor menu system: `utils.c:cursor_menu()` (GameBoy-ready navigation)

## Input System

### Current Implementation (Hybrid)
The game uses two input methods:
1. **Cursor-based menus** (GameBoy-ready): Dungeon selection, dungeon menu, camp menu, battle system
   - Uses `input_get_key()` for immediate key detection
   - W/S to navigate, Enter/Z to select, X/Esc to cancel
   - No scanf(), no input buffer issues
   - **Battle system** fully cursor-based: actions, targets, skills, items all use cursor navigation

2. **Text input menus** (PC-only): Party creation, save/load, inventory details
   - Uses `scanf()` for text/number input
   - **TODO for GameBoy port**: Convert these to cursor-based

### GameBoy Port TODO
When porting to GameBoy, the following screens still need cursor-based conversion:
- **Party selection** (handle_party_selection) - Currently uses scanf for job selection and names
- **Save/Load menus** (handle_save_menu, handle_load_menu) - Need cursor-based slot selection
- **Inventory menu** (handle_inventory_menu) - Need cursor-based item/equipment selection

**Note**: Battle system is now FULLY GameBoy-ready with cursor navigation for all actions and target selection!

## Common Pitfalls

1. **Directory naming**: Source files are in `SRC/` (uppercase), not `src/`
2. **State management**: Always use `game_state_change()` for state transitions
3. **Memory**: Never use `malloc`/`free` - this breaks GameBoy compatibility
4. **Global state**: Access party via `g_game_state.party`, not local copies
5. **Input**: Use `input_get_key()` abstraction, not direct keyboard input
6. **Battle state**: Check `battle_is_over()` before processing turns

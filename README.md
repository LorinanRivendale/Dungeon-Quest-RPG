# Dungeon Quest RPG - Test Framework

A small form factor RPG framework designed for PC testing before eventual GameBoy port. Inspired by Final Fantasy Legend and Sword of Hope series.

## Overview

This is a dungeon-crawling RPG where players:
- Select a party of 4 members from 6 job classes
- Explore 4 unique dungeons
- Defeat bosses to collect key items
- Unlock and complete the final dungeon

## Features

### Current Implementation
- ✅ Party creation with 6 job classes
- ✅ Turn-based battle system
- ✅ Dungeon exploration with hand-crafted maps
- ✅ Town system with Inn, shops, and NPCs
- ✅ Item Shop (buy/sell consumables)
- ✅ Equipment Shop (buy/sell equipment with 22 items)
- ✅ Equipment management system with comparison UI
- ✅ Inventory system (items and equipment)
- ✅ Boss battles with key item rewards
- ✅ Experience and leveling system
- ✅ Status effects framework
- ✅ Step-counter encounter system (Final Fantasy style)
- ✅ Save/Load system (2 save slots + suspend save)
- ✅ Cursor-based menu navigation (GameBoy-ready)

### Job Classes

1. **Knight** - High HP, high defense, excellent frontline fighter
2. **Black Belt** - High strength and agility, martial arts expert
3. **Thief** - High agility and luck, nimble attacker
4. **Sage** - Balanced magic user, high intelligence
5. **Priest** - Healing specialist, high MP
6. **Mage** - Offensive magic specialist, very high intelligence

### Dungeons

1. **Cave of Earth** - Rewards: Earth Crystal
2. **Water Temple** - Rewards: Water Crystal
3. **Volcano Keep** - Rewards: Fire Crystal
4. **Sky Tower** - Rewards: Wind Crystal
5. **Final Sanctum** - Unlocked after collecting all 4 crystals

## Controls

### Dungeon Exploration
- **W/A/S/D** - Move character (Up/Left/Down/Right)
- **Z or Space** - Interact with tiles (stairs, treasure, boss)
- **X or Escape** - Exit dungeon (with confirmation)
- **Enter or I** - Open menu

### Menu Navigation (Cursor-Based)
- **W/S** - Move cursor up/down
- **Enter or Z** - Select highlighted option
- **X or Escape** - Cancel/Go back

### Battle (Cursor-Based)
- **W/S** - Move cursor through battle actions and targets
- **Enter or Z** - Select action or target
- **X or Escape** - Cancel selection (return to action menu)
- Battle actions: Attack, Skill/Magic, Item, Defend, Flee
- All target selection uses cursor navigation

### General
- Follow on-screen prompts for text input (party creation, save/load)

### Town System

The town provides essential services between dungeon runs:

1. **Inn** - Rest and fully restore party HP/MP (50 Gold)
2. **Item Shop** - Buy and sell consumable items
   - Potion (50 HP) - 50 Gold
   - Hi-Potion (150 HP) - 150 Gold
   - Ether (30 MP) - 100 Gold
   - Elixir (Full restore) - 1000 Gold
   - Antidote - 30 Gold
   - Tent (Full party heal) - 200 Gold

3. **Equipment Shop** - Buy and sell equipment (22 items total)
   - Weapons: Daggers, Swords, Staffs, Rods, Nunchaku
   - Armor: Cloth, Leather, Chain Mail, Plate Mail, Robes
   - Helmets: Leather Cap, Iron Helm, Wizard Hat
   - Accessories: Power Ring, Defense Ring, Luck Ring
   - Equip items immediately after purchase!

4. **Tavern** - Talk to locals (coming soon: quests)

### Equipment Management

Access equipment management from multiple locations:
- **In Dungeon** - Camp menu → Manage Equipment
- **Main Menu** - Party Status → Manage Equipment
- **After Purchase** - Equip items immediately from shops

Features:
- View current stats with equipment bonuses
- Comparison UI shows stat changes before equipping
- Job restrictions enforced (e.g., Knights can use plate mail)
- Unequip option available for all slots

## Building and Running

### Requirements
- GCC compiler (or compatible C compiler)
- Make (optional, but recommended)
- Terminal/Console

### Compilation

#### Using Make (Recommended)
```bash
make            # Build the game
make run        # Build and run
make debug      # Build with debug symbols
make clean      # Clean build files
```

#### Windows (Manual)
```bash
gcc -Wall -Wextra -std=c99 -O2 SRC/*.c -o rpg_game.exe
```

#### Linux/Unix (Manual)
```bash
gcc -Wall -Wextra -std=c99 -O2 SRC/*.c -o rpg_game
```

### Running the Game

#### Linux/Unix
```bash
./rpg_game
```

#### Windows
```bash
rpg_game.exe
```

## Gameplay Tips

- **Start with a balanced party** - Include at least one healer (Priest/Sage)
- **Visit the town often** - Stock up on potions and upgrade equipment
- **Save frequently** - Use the suspend save for quick saves
- **Equipment matters** - Better gear significantly improves combat performance
- **Use the comparison UI** - Always check stat changes before equipping
- **Manage your gold** - Balance equipment upgrades with consumable items

## Game Flow

1. **Create your party** - Choose 4 characters from 6 job classes
2. **Visit the town** - Buy starting equipment and items
3. **Enter a dungeon** - Explore and fight monsters
4. **Collect crystals** - Defeat bosses to get key items
5. **Return to town** - Rest at the inn and upgrade equipment
6. **Unlock final dungeon** - Collect all 4 crystals
7. **Complete the game** - Defeat the final boss

## Technical Details

- Written in C99 for maximum portability
- No dynamic memory allocation (GameBoy-ready)
- Fixed-point math only
- State machine architecture
- Abstracted input layer for future GameBoy port
- Source files in `SRC/` directory (uppercase)
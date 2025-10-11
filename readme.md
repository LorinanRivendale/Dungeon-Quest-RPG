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
- ✅ Dungeon exploration with procedural generation
- ✅ Inventory system (items and equipment)
- ✅ Boss battles with key item rewards
- ✅ Experience and leveling system
- ✅ Status effects framework
- ✅ Random encounters

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
---

## QUICKSTART.md
```markdown
# Quick Start Guide

## First Time Setup

1. **Extract or clone the project**
   ```bash
   cd rpg_framework

Build the game
bashCopymake
Or if make is not available:
bashCopygcc -Wall -std=c99 src/*.c -o rpg_game

Run the game
bashCopy./rpg_game


Recommended First Playthrough
Party Composition
For a balanced first run:

Knight - Tank/Fighter (Name: "Arthur")
Priest - Healer (Name: "Sophia")
Mage - Magic DPS (Name: "Merlin")
Thief - Physical DPS (Name: "Robin")

Dungeon Order
Recommended progression:

Cave of Earth (Easiest) - Good for grinding early levels
Water Temple - Moderate difficulty
Sky Tower - Challenging
Volcano Keep (Hardest) - Toughest of the four
Final Sanctum (After collecting all 4 crystals)

Tips for Success
Combat Tips

Target weak enemies first - Reduce incoming damage quickly
Use items liberally - Gold is plentiful, health is not
Defend when low on HP - Reduces damage taken
Level up between dungeons - Grind in Cave of Earth if struggling

Exploration Tips

Explore thoroughly - Find all treasure chests
Mark treasure locations mentally - Gold adds up quickly
Use stairs strategically - You can always return to heal
Boss rooms are marked 'B' - Make sure you're ready before entering

Resource Management

Save potions for boss battles - You can't flee from bosses
Use cheaper items first - Save Hi-Potions and Ethers
Buy items after each dungeon - (When shops are implemented)
Don't hoard - Items are meant to be used

Sample Playthrough (30-45 minutes)
Phase 1: Getting Started (5 minutes)

Create party with recommended composition
Review party stats (press Enter in dungeon selection)
Enter Cave of Earth
Learn the controls by exploring

Phase 2: First Dungeon (10 minutes)

Walk around (battles occur every 15-30 steps)
Fight 3-5 battles to reach level 2-3
Find treasure chests on each floor
Locate stairs (press Z to use them)
Press Enter to access Camp menu and save progress
Find and defeat the Earth Golem boss (marked 'B')
Obtain Earth Crystal

Phase 3: Building Strength (10 minutes)

Complete Water Temple for Water Crystal
Level party to around level 5-6
Stock up on potions using earned gold

Phase 4: Advanced Dungeons (15 minutes)

Tackle Sky Tower and Volcano Keep
Use items strategically in boss fights
Collect remaining crystals

Phase 5: Final Challenge (10 minutes)

Enter Final Sanctum (unlocked with all 4 crystals)
Navigate 5 floors
Face the Dark Lord
Complete the game!

Common Issues & Solutions
"Command not found: make"
Solution: Install make or use manual compilation:
bashCopygcc -Wall -std=c99 src/*.c -o rpg_game
"Permission denied: ./rpg_game"
Solution: Make executable with:
bashCopychmod +x rpg_game
Game crashes on input (Unix/Linux)
Solution: This is a known issue with terminal settings. Try running in a different terminal emulator or use the blocking input mode.
Can't see dungeon properly
Solution:

Maximize your terminal window
Game requires at least 80x24 characters
Use a monospace font

Character names are cut off
Solution: Keep names under 12 characters
Party got wiped out
Solution:

Game Over means restart
Save/load not implemented yet
Consider leveling up more before boss fights

Controls Reference

Dungeon Exploration
```
W, A, S, D    - Move (Up, Left, Down, Right)
Z or Space    - Interact with tiles (stairs, treasure, boss)
X or ESC      - Leave dungeon (with confirmation)
Enter or I    - Open menu
```

Menu Navigation (NEW: Cursor-Based!)
```
W / S         - Move cursor up/down
Enter or Z    - Select highlighted option
X or ESC      - Cancel / Go back one level
```

Dungeon Menu Options
```
> Camp          - Rest, heal, and save game
> Inventory     - Manage items and equipment
> Party Status  - View detailed character stats
> Return        - Return to dungeon exploration
```

Battle Controls (NEW: Cursor-Based!)
```
W / S         - Move cursor through actions and targets
Enter or Z    - Select action or target
X or ESC      - Cancel selection (return to action menu)

Actions:
  Attack      - Select enemy target with cursor
  Skill/Magic - Choose skill, then target with cursor
  Item        - Choose item, then party member with cursor
  Defend      - Reduce damage taken this turn
  Flee        - Attempt escape (50% chance, not available vs bosses)
```

Dungeon Selection Menu (NEW: Cursor-Based!)
```
W / S         - Move cursor through dungeons and menu options
Enter or Z    - Select dungeon or menu option
X or ESC      - Stay in menu (can't exit from top level)
Options include: Inventory, Party Status, Save, Load, Quit
```
Test Scenarios
Quick Test (5 minutes)
Goal: Verify game is working

Create any party
Enter Cave of Earth
Walk until battle triggers
Test combat system
Find one treasure chest

Balance Test (20 minutes)
Goal: Test difficulty curve

Create balanced party
Complete Cave of Earth without items
Note: If too hard, balance needs adjustment
Try Water Temple at current level

Stress Test (45 minutes)
Goal: Find bugs and edge cases

Create party of all same class (e.g., 4 Knights)
Try to complete first two dungeons
Intentionally lose battles
Test flee mechanics extensively
Use all item types

Speed Run (60 minutes)
Goal: Complete game as fast as possible

Optimal party: Knight, Priest, Mage, Black Belt
Minimal grinding - just enough to survive
Direct path to each boss
Skip optional treasures
Time yourself!

Troubleshooting Party Wipes
If your party keeps dying:

Level up more - Grind in Cave of Earth
Use items - Don't hoard potions
Better composition - Ensure you have a healer (Priest)
Fight smarter - Target low-HP enemies first
Retreat strategically - Use stairs to exit and heal

Next Steps
Once you've completed a playthrough:

Try different party compositions
Experiment with solo runs (1-2 party members)
Test all job classes
Document any bugs found
Read PORTING_GUIDE.md for GameBoy port info

Getting Help
If you encounter issues:

Check the README.md for detailed information
Review this quickstart guide
Check the code comments
Create an issue with details about the problem

Have Fun!
This is a test framework, so expect some rough edges. The goal is to validate the game design before moving to GameBoy hardware. Enjoy exploring the dungeons!
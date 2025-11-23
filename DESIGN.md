# Dungeon Quest RPG - Game Design Document

**Version:** 1.0
**Target Platform:** GameBoy / GameBoy Color
**Genre:** Turn-based RPG / Dungeon Crawler
**Inspiration:** Final Fantasy Legend, Sword of Hope, Dragon Quest

---

## Table of Contents

1. [Game Overview](#game-overview)
2. [Character System](#character-system)
3. [Spell & Skill Progression](#spell--skill-progression)
4. [Combat System](#combat-system)
5. [Dungeon System](#dungeon-system)
6. [Treasure & Loot](#treasure--loot)
7. [Town System](#town-system)
8. [World Map](#world-map)
9. [Arena System](#arena-system)
10. [Equipment System](#equipment-system)
11. [Economy & Balance](#economy--balance)
12. [Story Structure](#story-structure)
13. [Technical Specifications](#technical-specifications)

---

## Game Overview

### Core Concept
A classic turn-based RPG where players create a party of 4 adventurers, explore dungeons, collect crystals, and defeat a final evil. The game features strategic combat, dungeon exploration with puzzles, town progression, and optional arena challenges.

### Target Completion Time
- **Main Story:** 8-12 hours (reach level 15-18)
- **100% Completion:** 15-20 hours (all side dungeons, arenas, level 20+)

### Key Features
- 6 job classes with unique skill trees
- Non-procedural, hand-crafted dungeons with puzzles
- 8 towns with fixed inventories and progression
- 12 dungeons (4 main + 8 side dungeons)
- Arena challenge system
- Two-tier treasure chest system (fixed + random)
- Smooth-scrolling camera with variable floor sizes
- World map exploration with random encounters

---

## Character System

### Job Classes (6 Total)

#### Physical Classes

**Knight** (Tank/Damage)
- **Role:** High HP/Defense tank, power attacks
- **Starting Stats:** HP 50, MP 10, ATK 12, DEF 10, INT 4, AGI 6, LUCK 5
- **Stat Growth:** High HP/DEF, moderate ATK
- **Starting Skills:** Power Strike, Shield Bash, Guard
- **Level-up Skills:**
  - Lv 5: Taunt

**Black Belt** (Monk/Fighter)
- **Role:** High damage fighter, self-sustain
- **Starting Stats:** HP 45, MP 15, ATK 14, DEF 7, INT 5, AGI 10, LUCK 6
- **Stat Growth:** High ATK/AGI, moderate HP
- **Starting Skills:** Focus Strike, Meditation
- **Level-up Skills:**
  - Lv 5: Counter Stance

**Thief** (Fast Attacker/Utility)
- **Role:** Fast attacks, steal items, status effects
- **Starting Stats:** HP 40, MP 12, ATK 10, DEF 6, INT 6, AGI 14, LUCK 12
- **Stat Growth:** High AGI/LUCK, low HP
- **Starting Skills:** Backstab, Steal, Poison Blade, Flash
- **Level-up Skills:**
  - Lv 5: Smoke Bomb

#### Magic Classes

**Mage** (Elemental Damage)
- **Role:** Highest magic damage, 3 elements, status effects
- **Starting Stats:** HP 30, MP 30, ATK 5, DEF 5, INT 15, AGI 8, LUCK 7
- **Stat Growth:** High INT/MP, very low HP
- **Starting Skills:** Fire, Ice
- **Level-up Skills:** (See [Spell Progression](#mage-spell-progression))

**Sage** (Jack-of-all-trades)
- **Role:** Balanced caster, damage + healing + status
- **Starting Stats:** HP 35, MP 25, ATK 7, DEF 6, INT 12, AGI 9, LUCK 8
- **Stat Growth:** Balanced INT/MP/HP
- **Starting Skills:** Fire, Cure
- **Level-up Skills:** (See [Spell Progression](#sage-spell-progression))

**Priest** (Healer/Support)
- **Role:** Best healer, party-wide healing, buffs
- **Starting Stats:** HP 38, MP 28, ATK 6, DEF 7, INT 13, AGI 7, LUCK 9
- **Stat Growth:** High MP/INT, moderate HP
- **Starting Skills:** Heal
- **Level-up Skills:** (See [Spell Progression](#priest-spell-progression))

### Party Composition
- **Party Size:** 4 members (fixed, no swapping)
- **Recommended Balance:** 1 Tank, 1 Healer, 1 Magic DPS, 1 Physical DPS
- **Example:** Knight, Priest, Mage, Black Belt

---

## Spell & Skill Progression

### Naming Convention
- **Elemental Spells:** Fire/Fire2/Fire3, Ice/Ice2/Ice3, Bolt/Bolt2/Bolt3
- **Healing Spells:** Cure/Cure2, Heal/Heal2/Heal3
- **Status/Buff Spells:** Consistent naming across all classes

### Mage Spell Progression

**Total Spells:** 15

| Level | Spell | MP Cost | Type | Description |
|-------|-------|---------|------|-------------|
| 1 | Fire | 5 | Attack | Basic fire damage |
| 1 | Ice | 5 | Attack | Basic ice damage |
| 3 | Bolt | 5 | Attack | Basic lightning damage |
| 5 | Slow | 6 | Status | Reduces enemy agility |
| 7 | Fire2 | 10 | Attack | Moderate fire damage |
| 7 | Silence | 7 | Status | Prevents enemy magic/skills |
| 9 | Ice2 | 10 | Attack | Moderate ice damage |
| 11 | Bolt2 | 10 | Attack | Moderate lightning damage |
| 13 | Toxic Cloud | 8 | Status | Poisons enemy (DOT) |
| 15 | Fire3 | 18 | Attack | Strong fire damage |
| 15 | Stone Gaze | 12 | Status | Petrifies enemy |
| 17 | Ice3 | 18 | Attack | Strong ice damage |
| 19 | Bolt3 | 18 | Attack | Strong lightning damage |
| 20 | Flare | 30 | Attack | Ultimate single-target nuke |

**Design Notes:**
- Learns fastest of all magic classes
- Gets all 3 elements at tier 3
- Heavy status effect utility
- Flare is endgame ultimate spell

### Priest Spell Progression

**Total Spells:** 8

| Level | Spell | MP Cost | Type | Description |
|-------|-------|---------|------|-------------|
| 1 | Heal | 5 | Healing | Single-target heal (30-50 HP) |
| 3 | Esuna | 8 | Healing | Cure status effects |
| 5 | Shell | 10 | Buff | Reduce magic damage taken |
| 7 | Protect | 10 | Buff | Reduce physical damage taken |
| 9 | Heal2 | 12 | Healing | Group heal (40-60 HP) |
| 12 | Prayer | 0 | Buff | Restore 10 MP per turn (3 turns) |
| 15 | Blinding Light | 10 | Status | Blind enemy (reduce accuracy) |
| 18 | Heal3 | 20 | Healing | Strong group heal (80-120 HP) |

**Design Notes:**
- Focused on healing and support
- Shell/Protect are defensive buffs (early access)
- Fewer spells than Mage, but all essential
- Prayer provides MP sustain

### Sage Spell Progression

**Total Spells:** 13

| Level | Spell | MP Cost | Type | Description |
|-------|-------|---------|------|-------------|
| 1 | Fire | 5 | Attack | Basic fire damage |
| 1 | Cure | 5 | Healing | Single-target heal (20-40 HP) |
| 4 | Ice | 5 | Attack | Basic ice damage |
| 4 | Bolt | 5 | Attack | Basic lightning damage |
| 6 | Tranquility | 0 | Buff | Restore 8 MP per turn (3 turns) |
| 8 | Fire2 | 10 | Attack | Moderate fire damage |
| 8 | Cure2 | 10 | Healing | Single-target heal (50-80 HP) |
| 10 | Ice2 | 10 | Attack | Moderate ice damage |
| 12 | Bolt2 | 10 | Attack | Moderate lightning damage |
| 14 | Time Warp | 12 | Status | Slow enemy (reduce AGI) |
| 16 | Mute | 10 | Status | Silence enemy |
| 18 | Petrify | 15 | Status | Stone enemy (cannot act) |
| 20 | Barrier | 12 | Buff | Reduce physical damage taken |

**Design Notes:**
- Jack-of-all-trades, master of none
- All 3 elements but caps at tier 2
- Healing caps at Cure2 (weaker than Priest)
- Learns slowest of magic classes
- High-tier status effects at endgame

---

## Combat System

### Battle Flow
1. Random encounter (world map or dungeon)
2. Turn order calculated by Agility stat
3. Player selects actions for party members
4. Actions execute in turn order (party + enemies)
5. Repeat until victory, defeat, or flee

### Action Types
- **Attack:** Basic physical attack
- **Skill/Magic:** Use learned skill or spell (costs MP)
- **Item:** Use consumable item from inventory
- **Defend:** Reduce damage taken this turn
- **Flee:** Attempt escape (50% chance, 0% vs bosses)

### Damage Calculation
- **Physical:** `(ATK * 2) - DEF + random(0, 5)`
- **Magic:** `(INT * spell_power) - enemy_INT/2 + random(0, 5)`
- **Critical Hit:** 5% base + (LUCK / 10)% bonus damage

### Status Effects
- **Poison:** 5-10% max HP damage per turn (4 turns)
- **Blind:** Reduces accuracy by 50%
- **Slow:** Reduces agility by 50%
- **Silence:** Cannot use magic/skills
- **Paralysis:** Cannot act (50% chance each turn)
- **Stone:** Cannot act (temporary petrify)
- **Dead:** 0 HP, cannot act

### Buffs & Debuffs
- **Protect/Shell/Barrier:** Reduce damage by 25-50%
- **Focus:** Increase INT by 20%
- **Prayer/Tranquility:** Restore MP over time (3 turns)
- **Slow/Time Warp:** Reduce enemy AGI by 50%

### Enemy Drops
- **Gold:** 5 + (enemy_level * 3) per enemy
- **EXP:** 10 + (enemy_level * 5) per enemy
- **Items:** 25% chance to drop consumables (if not stolen)
- **Steal Prevention:** Enemies stolen from don't drop items

---

## Dungeon System

### Dungeon Types

**Main Dungeons (4):**
1. **Cave of Earth** - Tutorial dungeon, earthy theme
2. **Water Temple** - Water/ice puzzles, sliding mechanics
3. **Volcano Keep** - Lava mazes, fire theme
4. **Sky Tower** - Cloud platforms, wind currents

**Final Dungeon:**
5. **Final Sanctum** - Unlocks after collecting 4 crystals, 5 floors

**Side Dungeons (7-8):**
- Smuggler's Cave (water-themed side dungeon)
- Ancient Forge (fire-themed side dungeon)
- Cloud Temple (sky-themed side dungeon)
- Pyramid of Trials (desert dungeon)
- Hidden Forest Shrine (forest dungeon)
- Crystal Cavern (optional hard dungeon)
- Underwater Grotto (requires special item?)
- Final Bonus Dungeon (post-game super-hard)

### Floor Size System

**Three Size Options:**

| Size | Dimensions | Tiles | Use Case |
|------|------------|-------|----------|
| Small | 16x16 | 256 | Simple rooms, boss arenas, early floors |
| Medium | 24x24 | 576 | Moderate exploration, simple puzzles |
| Large | 32x32 | 1024 | Mazes, complex puzzles, late-game |

**Viewport:**
- **Visible Area:** 18x16 tiles
- **Camera:** Smooth scrolling, follows player
- **Clamping:** Camera stops at map edges

### Floor Progression by Dungeon

**Cave of Earth:**
- Floor 0: 16x16 (Small) - Entry/tutorial
- Floor 1: 16x16 (Small) - Basic layout
- Floor 2: 24x24 (Medium) - Introduces scrolling
- Boss: 16x16 (Small) - Boss arena

**Water Temple:**
- Floor 0: 24x24 (Medium) - Water puzzle
- Floor 1: 24x24 (Medium) - Exploration
- Floor 2: 32x32 (Large) - Ice sliding maze
- Boss: 24x24 (Medium) - Arena with water

**Volcano Keep:**
- Floor 0: 24x24 (Medium)
- Floor 1: 32x32 (Large) - Lava maze
- Floor 2: 32x32 (Large) - Multi-path
- Boss: 24x24 (Medium) - Volcanic arena

**Sky Tower:**
- Floor 0: 32x32 (Large) - Cloud maze
- Floor 1: 32x32 (Large) - Wind puzzles
- Floor 2: 32x32 (Large) - Platforming
- Boss: 32x32 (Large) - Epic arena

**Final Sanctum:**
- Floors 0-3: 32x32 (Large) - All large
- Floor 4: 32x32 (Large) - Final boss arena

### Puzzle Mechanics

**Ice Sliding:**
- Player slides until hitting obstacle
- Multiple ice patches + rocks
- Goal: Navigate to stairs/treasure

**Teleporters:**
- Teleporter pads connect locations
- Must use in correct sequence
- Can create maze-like puzzles

**Locked Doors:**
- Colored keys scattered on floor
- Must collect keys to unlock doors
- Forces exploration

**Lava Mazes:**
- Narrow safe paths
- One-way doors
- Find correct route or backtrack

**Push Blocks:**
- Push blocks onto switches
- Opens doors or reveals stairs
- Classic Zelda-style puzzle

**One-Way Passages:**
- Doors lock behind player
- Must complete section before returning
- Creates tension

---

## Treasure & Loot

### Two Chest Types

**Gold Chests ($)** - Fixed Rewards
- Contains specific predetermined item
- Usually equipment (weapons, armor, accessories)
- One-time only (marked as opened in save file)
- Placed in specific locations (hand-designed)
- Purpose: Reward exploration, provide progression gear

**Silver Chests (c)** - Random Rewards
- Contains randomized loot
- Distribution: 40% gold, 30% consumables, 30% equipment
- Respawns on dungeon re-entry
- Purpose: Farming resources, gold income

### Chest Distribution

**Early Dungeons (Tier 1):**
- 1-2 chests per floor
- Ratio: 1 gold, 1-2 silver

**Mid Dungeons (Tier 2):**
- 2-3 chests per floor
- Ratio: 1-2 gold, 2 silver

**Late Dungeons (Tier 3-4):**
- 4-5 chests per floor
- Ratio: 2 gold, 3 silver

### Chest Rules
- **Obstacle:** All chests block movement (opened or not)
- **Visual Distinction:** Gold vs Silver sprites
- **Save Integration:** Opened gold chests tracked globally

### Gold Chest Examples

**Cave of Earth:**
- Floor 1: Bronze Sword
- Floor 2: Iron Helm
- Boss: Earth Crystal (key item)

**Water Temple:**
- Floor 1: Mithril Armor
- Floor 2: Agility Ring
- Boss: Water Crystal (key item)

**Volcano Keep:**
- Floor 1: Flame Sword
- Floor 2: Flame Shield
- Boss: Volcano Crystal (key item)

**Sky Tower:**
- Floor 1: Dragon Mail
- Floor 2: Guardian Amulet
- Boss: Sky Crystal (key item)

---

## Town System

### Main Towns (4)

**1. Greenleaf Village** (Starter Town)
- **Unlock:** Always accessible
- **Connected Dungeon:** Cave of Earth
- **Services:**
  - Inn: 50 Gold
  - Item Shop: Potion (50g), Antidote (30g), Tent (100g)
  - Equipment Shop: Bronze Sword, Bronze Axe, Dagger, Leather Armor, Leather Cap
  - Arena 1: Entry fee 10g
- **Story Role:** Tutorial town, party starts here

**2. Coral Bay** (Port Town)
- **Unlock:** Requires Earth Crystal
- **Connected Dungeon:** Water Temple
- **Access Gate:** Bridge guard: "Only those with the Earth Crystal may pass!"
- **Services:**
  - Inn: 75 Gold
  - Item Shop: Potion (50g), Hi-Potion (150g), Ether (200g), Antidote (30g)
  - Equipment Shop: Iron Sword, Iron Axe, Short Bow, Chain Mail, Iron Helm, Silver Ring
  - Arena 2: Entry fee 100g
- **Story Role:** Coastal town, water theme

**3. Emberforge** (Mountain Town)
- **Unlock:** Requires Water Crystal
- **Connected Dungeon:** Volcano Keep
- **Access Gate:** Mountain pass blocked by ice (melts with Water Crystal)
- **Services:**
  - Inn: 100 Gold
  - Item Shop: Hi-Potion (150g), Ether (200g), Tent (100g)
  - Equipment Shop: Flame Sword, Battle Axe, Long Bow, Dragon Scale, Steel Helm, Power Ring
  - Blacksmith: Weapon upgrades (+5 ATK for 500g)
  - Arena 3: Entry fee 200g
- **Story Role:** Mining/smithing town

**4. Windspire** (Sky Town)
- **Unlock:** Requires Volcano Crystal
- **Connected Dungeon:** Sky Tower
- **Access Gate:** Sky ladder/portal locked by magical seal
- **Services:**
  - Inn: 150 Gold
  - Item Shop: Ether (200g), Elixir (1000g), Tent (100g)
  - Equipment Shop: Crystal Blade, Holy Lance, Mystic Robe, Dragon Helm, Guardian Amulet
  - Sage's Tower: Lore about final dungeon
  - Arena 4: Entry fee 500g
- **Story Role:** Floating island town, final main dungeon

### Side Towns (4)

**5. Crossroads Market** (Trade Hub)
- **Unlock:** Always accessible
- **Location:** Central area between main towns
- **Services:**
  - Rotating Bazaar: Random Tier 2-3 equipment (changes weekly)
  - Item Shop: All consumables (+50% markup)
  - Merchant: Buy at 150% price, sell at 75% (better rates)
- **Purpose:** Alternative shopping, hints

**6. Sandfall Oasis** (Desert Town)
- **Unlock:** Requires 1+ Crystal (sandstorm clears)
- **Location:** Desert region
- **Services:**
  - Inn: 75 Gold
  - Item Shop: Hi-Potion (150g), Ether (200g), Remedy (500g)
  - Equipment Shop: Scimitar, Desert Staff, Desert Robes, Thief's Glove
  - Tavern: Hints about Pyramid of Trials
- **Purpose:** Gateway to desert side dungeons

**7. Elderwood Grove** (Hidden Forest Village)
- **Unlock:** Hidden path from Greenleaf (requires exploration)
- **Location:** Secret forest area
- **Services:**
  - Inn: 50 Gold (cheapest!)
  - Apothecary: Phoenix Down (1500g), Elixir (1000g), Remedy (500g)
  - Elder: Lore/secrets, rare accessory quest
- **Purpose:** Hidden treasure, special items

**8. Scholar's Outpost** (Ruins Camp)
- **Unlock:** Requires 3+ Crystals (ruins entrance unsealed)
- **Location:** Ancient ruins
- **Services:**
  - Item Shop: Ether (200g), Elixir (1000g), Mega Elixir (5000g)
  - Scholar: Endgame lore, hints about ultimate weapons
  - Arena 5: Entry fee 1000g (Ultimate Challenge)
- **Purpose:** Gateway to super-hard dungeon

### Town Progression

**Crystal-Gated Access:**
- **0 Crystals:** Greenleaf Village, Crossroads Market
- **1 Crystal:** Coral Bay, Sandfall Oasis
- **2 Crystals:** Emberforge
- **3 Crystals:** Windspire, Scholar's Outpost
- **Secret:** Elderwood Grove (hidden path)

---

## World Map

### Map Layout

**Concept:**
- Top-down tile-based map
- Player sprite moves on map (8-directional or 4-directional)
- Towns represented by building icons
- Dungeons represented by cave/tower icons
- Terrain types: Grass, water, mountains, desert, forest

**Map Size:**
- **Option A:** 128x128 tiles (larger world)
- **Option B:** 64x64 tiles (smaller, tighter design)
- **Recommendation:** 64x64 for GameBoy memory constraints

### Random Encounters

**Encounter Rate:**
- Every 15-30 steps on world map
- Enemy level scales with region
- Can flee from encounters (50% chance)

**Enemy Regions:**
- **Starter Region (Greenleaf):** Level 1-3 enemies
- **Port Region (Coral Bay):** Level 4-7 enemies
- **Mountain Region (Emberforge):** Level 8-12 enemies
- **Sky Region (Windspire):** Level 13-16 enemies
- **Desert/Forest Regions:** Level 5-10 enemies (mixed)

### Fast Travel

**System:**
- Visit a town once to unlock it on map
- Can warp to any unlocked town from menu
- OR: Walk to town entrances to enter

---

## Arena System

### Arena Progression

**Arena 1: Greenleaf Village Arena**
- **Unlock:** Available from start
- **Entry Fee:** 10 Gold
- **Challenge:** 3 waves of low-level monsters (Lv 3-5)
- **Reward:** 500 Gold + Bronze Shield
- **Net Profit:** +490 Gold
- **Unlocks:** Arena 2

**Arena 2: Coral Bay Coliseum**
- **Unlock:** Beat Arena 1 + reach Coral Bay
- **Entry Fee:** 100 Gold
- **Challenge:** 4 waves of mid-level monsters (Lv 7-9)
- **Reward:** 1000 Gold + Silver Ring
- **Net Profit:** +900 Gold
- **Unlocks:** Arena 3

**Arena 3: Emberforge Battle Pit**
- **Unlock:** Beat Arena 2 + reach Emberforge
- **Entry Fee:** 200 Gold
- **Challenge:** 5 waves including mini-boss (Lv 12-14)
- **Reward:** 2000 Gold + Flame Shield
- **Net Profit:** +1800 Gold
- **Unlocks:** Arena 4

**Arena 4: Windspire Grand Arena**
- **Unlock:** Beat Arena 3 + reach Windspire
- **Entry Fee:** 500 Gold
- **Challenge:** 5 waves + Champion Boss (Lv 18-20)
- **Reward:** 5000 Gold + Champion's Belt (+10 all stats)
- **Net Profit:** +4500 Gold
- **Unlocks:** Arena 5

**Arena 5: Ultimate Challenge**
- **Unlock:** Beat Arena 4 + collect all 4 crystals
- **Entry Fee:** 1000 Gold
- **Challenge:** Gauntlet - 10 waves, no healing between (Lv 20+)
- **Reward:** Ultimate Weapon (class-specific) + Ribbon (status immunity)
- **Purpose:** Endgame challenge

### Arena Rules

- **No Items:** Cannot use consumables during fights
- **Skills/Magic Only:** Party must rely on MP management
- **No Fleeing:** Must win or lose
- **Healing Between Waves:** Party fully healed (except Arena 5)
- **Retry Allowed:** Can pay entry fee to retry if defeated
- **One-Time Rewards:** Equipment rewards only given on first clear

---

## Equipment System

### Equipment Tiers (4 Tiers)

**Tier 1: Starting Gear** (Levels 1-5)
- Found in: Greenleaf Village shop, Cave of Earth
- Examples: Bronze Sword, Leather Armor, Leather Cap
- Stat Range: +5 to +10 ATK/DEF

**Tier 2: Mid-Game Gear** (Levels 6-10)
- Found in: Coral Bay shop, Water Temple chests
- Examples: Iron Sword, Chain Mail, Iron Helm, Silver Ring
- Stat Range: +10 to +20 ATK/DEF

**Tier 3: Late-Game Gear** (Levels 11-16)
- Found in: Emberforge shop, Volcano/Sky dungeons
- Examples: Flame Sword, Dragon Scale, Steel Helm, Power Ring
- Stat Range: +20 to +35 ATK/DEF

**Tier 4: Legendary Gear** (Levels 17-20)
- Found in: Windspire shop, Final Sanctum chests, Arena rewards
- Examples: Crystal Blade, Mystic Robe, Dragon Helm, Guardian Amulet
- Stat Range: +35 to +50 ATK/DEF

### Equipment Slots (4 per character)

1. **Weapon:** Increases ATK (or INT for staves)
2. **Armor:** Increases DEF
3. **Helmet:** Increases DEF + minor stat bonuses
4. **Accessory:** Stat bonuses (INT, AGI, LUCK, etc.)

### Job Restrictions

- **Knight:** All weapons, heavy armor
- **Black Belt:** Light weapons (claws, staffs), light armor
- **Thief:** Daggers, bows, light armor
- **Mage:** Staves, robes
- **Sage:** Staves, robes
- **Priest:** Staves, robes

### Special Equipment

**Accessories:**
- **Silver Ring:** +5 INT
- **Power Ring:** +5 ATK
- **Agility Ring:** +10 AGI
- **Thief's Glove:** +10 LUCK
- **Guardian Amulet:** +10 DEF
- **Champion's Belt:** +10 all stats (Arena 4 reward)
- **Ribbon:** Status immunity (Arena 5 reward)

**Ultimate Weapons (Arena 5 rewards):**
- **Knight:** Excalibur (+50 ATK)
- **Black Belt:** Dragon Claw (+45 ATK, +10 AGI)
- **Thief:** Assassin's Dagger (+40 ATK, +15 LUCK)
- **Mage:** Sage Staff (+50 INT, +20 MP)
- **Sage:** Mystic Rod (+40 INT, +15 MP)
- **Priest:** Holy Staff (+45 INT, +10 DEF)

---

## Economy & Balance

### Gold Economy

**Gold Cap:** 9,999 Gold

**Income Sources:**
- Battle rewards: 5 + (enemy_level * 3) per enemy
- Boss rewards: boss_level * 100
- Treasure chests: 50-500 Gold (scales with dungeon)
- Arena rewards: 500-5000 Gold
- Selling items: 50% of purchase price

**Major Expenses:**
- Inn: 50-150 Gold per rest
- Equipment: 100-2000 Gold per piece (tier-dependent)
- Consumables: 50-1000 Gold (Potion to Elixir)
- Arena fees: 10-1000 Gold
- Weapon upgrades: 500 Gold (Emberforge blacksmith)

**Gold Progression:**
- Early game (Lv 1-5): 500-1000 Gold total
- Mid game (Lv 6-10): 2000-4000 Gold
- Late game (Lv 11-16): 5000-8000 Gold
- Endgame (Lv 17-20): 9999 Gold (cap)

### Experience Progression

**EXP Formula:** 100 * level²

| Level | EXP Required | Cumulative EXP |
|-------|--------------|----------------|
| 1→2 | 100 | 100 |
| 2→3 | 400 | 500 |
| 3→4 | 900 | 1,400 |
| 5→6 | 2,500 | 5,500 |
| 10→11 | 10,000 | 38,500 |
| 15→16 | 22,500 | 120,500 |
| 19→20 | 36,100 | 247,000 |

**Target Levels:**
- **Cave of Earth:** Level 3-5
- **Water Temple:** Level 7-9
- **Volcano Keep:** Level 11-13
- **Sky Tower:** Level 15-17
- **Final Sanctum:** Level 18-20
- **Post-Game Content:** Level 22-25

### Item Costs

**Consumables:**
- Potion (restore 50 HP): 50g
- Hi-Potion (restore 150 HP): 150g
- Ether (restore 30 MP): 200g
- Elixir (restore full HP/MP): 1000g
- Antidote (cure poison): 30g
- Tent (full party heal): 100g
- Remedy (cure all status): 500g
- Phoenix Down (revive): 1500g
- Mega Elixir (full party HP/MP): 5000g

**Equipment (Sample Prices):**
- Bronze Sword (Tier 1): 100g
- Iron Sword (Tier 2): 500g
- Flame Sword (Tier 3): 1500g
- Crystal Blade (Tier 4): 3000g

---

## Story Structure

### Main Story Arc

**Act 1: The Awakening** (Levels 1-5)
- Party forms in Greenleaf Village
- Elder warns of ancient evil awakening
- First crystal lost in Cave of Earth
- Tutorial dungeon, learn mechanics
- Defeat Earth Guardian, claim Earth Crystal

**Act 2: The Spreading Darkness** (Levels 6-13)
- Travel to Coral Bay (bridge unlocked)
- Water Temple threatened by ice corruption
- Defeat Water Guardian, claim Water Crystal
- Emberforge mountain pass opens
- Volcano Keep erupting from dark magic
- Defeat Fire Guardian, claim Volcano Crystal

**Act 3: The Sky Fortress** (Levels 14-17)
- Windspire floating island accessible
- Sky Tower under attack by winged demons
- Defeat Sky Guardian, claim Sky Crystal
- Four crystals unlock Final Sanctum
- Elder reveals ancient prophecy

**Act 4: The Final Battle** (Levels 18-20)
- Final Sanctum entrance opens
- Five floors of increasing difficulty
- Face corrupted guardians (mini-bosses)
- Final boss: Ancient Evil (multi-phase fight)
- Restore balance, crystals return to temples
- Ending cutscene, credits

### Side Story Content

**Arena Questline:**
- Mysterious tournament organizer
- Each arena victory unlocks next location
- Ultimate Challenge reveals secret about final boss

**Hidden Village Quest:**
- Find Elderwood Grove
- Elder teaches ancient techniques
- Unlock rare accessory or skill

**Desert Mystery:**
- Sandfall Oasis NPCs hint at Pyramid
- Ancient treasure buried within
- Solve pyramid puzzles for legendary gear

**Blacksmith Sidequests:**
- Emberforge blacksmith needs materials
- Collect rare ores from dungeons
- Upgrade weapons beyond normal limits

---

## Technical Specifications

### GameBoy Hardware Constraints

**Memory:**
- **RAM:** 8 KB total (must fit all game state)
- **ROM:** 32-512 KB (cart size dependent)
- **VRAM:** 8 KB for tile data

**Graphics:**
- **Sprites:** 8x8 or 8x16 pixels
- **Tiles:** 8x8 pixels
- **Colors:** 4 shades of gray (GB) or 4 colors per palette (GBC)
- **Screen:** 160x144 pixels = 20x18 tiles

**Performance:**
- **CPU:** 4.19 MHz (very slow!)
- **Frame Rate:** 60 FPS target
- **Sound:** 4 channels (2 square, 1 wave, 1 noise)

### Data Type Limits

```c
// Character stats
uint8_t level;           // Max 255 (cap at 25)
uint16_t current_hp;     // Max 65535 (cap at 999)
uint16_t max_hp;         // Max 65535 (cap at 999)
uint8_t current_mp;      // Max 255
uint8_t max_mp;          // Max 255
uint8_t stats[5];        // ATK/DEF/INT/AGI/LUCK - Max 255 each
uint16_t gold;           // Max 65535 (cap at 9999 for display)
uint32_t experience;     // Max 4.2 billion (more than enough)

// Dungeon floor
uint8_t width;           // 16, 24, or 32
uint8_t height;          // 16, 24, or 32
DungeonTile tiles[32][32]; // 1024 tiles max (1 KB)

// Party
PartyMember members[4];  // Fixed party size
Inventory inventory;     // 20 items + 20 equipment (static)
```

### Memory Budget (Estimated)

| System | Size | Notes |
|--------|------|-------|
| Party (4 members) | ~800 bytes | Stats, skills, buffs |
| Inventory | ~600 bytes | 40 item slots |
| Game State | ~200 bytes | Flags, crystals, gold |
| Current Dungeon Floor | ~1200 bytes | 32x32 tiles + metadata |
| Battle State | ~400 bytes | Enemies, turn order |
| **Total** | ~3200 bytes | ~40% of 8KB RAM |

Remaining RAM for:
- Code stack
- Temporary buffers
- Screen rendering
- Audio buffers

### GameBoy-Specific Optimizations

**No Dynamic Memory:**
- All `malloc`/`free` removed
- Static allocation only
- Fixed-size arrays

**Input Abstraction:**
- `InputButton` enum maps to GB buttons
- `input_get_key()` handles D-pad, A, B, Start, Select

**Display Abstraction:**
- PC version: ASCII text rendering
- GB version: Tile-based graphics
- `display_*` functions in utils.c

**Fixed-Point Math:**
- No floating-point operations
- All calculations use integers
- Damage formulas optimized for integer math

### Save System

**Save Data Structure:**
```c
typedef struct {
    Party party;                    // 4 characters
    Inventory inventory;            // Items + equipment
    uint16_t gold;                  // Current gold
    uint8_t crystals_collected;     // Bitfield: 0x0F = all 4
    OpenedChest opened_chests[64];  // Fixed chests opened
    uint8_t opened_chest_count;
    uint8_t arena_progress;         // Bitfield: Arena 1-5 completed
    uint8_t current_dungeon_id;
    uint8_t current_floor_id;
    // Story flags, NPC flags, etc.
} SaveData;
```

**Size:** ~2-3 KB per save file

**GB Implementation:**
- Save to SRAM (battery-backed RAM)
- Multiple save slots (3 slots)
- Checksum validation

---

## Implementation Priorities

### Phase 1: Core Systems (PC Version)
1. ✅ Battle system (turn-based, actions, damage)
2. ✅ Party system (stats, skills, leveling)
3. ✅ Inventory system (items, equipment)
4. ✅ Basic dungeon exploration (16x16 fixed size)
5. ✅ Save/Load system

### Phase 2: Enhanced Features (PC Version)
1. 🔲 Variable floor sizes (16x16, 24x24, 32x32)
2. 🔲 Viewport/camera system (18x16 visible area)
3. 🔲 Two-tier treasure chest system (gold + silver)
4. 🔲 Town system with fixed inventories
5. 🔲 World map with random encounters
6. 🔲 Arena challenge system
7. 🔲 Spell progression system (level-gated learning)

### Phase 3: Content Creation (PC Version)
1. 🔲 Hand-craft 4 main dungeons
2. 🔲 Design 7-8 side dungeons
3. 🔲 Create 8 towns with shops/NPCs
4. 🔲 Design world map layout
5. 🔲 Write story dialogue
6. 🔲 Balance testing (levels, gold, damage)

### Phase 4: GameBoy Port
1. 🔲 Graphics conversion (tiles, sprites)
2. 🔲 Input mapping (D-pad, A/B buttons)
3. 🔲 Display rendering (tile-based)
4. 🔲 Memory optimization
5. 🔲 Audio implementation (4-channel sound)
6. 🔲 Save system (SRAM)
7. 🔲 Final testing on hardware

---

## Design Philosophy

### Core Pillars

**1. Classic RPG Feel**
- Turn-based combat (no action elements)
- Strategic party composition
- Exploration rewarded with treasure
- Level progression feels meaningful

**2. GameBoy-First Design**
- All features must work on GameBoy hardware
- No dynamic memory allocation
- Tile-based graphics and world
- Simple, clean UI

**3. Meaningful Choices**
- Job class selection matters (no job changing)
- Fixed town inventories (must visit specific towns)
- Resource management (MP, items, gold)
- Risk/reward (arena fees, dungeon difficulty)

**4. Respect Player Time**
- Clear progression path (crystal gates)
- Save anywhere in dungeons (camp menu)
- Fast travel unlocks after town visits
- No excessive grinding required

**5. Exploration Encouraged**
- Gold chests reward exploration
- Side dungeons have unique gear
- Hidden town has special items
- Secrets and optional content

---

## Glossary

**Terms:**
- **Crystal:** Key item obtained from main dungeon bosses
- **Arena:** Optional combat challenge for rewards
- **Tier:** Equipment quality level (1-4)
- **Viewport:** Visible portion of larger dungeon floor
- **Gold Chest:** Fixed, one-time treasure
- **Silver Chest:** Random, respawning treasure
- **Smooth Scrolling:** Camera follows player pixel-by-pixel

**Abbreviations:**
- **HP:** Hit Points (health)
- **MP:** Magic Points (mana)
- **ATK:** Attack (physical damage stat)
- **DEF:** Defense (physical resistance)
- **INT:** Intelligence (magic power)
- **AGI:** Agility (turn order, evasion)
- **LUCK:** Luck (critical hits, steal success)
- **EXP:** Experience Points
- **GB:** GameBoy
- **GBC:** GameBoy Color
- **SRAM:** Save RAM (battery-backed)
- **DOT:** Damage Over Time

---

**End of Design Document**

*Last Updated: 2025-01-22*
*Version: 1.0 - Initial Planning Phase Complete*

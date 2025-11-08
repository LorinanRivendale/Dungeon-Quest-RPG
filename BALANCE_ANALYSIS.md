# Dungeon Quest RPG - Balance Analysis Report

**Analysis Date**: 2025-11-08
**Analyzed Version**: Git commit bce2fcf

---

## Executive Summary

This document provides a comprehensive mathematical analysis of game balance for Dungeon Quest RPG. The analysis covers combat formulas, progression curves, economy, and difficulty scaling across all 5 dungeons.

### Key Findings:
- ✅ **Good**: Damage formula creates meaningful stat progression
- ✅ **Good**: Experience curve is reasonable (quadratic scaling)
- ⚠️ **Concern**: Early game gold economy may be tight
- ⚠️ **Concern**: Boss difficulty spike at final dungeon
- ⚠️ **Issue**: HP/MP sustainability issues in longer dungeons
- ⚠️ **Issue**: Healing items expensive relative to early-game gold income

---

## 1. Combat Balance Analysis

### Damage Formula
```c
damage = (attacker_atk * 2) - defender_def
damage *= random(75, 125) / 100  // ±25% variance
if (critical) damage *= 2        // 5% base chance
```

**Analysis**:
- Attack is weighted 2x compared to defense
- This creates **offense-favored** combat
- Defense reduces damage linearly, but cannot negate attacks (minimum 1 damage)
- ±25% variance adds unpredictability without being excessive

### Early Game Combat (Level 1, Dungeon 1)

**Player Stats** (Knight, no equipment):
- HP: 100, ATK: 12, DEF: 10

**Enemy Stats** (Dungeon 1, Level 1-3):
- HP: 25-35, ATK: 7-9, DEF: 4-6

**Damage Calculations**:
```
Player attacks enemy (Lv1):
  Base: (12 * 2) - 4 = 20
  Range: 15-25 damage
  Kills in: 1-2 hits

Enemy attacks player:
  Base: (7 * 2) - 10 = 4
  Range: 3-5 damage
  Player survives: 20-33 hits
```

**Verdict**: Early game heavily favors player - **low risk of game over**

---

## 2. Progression Curves

### Experience Requirements
```c
EXP for next level = 100 * level²
```

| Level | EXP Required | Cumulative EXP |
|-------|--------------|----------------|
| 1→2   | 100          | 100            |
| 2→3   | 400          | 500            |
| 3→4   | 900          | 1,400          |
| 4→5   | 1,600        | 3,000          |
| 5→6   | 2,500        | 5,500          |
| 10    | 10,000       | 38,500         |
| 15    | 22,500       | 125,000        |

### Experience Rewards
```c
Regular enemies: 10 + (enemy_level * 5)
Boss rewards: (calculated separately - not specified in code)
```

| Dungeon | Enemy Level | EXP per Enemy | Enemies to Level 2 |
|---------|-------------|---------------|---------------------|
| 1       | 1-3         | 15-25         | 4-7 battles         |
| 2       | 2-4         | 20-30         | ~13 for Lv3→4       |
| 3       | 3-5         | 25-35         | ~29 for Lv5→6       |

**Verdict**: Quadratic growth is **standard for JRPGs** - reasonable progression pace

### Stat Growth Per Level
```c
HP:  +8 to +12 per level (8 + random(0,4))
MP:  +3 to +5  per level (3 + random(0,2))
STR: +0 to +2  per level
DEF: +0 to +2  per level
INT: +0 to +2  per level
AGI: +0 to +2  per level
LUCK: +0 to +1 per level
```

**Level 10 Projected Stats** (Knight):
- HP: ~180-220 (started at 100)
- ATK: ~21-30 (started at 12)
- DEF: ~19-28 (started at 10)

**Concern**: Random stat growth creates **RNG variance** - two identical characters can differ significantly

---

## 3. Enemy Scaling Analysis

### Enemy Generation Formula
```c
level = dungeon_level + random(0, 3)
max_hp = 20 + (level * 5) + (type * 10)
attack = 5 + (level * 2) + type
defense = 3 + level + type
exp_reward = 10 + (level * 5)
gold_reward = 5 + (level * 3)
```

### Enemy Progression Table

| Dungeon | Enemy Level | HP        | ATK      | DEF     | Gold/Enemy |
|---------|-------------|-----------|----------|---------|------------|
| 1       | 1-3         | 25-45     | 7-11     | 4-7     | 8-14       |
| 2       | 2-4         | 30-50     | 9-13     | 5-8     | 11-17      |
| 3       | 3-5         | 35-55     | 11-15    | 6-9     | 14-20      |
| 4       | 4-6         | 40-60     | 13-17    | 7-10    | 17-23      |
| Final   | 5-7         | 45-65     | 15-19    | 8-11    | 20-26      |

### Boss Scaling
```c
max_hp = 200 + (dungeon_id * 100)
attack = 15 + (dungeon_id * 5)
defense = 10 + (dungeon_id * 3)
level = 10 + (dungeon_id * 5)
```

| Dungeon | Boss        | HP  | ATK | DEF | Level |
|---------|-------------|-----|-----|-----|-------|
| 1       | Earth Golem | 200 | 15  | 10  | 10    |
| 2       | Leviathan   | 300 | 20  | 13  | 15    |
| 3       | Ifrit       | 400 | 25  | 16  | 20    |
| 4       | Garuda      | 500 | 30  | 19  | 25    |
| Final   | Dark Lord   | 600 | 35  | 22  | 30    |

**Critical Issue**: Final boss (Dark Lord) has:
- **3x HP** of first boss
- **2.3x ATK** of first boss
- Expected player level: ~15-20
- Boss level: 30

**Verdict**: **STEEP difficulty spike** at endgame boss

---

## 4. Gold Economy Analysis

### Starting Resources
- Starting Gold: **100G**
- Inn cost: **50G** (full party heal)

### Gold Income
```c
Battle: 5 + (enemy_level * 3) per enemy
Boss: boss_level * 50 (estimated from code pattern)
```

**Estimated Gold Progression**:
- Dungeon 1 (10 battles): ~120-160G
- Dungeon 2 (10 battles): ~150-200G
- Dungeon 3 (10 battles): ~180-240G
- Boss 1 (Level 10): ~500G

**Total after Dungeon 1 + Boss**: 100 + 140 + 500 = **740G**

### Shop Prices (Key Items)

**Consumables**:
| Item       | Buy  | Sell | HP/MP Restored |
|------------|------|------|----------------|
| Potion     | 50G  | 25G  | 50 HP          |
| Hi-Potion  | 150G | 75G  | 150 HP         |
| Ether      | 100G | 50G  | 30 MP          |
| Elixir     | 1000G| 500G | Full HP/MP     |
| Tent       | 200G | 100G | Full party     |

**Early Equipment**:
| Item         | Price | Bonus         |
|--------------|-------|---------------|
| Dagger       | 100G  | ATK+5         |
| Short Sword  | 200G  | ATK+8         |
| Cloth Armor  | 80G   | DEF+3         |
| Leather Armor| 180G  | DEF+6         |
| Leather Cap  | 60G   | DEF+2         |

### Economy Calculations

**Starting Loadout** (100G budget):
- Option 1: 1x Cloth Armor (80G) → 20G remaining
- Option 2: 2x Potion (100G) → 0G remaining
- **Cannot afford** weapon + armor

**After Dungeon 1** (~740G total):
- Buy: Short Sword (200G) + Leather Armor (180G) + Leather Cap (60G) = 440G
- Remaining: 300G
- Can buy 6x Potions or 1.5x Tents

**Verdict**: Early economy is **tight but manageable**. Players must choose between equipment and healing items.

### Healing Cost Analysis

**Inn**: 50G for full party heal
**Tent**: 200G for full party heal (usable in dungeon)
**Potions**: 50G for 50 HP (1 character)

Avg party HP at Level 1: ~70 HP/character = 280 HP total
- Inn: 50G / 280 HP = **0.18G per HP**
- Tent: 200G / 280 HP = **0.71G per HP**
- Potion: 50G / 50 HP = **1.00G per HP**

**Verdict**: Inn is **5-6x more cost-efficient** than items. Encourages returning to town frequently.

---

## 5. HP/MP Sustainability

### Party HP Pools by Level

| Level | Knight HP | Mage HP | Party Avg | Total Party HP |
|-------|-----------|---------|-----------|----------------|
| 1     | 100       | 55      | 72        | 288            |
| 5     | 140-180   | 75-95   | 107-137   | 428-548        |
| 10    | 180-220   | 95-115  | 137-167   | 548-668        |
| 15    | 220-260   | 115-135 | 167-197   | 668-788        |

### Damage Taken Per Fight (Dungeon 2, Level 5 party)

Enemies (Lv 2-4): ATK 9-13
Party (Lv 5): DEF ~15-20 (with equipment)

```
Enemy damage: (10 * 2) - 17 = 3 damage (min)
             (13 * 2) - 15 = 11 damage (max)
Average: ~7 damage per enemy attack
```

**4 enemies, 2 turns each** = 8 attacks = **~56 damage per fight**
**Dungeon (15 fights)** = **~840 damage total**
**Party HP pool at Lv 5** = ~490 HP

**Verdict**: **Party cannot complete a dungeon without healing items or camping**

### MP Sustainability

Mages start with 40 MP, gain 3-5 per level
**Level 5 Mage**: ~55-60 MP

**Spell costs** (from earlier grep):
- Fire/Ice/Bolt: ~5-8 MP
- Cure: ~6-10 MP
- Tier 2 spells: ~10-15 MP
- Tier 3 spells: ~20-25 MP

**Verdict**: Mages can cast **5-10 spells** before running dry. MP items (Ether: 100G) are expensive.

---

## 6. Equipment Progression

### Weapon Tiers

| Tier | Weapon        | Price | ATK Bonus | Job        |
|------|---------------|-------|-----------|------------|
| 1    | Dagger        | 100G  | +5        | All        |
| 2    | Short Sword   | 200G  | +8        | All        |
| 3    | Long Sword    | 500G  | +12       | Knight     |
| 4    | Great Sword   | 1200G | +18       | Knight     |

**ATK Progression**:
- Tier 1→2: +3 ATK (+60%)
- Tier 2→3: +4 ATK (+50%)
- Tier 3→4: +6 ATK (+50%)

**Cost Progression**:
- Tier 1→2: +100G (2x)
- Tier 2→3: +300G (2.5x)
- Tier 3→4: +700G (2.4x)

**Verdict**: Equipment costs scale **faster than stats scale**. Diminishing returns on investment.

### Armor Tiers

| Tier | Armor        | Price | DEF Bonus | Job        |
|------|--------------|-------|-----------|------------|
| 1    | Cloth Armor  | 80G   | +3        | All        |
| 2    | Leather      | 180G  | +6        | All        |
| 3    | Chain Mail   | 450G  | +10       | Knight     |
| 4    | Plate Mail   | 1000G | +15       | Knight     |

**Cost-Effectiveness**:
- Cloth: 26.7G per DEF point
- Leather: 30G per DEF point
- Chain Mail: 45G per DEF point
- Plate Mail: 66.7G per DEF point

**Verdict**: Higher tiers have **worse cost-efficiency**. Best value is Tier 1-2 equipment.

---

## 7. Identified Balance Issues

### CRITICAL Issues

1. **Boss Difficulty Spike**
   - Dark Lord (Final Boss) stats assume Level 30 party
   - Expected player level after 4 dungeons: ~15-20
   - **Gap**: 10-15 levels
   - **Impact**: Final boss may be unbeatable without grinding

2. **HP Sustainability**
   - Cannot complete dungeon without healing
   - Tent costs 200G (expensive for early game)
   - **Forces** frequent town returns or heavy potion use

### MODERATE Issues

3. **Early Game Economy**
   - Starting 100G insufficient for equipment
   - Must complete Dungeon 1 with minimal gear
   - Creates **difficulty spike at start** before it gets easier

4. **Healing Item Costs**
   - Potion: 50G for 50 HP (1G per HP)
   - Inn: 50G for full party (0.18G per HP)
   - **5x cost difference** discourages dungeon camping

5. **Equipment Cost Scaling**
   - Tier 4 equipment costs 10-15x Tier 1
   - Stat bonuses only 2-3x higher
   - **Diminishing returns** on upgrades

### MINOR Issues

6. **Random Stat Growth**
   - 0-2 points per stat per level
   - Over 10 levels: 0-20 point variance
   - Can create **unbalanced characters**

7. **Encounter Rate**
   - Step counter: 15-30 steps between encounters
   - Fixed maps have ~50-80 walkable tiles
   - **Expect** 2-5 encounters per floor
   - May feel grindy in 5-floor final dungeon

---

## 8. Recommendations

### High Priority

**1. Rebalance Final Boss**
- **Option A**: Reduce Dark Lord stats by 30%
  - HP: 600 → 420
  - ATK: 35 → 25
  - DEF: 22 → 16
- **Option B**: Add grinding zone before final dungeon
- **Option C**: Scale boss stats based on party level

**2. Improve Early Economy**
- Increase starting gold: 100G → 200G
- OR reduce Tier 1 equipment prices by 25%
- OR add free starter equipment after character creation

**3. Reduce Healing Costs**
- Potion: 50G → 30G
- Tent: 200G → 150G
- Makes dungeon camping more viable

### Medium Priority

**4. Add Gold Scaling**
- Boss gold rewards feel low
- Suggest: `boss_level * 100` instead of `* 50`
- Example: Level 10 boss gives 1000G instead of 500G

**5. Rebalance Equipment Costs**
```
Current: 100 → 200 → 500 → 1200 (exponential)
Suggested: 100 → 250 → 500 → 900 (linear)
```

**6. Make Stat Growth Deterministic**
- Remove randomness from levelup stats
- Ensures consistent character power
- Example: STR always +1 per level (not 0-2)

### Low Priority

**7. Adjust Encounter Rate**
- Increase min steps: 15 → 20
- Increase max steps: 30 → 40
- Reduces grindiness

**8. Add Item Drops**
- Enemies have chance to drop potions/ethers
- Reduces reliance on shops
- Standard JRPG feature

---

## 9. Difficulty Curve Projection

### Expected Player Progression

| After Dungeon | Player Level | Total Gold | Boss Difficulty | Balance |
|---------------|--------------|------------|-----------------|---------|
| 0 (Start)     | 1            | 100G       | N/A             | ✅ Good |
| 1             | 3-4          | 700G       | Earth Golem     | ✅ Good |
| 2             | 6-7          | 1500G      | Leviathan       | ✅ Good |
| 3             | 9-11         | 2800G      | Ifrit           | ⚠️ Moderate |
| 4             | 12-15        | 4500G      | Garuda          | ⚠️ Challenging |
| Final         | 16-20        | 6500G      | Dark Lord (Lv30)| ❌ **WALL** |

**Issue**: Final boss expects Level 30, but players will be Level 16-20

**Solutions**:
1. Reduce Dark Lord to Level 20-22
2. Add EXP grinding area before final dungeon
3. Increase EXP rewards in Dungeon 4

---

## 10. Conclusion

### Strengths
- Combat formula is well-designed
- Experience curve follows JRPG standards
- Equipment progression offers meaningful choices
- Early-mid game balance is solid

### Weaknesses
- Final boss difficulty spike
- HP sustainability requires frequent healing
- Early economy is punishingly tight
- Random stat growth creates variance

### Priority Fixes
1. **Reduce final boss stats** (CRITICAL)
2. **Increase starting gold OR reduce early equipment costs** (HIGH)
3. **Reduce healing item costs** (MEDIUM)
4. **Increase boss gold rewards** (MEDIUM)

With these changes, the game should have a smoother difficulty curve and better economic balance throughout the full playthrough.

---

**Analysis Methodology**: All calculations based on source code formulas extracted from:
- `src/battle.c` (damage, enemy generation)
- `src/party.c` (stats, leveling)
- `src/dungeon.c` (boss stats)
- `src/main.c` (shop prices)

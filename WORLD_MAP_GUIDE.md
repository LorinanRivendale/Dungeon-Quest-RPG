# Dungeon Quest RPG - World Map Guide

**Status:** ✅ COMPLETE (Phase 1 MVP)
**Map Size:** 128×128 tiles (16 KB tile data + 16 KB zones = 32 KB total)
**Tileset:** Final Fantasy NES Overworld Tileset
**Created:** 2025-01-23
**Updated:** 2025-01-23 (Expanded to 128×128)

---

## Overview

The world map (`SRC/world_map.h` and `SRC/world_map.c`) is the overworld for Dungeon Quest RPG. It connects all **8 towns** and **9 dungeons** through a varied terrain layout spanning 128×128 tiles.

---

## Map Layout

### Regions

```
┌──────────────────────────────────────────┐
│  NW: Windspire (highlands/mountains)     │
│      - Windspire town (23, 10)           │
│      - Sky Tower dungeon (17, 15)        │
│                                          │
│  NE: Scholar Outpost (forest/ruins)      │
│      - Scholar Outpost town (54, 20)     │
│      - Elderwood Grove nearby            │
│                                          │
│  CENTER: Crossroads + Final Sanctum      │
│      - Crossroads Market (43, 30)        │
│      - Final Sanctum island (28, 33)     │
│        [LOCKED until 4 crystals]         │
│                                          │
│  SW: Emberforge + Sandfall (desert)      │
│      - Emberforge town (4, 45)           │
│      - Volcano Keep dungeon (7, 47)      │
│      - Sandfall Oasis (15, 53)           │
│                                          │
│  SE: Greenleaf (grasslands/coast)        │
│      - Greenleaf town (60, 63) STARTER   │
│      - Coral Bay (44, 59)                │
│      - Water Temple (42, 61)             │
└──────────────────────────────────────────┘
```

### Terrain Distribution

- **Mountains (NW, W)**: High-level encounters, volcanic areas near Emberforge
- **Highlands (N)**: Medium-high level, windy plains near Windspire
- **Grasslands (E, SE)**: Low-level starter area near Greenleaf
- **Forest (NE, E)**: Medium-level, dense trees near Elderwood/Scholar Outpost
- **Desert (SW, S)**: Medium-level, sandy dunes near Sandfall Oasis
- **Coastal (S)**: Medium-level, beaches and ports near Coral Bay
- **Water (Center)**: Impassable deep water surrounding Final Sanctum island

---

## Towns (8 Total)

### Main Towns (4)

1. **Greenleaf** (95, 100)
   - **Type**: Grassland village (STARTER TOWN)
   - **Services**: Inn, Item Shop, Equipment Shop
   - **Description**: Peaceful farming village where the journey begins
   - **Nearby Dungeon**: Cave of Earth (105, 95)

2. **Coral Bay** (85, 107)
   - **Type**: Coastal port town
   - **Services**: Inn, Item Shop, Equipment Shop, Ship docks
   - **Description**: Bustling port with access to sea routes
   - **Nearby Dungeon**: Water Temple (75, 106)

3. **Emberforge** (22, 85)
   - **Type**: Mountain mining town
   - **Services**: Inn, Item Shop, Premium Equipment Shop (high-tier gear)
   - **Description**: Industrial town built into the mountains, master blacksmiths
   - **Nearby Dungeon**: Volcano Keep (18, 95)

4. **Windspire** (30, 28)
   - **Type**: Highland sky town
   - **Services**: Inn, Item Shop, Sage Tower (spell research)
   - **Description**: Town on the highest peaks, scholars and wind mages
   - **Nearby Dungeon**: Sky Tower (25, 40)

### Side Towns (4)

5. **Crossroads Market** (72, 60)
   - **Type**: Trade hub (center of map)
   - **Services**: Bazaar (unique items), Equipment exchange, Tavern (quest info)
   - **Description**: Central trading post where all roads meet
   - **Special**: Fast travel hub to all main towns

6. **Sandfall Oasis** (45, 102)
   - **Type**: Desert oasis
   - **Services**: Inn, Limited supplies, Treasure hunter guild
   - **Description**: Isolated oasis in the desert, treasure hunters gather here
   - **Nearby Dungeon**: Pyramid of Trials (55, 108)

7. **Elderwood Grove** (85, 42)
   - **Type**: Hidden forest village
   - **Services**: Inn, Herbalist (potions), Elder wisdom (lore)
   - **Description**: Ancient village hidden in the deep forest
   - **Nearby Dungeon**: Forest Shrine (78, 35)

8. **Scholar Outpost** (95, 25)
   - **Type**: Ruins research camp
   - **Services**: Item Shop, Ancient knowledge (skill scrolls)
   - **Description**: Temporary camp set up to study nearby ruins
   - **Special**: Access to bonus dungeon (105, 22) after main story

---

## Dungeons (9 Total)

### Main Story Dungeons (5)

1. **Cave of Earth** (105, 95) - Near Greenleaf
   - **Crystal**: Earth Crystal
   - **Floors**: 3 + boss arena
   - **Difficulty**: Easy (level 1-5)
   - **Boss**: Earth Guardian

2. **Water Temple** (75, 106) - Near Coral Bay
   - **Crystal**: Water Crystal
   - **Floors**: 3 + boss arena
   - **Difficulty**: Medium (level 6-10)
   - **Boss**: Water Guardian

3. **Volcano Keep** (18, 95) - Near Emberforge
   - **Crystal**: Fire Crystal
   - **Floors**: 3 + boss arena
   - **Difficulty**: Medium-Hard (level 11-15)
   - **Boss**: Fire Guardian

4. **Sky Tower** (25, 40) - Near Windspire
   - **Crystal**: Wind Crystal
   - **Floors**: 3 + boss arena
   - **Difficulty**: Hard (level 16-20)
   - **Boss**: Sky Guardian

5. **Final Sanctum** (59, 63) - Center Island
   - **Requirement**: All 4 crystals
   - **Floors**: 5 + final boss arena
   - **Difficulty**: Very Hard (level 25-30)
   - **Boss**: Final Boss (multi-phase)

### Side Dungeons (4)

6. **Smuggler's Cave** (100, 110) - Coastal
   - **Floors**: 2
   - **Difficulty**: Easy-Medium
   - **Rewards**: Gold, rare items

7. **Pyramid of Trials** (55, 108) - Near Sandfall
   - **Floors**: 3
   - **Difficulty**: Medium-Hard
   - **Rewards**: Desert-themed equipment, treasure

8. **Forest Shrine** (78, 35) - Near Elderwood
   - **Floors**: 3
   - **Difficulty**: Medium
   - **Rewards**: Nature magic, healing items

9. **Bonus Dungeon** (105, 22) - Far NE (Post-game)
   - **Requirement**: Beat Final Sanctum
   - **Floors**: 5
   - **Difficulty**: Extreme (level 35-40)
   - **Boss**: Ultimate Boss

---

## Encounter Zones

The world map uses region-based random encounters. Each tile has an assigned encounter zone that determines:
- Enemy types that spawn
- Enemy level ranges
- Encounter rate

### Zone Types

| Zone ID | Zone Name       | Enemy Level | Encounter Rate | Description |
|---------|-----------------|-------------|----------------|-------------|
| 0       | ZONE_NONE       | N/A         | 0%             | Towns, safe areas |
| 1       | ZONE_GRASSLAND  | 1-3         | Low (15%)      | Starter area, Greenleaf region |
| 2       | ZONE_FOREST     | 4-7         | Medium (25%)   | Forest areas, moderate challenge |
| 3       | ZONE_DESERT     | 5-8         | Medium (25%)   | Desert areas, heat-themed enemies |
| 4       | ZONE_MOUNTAIN   | 8-12        | High (30%)     | Mountain paths, tough enemies |
| 5       | ZONE_COAST      | 4-6         | Low (20%)      | Coastal areas, water enemies |
| 6       | ZONE_DEEP_WATER | N/A         | 0%             | Impassable ocean tiles |
| 7       | ZONE_HIGHLAND   | 9-13        | High (30%)     | Windspire area, aerial enemies |
| 8       | ZONE_SANCTUM_PATH | 15-20     | Very High (40%)| Near Final Sanctum, endgame enemies |

### Enemy Distribution by Zone

**ZONE_GRASSLAND (1)**
- Slimes, Goblins, Wild Beasts
- Gold: 5-15 per battle
- EXP: 10-30 per battle

**ZONE_FOREST (2)**
- Forest Wolves, Bandits, Spiders
- Gold: 15-40 per battle
- EXP: 30-60 per battle

**ZONE_DESERT (3)**
- Scorpions, Sand Worms, Desert Raiders
- Gold: 20-50 per battle
- EXP: 40-80 per battle

**ZONE_MOUNTAIN (4)**
- Mountain Trolls, Stone Golems, Harpies
- Gold: 40-100 per battle
- EXP: 80-150 per battle

**ZONE_COAST (5)**
- Crabs, Pirates, Sea Serpents
- Gold: 20-60 per battle
- EXP: 40-90 per battle

**ZONE_HIGHLAND (7)**
- Wind Elementals, Griffons, Sky Knights
- Gold: 50-120 per battle
- EXP: 100-180 per battle

**ZONE_SANCTUM_PATH (8)**
- Demons, Dark Knights, Void Creatures
- Gold: 100-250 per battle
- EXP: 200-400 per battle

---

## Fast Travel System

### Unlocking Fast Travel

Fast travel becomes available after visiting **Crossroads Market** for the first time.

### Fast Travel Points

Once visited, players can fast travel between these locations:

1. **Greenleaf** (starter town)
2. **Coral Bay** (port)
3. **Emberforge** (mountains)
4. **Windspire** (highlands)
5. **Crossroads Market** (hub)
6. **Sandfall Oasis** (desert)
7. **Elderwood Grove** (forest)
8. **Scholar Outpost** (ruins)

### Usage

- Open world map menu (M key)
- Select "Fast Travel"
- Choose destination from list of visited towns
- Instant teleport (uses Tent item or 50 Gold)

### Restrictions

- Cannot fast travel during combat
- Cannot fast travel from inside dungeons
- Final Sanctum has no fast travel point (must walk there)

---

## Tile ID Reference

### Terrain Tiles

| Tile ID Range | Terrain Type | Passable | Description |
|---------------|--------------|----------|-------------|
| 0x00-0x0F     | Grass        | Yes      | Grassland, plains |
| 0x10-0x1F     | Forest       | Yes      | Trees, woods |
| 0x20-0x2F     | Water        | No       | Ocean, rivers |
| 0x30-0x3F     | Mountain     | No       | Mountains, cliffs |
| 0x40-0x4F     | Desert       | Yes      | Sand, dunes |
| 0x50-0x5F     | Bridge/Road  | Yes      | Paths, bridges |
| 0x60-0x6F     | Road         | Yes      | Paved roads |

### Structure Tiles (2×2 metasprites)

| Tile IDs      | Structure Type | Passable | Description |
|---------------|----------------|----------|-------------|
| 0x70-0x73     | Town           | Yes      | Town/village building |
| 0x74-0x77     | Castle         | Yes      | Castle/fortress |
| 0x78-0x7B     | Dungeon        | Yes      | Dungeon entrance |
| 0x7C-0x7F     | Final Sanctum  | Special  | Final dungeon (requires 4 crystals) |

**Note:** All structures use 2×2 tile grids:
- Top-left (TL), Top-right (TR)
- Bottom-left (BL), Bottom-right (BR)

---

## Implementation Notes

### GameBoy Porting Considerations

1. **Map Storage**
   - 128×128 = 16,384 bytes (16 KB) for tile data
   - Stored in ROM banks (not RAM)
   - Zone data: additional 16,384 bytes (16 KB) in separate ROM bank
   - Total: 32 KB in ROM
   - Collision data: computed from tile IDs (no extra storage)

2. **Viewport**
   - Display: 18×16 tiles visible
   - Camera follows player with smooth scrolling
   - Clamps at map edges (0,0) to (63,63)

3. **Loading Strategy**
   - **Cannot** load entire 32 KB map into 8 KB RAM
   - **Stream map data** from ROM in chunks (20×18 viewport + buffer)
   - Load visible area + 1-tile border = ~22×20 = 440 bytes in RAM
   - Update chunks as player moves near edges
   - Recommended approach for GameBoy limitations

4. **Encounter Checks**
   - Check zone after each tile movement
   - Random roll: `random(0, 99) < encounter_rate`
   - Skip encounter check in ZONE_NONE and ZONE_DEEP_WATER

### Integration with Existing Game

Add to `game_state.h`:
```c
#include "world_map.h"

typedef struct {
    // Existing fields...
    uint8_t player_world_x;
    uint8_t player_world_y;
    uint8_t visited_towns[NUM_TOWNS];  // Bitmask for fast travel
    uint8_t unlocked_sanctum;           // 1 when all 4 crystals collected
} GameStateData;
```

Add state for world map exploration:
```c
#define STATE_WORLD_MAP  /* New state ID */
```

### Encounter System Integration

In `dungeon.c` or new `world_encounters.c`:
```c
void world_map_check_encounter(uint8_t x, uint8_t y) {
    uint8_t zone = world_map_get_zone(x, y);
    if (zone == ZONE_NONE || zone == ZONE_DEEP_WATER) return;

    uint8_t rate = get_encounter_rate_for_zone(zone);
    if (random_range(0, 99) < rate) {
        // Trigger battle
        battle_init_world_encounter(zone);
        game_state_change(STATE_BATTLE);
    }
}
```

---

## Future Enhancements

### Phase 2 (Post-MVP)

- [ ] Add NPCs on world map (wandering merchants, quest givers)
- [ ] Ship travel system (sail between ports)
- [ ] Hidden paths (revealed with special items)
- [ ] Weather effects (rain in forest, sandstorms in desert)
- [ ] Day/night cycle (affects encounter rates and NPCs)

### Phase 3 (Polish)

- [ ] Animated water tiles
- [ ] Player sprite with directional animations
- [ ] Map markers for quest objectives
- [ ] Treasure chests on world map (one-time pickups)
- [ ] Secret areas (requires specific party member or item)

---

## Testing Checklist

- [ ] All 8 towns are accessible from world map
- [ ] All 9 dungeon entrances are placed correctly
- [ ] Collision detection works (water/mountains block movement)
- [ ] Encounter zones trigger appropriate enemy types
- [ ] Fast travel system unlocks after Crossroads visit
- [ ] Final Sanctum is locked until 4 crystals obtained
- [ ] Map boundaries prevent walking off edges
- [ ] Camera scrolling works correctly
- [ ] Tile rendering matches tileset

---

## Credits

- **Tileset**: Final Fantasy (NES) Overworld Tileset
- **Map Design**: Based on classic JRPG world maps (Final Fantasy, Dragon Quest)
- **Implementation**: Designed for GameBoy using GBDK

---

**Last Updated:** 2025-01-23
**Status:** Complete and ready for integration
**Next Step:** Implement world map rendering and player movement system

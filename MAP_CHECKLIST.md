# Dungeon Quest RPG - Map Creation Checklist

**Total Maps: 58-60**
**Map Data Size: ~60 KB**
**Status:** 5/60 Complete (8.3%)

---

## Phase 1: Minimum Viable Product (MVP) - 26 Maps
**Goal:** Playable main story from start to finish
**Estimated Time:** Core development phase
**Priority:** HIGH

### World Map (1 map) - CRITICAL ✅ COMPLETE
- [x] `world_map` - 128×128 tiles (32 KB: 16 KB tiles + 16 KB zones)
  - ✅ Shows all 8 towns properly placed
  - ✅ All 9 dungeon entrances positioned
  - ✅ Region-based encounter zones (9 zone types)
  - ✅ Fast travel system integration
  - ✅ Ocean borders, varied terrain (mountains, forest, desert, grassland, coast)
  - **Files:** `SRC/world_map.h`, `SRC/world_map.c`, `WORLD_MAP_GUIDE.md`
  - **Tools:** `generate_world_map.py`, `visualize_map.py`
  - **Tileset:** Final Fantasy NES Overworld tileset
  - **Preview:** `world_map_preview.html`

### Main Towns - Exteriors Only (4 maps) - CRITICAL
- [x] **Greenleaf** - 32×32 tiles (Starter town) ✅ COMPLETE
  - Inn (NW), Item Shop (E), Equipment Shop (SE)
  - 3 Houses, Town Well, Trees, Fences, Path network
  - **Files:** `SRC/greenleaf_map.c`, `SRC/town_map.h`
  - **Tools:** `generate_greenleaf_corneria.py`, `visualize_town.py`
  - **Tileset:** Corneria Sprites (Final Fantasy NES) - 16x16 metasprites
  - **Preview:** `greenleaf_preview.html`
- [x] **Coral Bay** - 32×32 tiles (Coastal fishing village) ✅ COMPLETE
  - Inn, Item Shop, Equipment Shop, 3 Houses
  - Harbor with docks, Water, Well, Trees, Fences
  - **Files:** `SRC/coral_bay_map.c`
  - **Tools:** `generate_coral_bay.py`
  - **Tileset:** Corneria Sprites (Final Fantasy NES)
  - **Preview:** `coral_bay_preview.html`
- [x] **Emberforge** - 32×32 tiles (Mountain mining town) ✅ COMPLETE
  - Inn, Item Shop, Equipment Shop, 3 Houses
  - Mine entrance (north), Well, Trees, Fences, Cross paths
  - **Files:** `SRC/emberforge_map.c`
  - **Tools:** `generate_emberforge.py`
  - **Tileset:** Corneria Sprites (Final Fantasy NES)
  - **Preview:** `emberforge_preview.html`
- [x] **Windspire** - 32×32 tiles (Highland sky town) ✅ COMPLETE
  - Inn, Item Shop, Equipment Shop, 3 Houses
  - Highland platform, Mountain spring, Well, Trees, Fences
  - **Files:** `SRC/windspire_map.c`
  - **Tools:** `generate_windspire.py`
  - **Tileset:** Corneria Sprites (Final Fantasy NES)
  - **Preview:** `windspire_preview.html`

### Main Dungeon: Cave of Earth (4 maps) - CRITICAL
- [ ] `cave_floor0.tmx` - 16×16 tiles (Entry/tutorial)
- [ ] `cave_floor1.tmx` - 16×16 tiles (Basic layout)
- [ ] `cave_floor2.tmx` - 24×24 tiles (Introduces scrolling)
- [ ] `cave_boss.tmx` - 16×16 tiles (Earth Guardian boss arena)

### Main Dungeon: Water Temple (4 maps) - CRITICAL
- [ ] `water_floor0.tmx` - 24×24 tiles (Water puzzle)
- [ ] `water_floor1.tmx` - 24×24 tiles (Exploration)
- [ ] `water_floor2.tmx` - 32×32 tiles (Ice sliding maze)
- [ ] `water_boss.tmx` - 24×24 tiles (Water Guardian boss arena)

### Main Dungeon: Volcano Keep (4 maps) - CRITICAL
- [ ] `volcano_floor0.tmx` - 24×24 tiles (Entry)
- [ ] `volcano_floor1.tmx` - 32×32 tiles (Lava maze)
- [ ] `volcano_floor2.tmx` - 32×32 tiles (Multi-path)
- [ ] `volcano_boss.tmx` - 24×24 tiles (Fire Guardian boss arena)

### Main Dungeon: Sky Tower (4 maps) - CRITICAL
- [ ] `sky_floor0.tmx` - 32×32 tiles (Cloud maze)
- [ ] `sky_floor1.tmx` - 32×32 tiles (Wind puzzles)
- [ ] `sky_floor2.tmx` - 32×32 tiles (Platforming)
- [ ] `sky_boss.tmx` - 32×32 tiles (Sky Guardian boss arena)

### Main Dungeon: Final Sanctum (5 maps) - CRITICAL
- [ ] `sanctum_floor0.tmx` - 32×32 tiles (Entry)
- [ ] `sanctum_floor1.tmx` - 32×32 tiles (Dark corridors)
- [ ] `sanctum_floor2.tmx` - 32×32 tiles (Puzzle floor)
- [ ] `sanctum_floor3.tmx` - 32×32 tiles (Gauntlet floor)
- [ ] `sanctum_floor4.tmx` - 32×32 tiles (Final boss arena)

**Phase 1 Total:** 26 maps
**Phase 1 Progress:** 5/26 (19.2%)

---

## Phase 2: Full Experience - 12 Additional Maps (38 Total)
**Goal:** Add side content for exploration and optional challenges
**Estimated Time:** Post-MVP expansion
**Priority:** MEDIUM

### Side Towns (4 maps) - MEDIUM PRIORITY
- [ ] `crossroads_market.tmx` - 24×24 tiles (Trade hub)
- [ ] `sandfall_oasis.tmx` - 32×32 tiles (Desert town)
- [ ] `elderwood_grove.tmx` - 24×24 tiles (Hidden forest village)
- [ ] `scholar_outpost.tmx` - 20×20 tiles (Ruins camp)

### Side Dungeon: Smuggler's Cave (2 maps) - MEDIUM PRIORITY
- [ ] `smuggler_floor0.tmx` - 24×24 tiles (Water caves)
- [ ] `smuggler_floor1.tmx` - 24×24 tiles (Boss room)

### Side Dungeon: Pyramid of Trials (3 maps) - MEDIUM PRIORITY
- [ ] `pyramid_floor0.tmx` - 32×32 tiles (Entrance)
- [ ] `pyramid_floor1.tmx` - 32×32 tiles (Puzzle chambers)
- [ ] `pyramid_floor2.tmx` - 32×32 tiles (Treasure vault + boss)

### Side Dungeon: Hidden Forest Shrine (3 maps) - MEDIUM PRIORITY
- [ ] `forest_shrine_floor0.tmx` - 24×24 tiles (Sacred grove)
- [ ] `forest_shrine_floor1.tmx` - 24×24 tiles (Ancient altar)
- [ ] `forest_shrine_floor2.tmx` - 24×24 tiles (Spirit boss)

**Phase 2 Total:** 12 maps (38 cumulative)
**Phase 2 Progress:** 0/12 (0%)

---

## Phase 3: Complete Edition - 20-22 Additional Maps (58-60 Total)
**Goal:** Full game with all optional content
**Estimated Time:** Polish and completion phase
**Priority:** LOW (Nice-to-have)

### Town Interiors (4-6 maps) - LOW PRIORITY
- [ ] `greenleaf_inn.tmx` - 20×18 tiles (Inn interior)
- [ ] `greenleaf_shop.tmx` - 20×18 tiles (Combined shop interior)
- [ ] `emberforge_blacksmith.tmx` - 20×18 tiles (Blacksmith shop)
- [ ] `windspire_tower.tmx` - 20×18 tiles (Sage's Tower)
- [ ] `crossroads_bazaar.tmx` - 24×24 tiles (Rotating bazaar)
- [ ] `elderwood_elder.tmx` - 20×18 tiles (Elder's hut)

### Side Dungeon: Ancient Forge (3 maps) - LOW PRIORITY
- [ ] `forge_floor0.tmx` - 24×24 tiles (Abandoned mines)
- [ ] `forge_floor1.tmx` - 32×32 tiles (Molten chambers)
- [ ] `forge_floor2.tmx` - 32×32 tiles (Master Forgekeeper boss)

### Side Dungeon: Cloud Temple (3 maps) - LOW PRIORITY
- [ ] `cloud_temple_floor0.tmx` - 24×24 tiles (Sky sanctuary)
- [ ] `cloud_temple_floor1.tmx` - 24×24 tiles (Wind trials)
- [ ] `cloud_temple_floor2.tmx` - 24×24 tiles (Tempest Guardian boss)

### Side Dungeon: Crystal Cavern (4 maps) - LOW PRIORITY
- [ ] `crystal_cavern_floor0.tmx` - 32×32 tiles (Glittering caves)
- [ ] `crystal_cavern_floor1.tmx` - 32×32 tiles (Maze of reflections)
- [ ] `crystal_cavern_floor2.tmx` - 32×32 tiles (Crystal gardens)
- [ ] `crystal_cavern_floor3.tmx` - 32×32 tiles (Crystal Golem boss)

### Side Dungeon: Underwater Grotto (3 maps) - LOW PRIORITY
- [ ] `grotto_floor0.tmx` - 24×24 tiles (Submerged entrance, requires special item)
- [ ] `grotto_floor1.tmx` - 32×32 tiles (Coral labyrinth)
- [ ] `grotto_floor2.tmx` - 32×32 tiles (Leviathan boss)

### Side Dungeon: Final Bonus Dungeon (5 maps) - LOW PRIORITY
- [ ] `bonus_floor0.tmx` - 32×32 tiles (Post-game super-hard)
- [ ] `bonus_floor1.tmx` - 32×32 tiles (Elite enemies)
- [ ] `bonus_floor2.tmx` - 32×32 tiles (Multi-boss gauntlet)
- [ ] `bonus_floor3.tmx` - 32×32 tiles (Final challenges)
- [ ] `bonus_floor4.tmx` - 32×32 tiles (Ultimate boss)

**Phase 3 Total:** 20-22 maps (58-60 cumulative)
**Phase 3 Progress:** 0/22 (0%)

---

## Tileset Requirements

### Core Tilesets (High Priority)
- [ ] `dungeon_cave.tsx` - Cave/earth tiles (browns, grays)
- [ ] `dungeon_water.tsx` - Water/ice tiles (blues, whites)
- [ ] `dungeon_volcano.tsx` - Lava/fire tiles (reds, oranges)
- [ ] `dungeon_sky.tsx` - Cloud/wind tiles (whites, light blues)
- [ ] `dungeon_sanctum.tsx` - Final dungeon tiles (purples, blacks)
- [ ] `town.tsx` - Town buildings, paths, NPCs
- [ ] `world.tsx` - Overworld terrain (grass, mountains, water, desert, forest)

### Expansion Tilesets (Medium/Low Priority)
- [ ] `dungeon_desert.tsx` - Pyramid tiles (yellows, sandstone)
- [ ] `dungeon_forest.tsx` - Forest shrine tiles (greens, wood)
- [ ] `dungeon_crystal.tsx` - Crystal cavern tiles (gem colors)
- [ ] `dungeon_underwater.tsx` - Underwater tiles (deep blues, coral)
- [ ] `dungeon_bonus.tsx` - Special endgame tiles (unique palette)

---

## Map Design Checklist (Per Map)

When creating each map, ensure:
- [ ] Collision layer configured (walls, obstacles)
- [ ] Object layer for entities (treasure chests, stairs, NPCs, spawn points)
- [ ] Encounter zones marked (if applicable)
- [ ] Puzzle mechanics tested (ice sliding, teleporters, switches)
- [ ] Exported to C array format
- [ ] Integrated into ROM bank structure
- [ ] Tested in-game (movement, camera, loading)

---

## Map Size Reference

| Size | Dimensions | Tiles | Bytes | Use Case |
|------|------------|-------|-------|----------|
| Small | 16×16 | 256 | 256 B | Boss arenas, tutorial rooms |
| Medium | 24×24 | 576 | 576 B | Mid-size dungeons, towns |
| Large | 32×32 | 1024 | 1 KB | Mazes, complex puzzles, late-game |
| World | 64×64 | 4096 | 4 KB | Overworld map |

**Viewport:** 18×16 tiles (visible area)
**Camera:** Smooth scrolling, clamps at edges

---

## Completion Milestones

- [ ] **Milestone 1:** World map + 4 main towns (5 maps)
- [ ] **Milestone 2:** Cave of Earth complete (4 maps, playable first dungeon)
- [ ] **Milestone 3:** All main dungeons complete (21 maps, main story playable)
- [ ] **Milestone 4:** Phase 1 complete (26 maps, MVP done)
- [ ] **Milestone 5:** All side towns added (4 maps)
- [ ] **Milestone 6:** 3 side dungeons complete (8 maps)
- [ ] **Milestone 7:** Phase 2 complete (38 maps, full experience)
- [ ] **Milestone 8:** All content complete (58-60 maps, 100% game)

---

## Notes

**Priorities:**
- Focus on Phase 1 (MVP) first - gets main story playable
- Phase 2 adds replay value and optional content
- Phase 3 is polish and completionist content

**Estimated Development Time:**
- Simple map (16×16): 1-2 hours
- Medium map (24×24) with puzzles: 3-4 hours
- Complex map (32×32) with multi-path: 5-8 hours
- Total Phase 1 estimate: ~80-120 hours of map design

**Tools:**
- Tiled Map Editor (free, cross-platform)
- Export to C arrays for GBDK integration
- Version control each map file

**Testing:**
- Test each map individually before moving to next
- Verify collision, camera bounds, entity spawns
- Playtest dungeons as complete sets (all floors together)

---

**Last Updated:** 2025-01-23
**Current Phase:** Planning
**Next Action:** Create core tilesets, then start with world_map.tmx

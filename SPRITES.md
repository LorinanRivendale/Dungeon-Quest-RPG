# Dungeon Quest RPG - Sprite & Graphics Specifications

**Target Platform:** GameBoy / GameBoy Color
**Sprite Style:** 16×16 metasprites (Final Fantasy Legend / Dragon Quest III inspired)
**Battle Style:** DQ3 (enemy sprites only, text-based party UI)

---

## Table of Contents

1. [GameBoy Hardware Limits](#gameboy-hardware-limits)
2. [Sprite Size Standards](#sprite-size-standards)
3. [Battle Screen Layout](#battle-screen-layout)
4. [Exploration Sprites](#exploration-sprites)
5. [Sprite Budget Analysis](#sprite-budget-analysis)
6. [Metasprite Implementation](#metasprite-implementation)
7. [Graphics Asset List](#graphics-asset-list)

---

## GameBoy Hardware Limits

### Hardware Constraints
- **Total Sprites:** 40 hardware sprites (OAM entries) maximum
- **Per Scanline:** 10 sprites per horizontal line maximum
- **Sprite Modes:** 8×8 or 8×16 pixels (global setting, one mode at a time)
- **Sprite Size:** Each hardware sprite is 8×8 pixels (in 8×8 mode)
- **Colors:** 4 shades of gray (GB) or 4 colors per palette (GBC)
- **Palettes:** 2 sprite palettes (OBP0, OBP1) on GB, 8 palettes on GBC

### Screen Resolution
- **Display:** 160×144 pixels
- **Tile Grid:** 20×18 tiles (8×8 each)
- **Viewport (Dungeons):** 18×16 tiles visible area
- **Background Layers:** 1 background layer (tiles)

---

## Sprite Size Standards

### Exploration (Dungeons, World Map, Towns)
**Player Character:** 16×16 pixels
- Uses 4 hardware sprites (2×2 grid)
- Clear, detailed character representation
- Allows for 4-directional animations

**NPCs:** 16×16 pixels
- Same size as player for consistency
- Merchants, quest givers, townsfolk
- Uses 4 hardware sprites each

**Small Objects:** 8×8 pixels (optional)
- Treasure indicators, sparkles, effects
- Uses 1 hardware sprite each

### Battle Screen
**Regular Enemies:** 16×16 pixels
- Uses 4 hardware sprites (2×2 grid)
- Standard enemy size for normal encounters
- Allows up to 6 enemies on screen comfortably

**Boss Enemies:** 24×24 pixels
- Uses 9 hardware sprites (3×3 grid)
- Larger, more imposing boss sprites
- Main dungeon bosses and mini-bosses

**Epic Final Bosses:** 32×32 pixels (optional)
- Uses 16 hardware sprites (4×4 grid)
- Reserved for final boss multi-phase fights
- Maximum sprite budget: 20-22 sprites total with UI

**Player Party:** Text-based UI (NO sprites in battle)
- Saves 16 sprites (4 characters × 4 sprites each)
- DQ3-style text display with HP/MP bars
- More screen space for enemies

---

## Battle Screen Layout

### DQ3-Style Battle UI (Recommended)

```
┌────────────────────────────────────────┐
│                                        │  ← Top: Enemy sprite area
│    [Slime]   [Goblin]   [Skeleton]    │    (16×16 each = 4 sprites × 3)
│                                        │
│              [Dragon]                  │  ← Boss sprite (24×24 = 9 sprites)
│                                        │
├────────────────────────────────────────┤
│  Fighter    HP: 085/120  MP: 015/020  │  ← Party stats (text-based)
│  Mage       HP: 042/060  MP: 050/080  │
│  Priest     HP: 055/080  MP: 040/070  │
│  Thief      HP: 070/095  MP: 020/030  │
├────────────────────────────────────────┤
│ > Attack    Skill    Item    Run      │  ← Action menu (cursor)
└────────────────────────────────────────┘
```

**Benefits:**
- ✅ No party sprites = saves 16 hardware sprites
- ✅ More screen space for detailed enemy sprites
- ✅ Shows both HP and MP clearly
- ✅ Classic GameBoy RPG aesthetic
- ✅ Well within sprite limits

### Alternative: FFL-Style (Enemy Groups)

```
┌────────────────────────────────────────┐
│                                        │
│    [Slime×4]          [Goblin×2]      │  ← Group sprites (2 sprites total)
│                                        │    Each sprite represents multiple enemies
│                                        │
│  [Party UI same as above]              │
└────────────────────────────────────────┘
```

**Benefits:**
- ✅ Even fewer sprites (2-3 for entire enemy force)
- ✅ Allows 8-10+ total enemies
- ✅ Classic Final Fantasy Legend style
- ✅ Simple and clean

**Drawback:**
- ❌ Less visual variety (one sprite per enemy type)

**Recommendation:** Use DQ3 style (individual enemy sprites) for better visual appeal.

---

## Exploration Sprites

### Dungeon Exploration

**Player:**
- Size: 16×16 (4 sprites)
- Animations: 4-directional walk cycles (up, down, left, right)
- Frames: 2-3 frames per direction (8-12 frames total)

**NPCs/Enemies (on map):**
- Size: 16×16 (4 sprites each)
- Count: 2-3 visible NPCs maximum
- Purpose: Quest givers, merchants, wandering enemies

**Objects:**
- Treasure chests: 8×8 or 16×16 (1-4 sprites)
- Switches/levers: 8×8 (1 sprite)
- Effects: 8×8 (1 sprite) - sparkles, indicators

**Sprite Budget:**
- Player: 4 sprites
- NPCs (3): 12 sprites
- Objects/effects: 4-8 sprites
- **Total: 20-24 sprites** ✅ Well under 40 limit

### World Map Exploration

**Player:**
- Size: 16×16 (4 sprites)
- Animations: 4-directional movement

**Landmarks (optional):**
- Town indicators: 8×8 sprites (1 each)
- Dungeon markers: 8×8 sprites (1 each)
- Could also use background tiles instead

**Sprite Budget:**
- Player: 4 sprites
- Markers (if used): 8-10 sprites
- **Total: 12-14 sprites** ✅ Plenty of headroom

---

## Sprite Budget Analysis

### Battle Scenarios

#### Regular Battle: 4 Enemies @ 16×16
- 4 enemies × 4 sprites = **16 sprites**
- Cursor/UI elements: **4-6 sprites**
- **Total: 20-22 sprites** ✅ **50% of limit**

#### Boss Battle: 1 Boss @ 24×24
- 1 boss × 9 sprites = **9 sprites**
- Cursor/UI elements: **4-6 sprites**
- **Total: 13-15 sprites** ✅ **32% of limit**

#### Mixed Battle: 2 Enemies + 1 Mini-Boss @ 24×24
- 2 enemies × 4 sprites = **8 sprites**
- 1 mini-boss × 9 sprites = **9 sprites**
- Cursor/UI: **4-6 sprites**
- **Total: 21-23 sprites** ✅ **52% of limit**

#### Large Enemy Group: 6 Enemies @ 16×16
- 6 enemies × 4 sprites = **24 sprites**
- Cursor/UI: **4-6 sprites**
- **Total: 28-30 sprites** ✅ **70% of limit**

#### Epic Final Boss: 1 Boss @ 32×32
- 1 boss × 16 sprites = **16 sprites**
- Cursor/UI: **4-6 sprites**
- **Total: 20-22 sprites** ✅ **50% of limit**

### Scanline Limit Check

**4 Enemies Horizontally Aligned:**
- Each 16×16 enemy = 2 sprites wide
- 4 enemies = **8 sprites per scanline** ✅ **Under 10 limit**

**24×24 Boss:**
- 3 sprites wide = **3 sprites per scanline** ✅ **No problem**

**Vertical Stacking (2 rows of 2 enemies):**
- Only **4 sprites per scanline** ✅ **Even safer**

**Conclusion:** All scenarios are well within hardware limits!

---

## Metasprite Implementation

### 16×16 Metasprite (4 Hardware Sprites)

```c
// Metasprite structure
typedef struct {
    uint8_t tile_ids[4];  // Tile indices: [TL, TR, BL, BR]
    uint8_t x, y;         // Screen position
    uint8_t palette;      // Sprite palette (0 or 1)
} Metasprite16x16;

// Display function
void draw_metasprite_16x16(uint8_t sprite_id, uint8_t x, uint8_t y,
                           const uint8_t* tiles, uint8_t palette) {
    // Top-left
    set_sprite_tile(sprite_id + 0, tiles[0]);
    set_sprite_prop(sprite_id + 0, palette);
    move_sprite(sprite_id + 0, x, y);

    // Top-right
    set_sprite_tile(sprite_id + 1, tiles[1]);
    set_sprite_prop(sprite_id + 1, palette);
    move_sprite(sprite_id + 1, x + 8, y);

    // Bottom-left
    set_sprite_tile(sprite_id + 2, tiles[2]);
    set_sprite_prop(sprite_id + 2, palette);
    move_sprite(sprite_id + 2, x, y + 8);

    // Bottom-right
    set_sprite_tile(sprite_id + 3, tiles[3]);
    set_sprite_prop(sprite_id + 3, palette);
    move_sprite(sprite_id + 3, x + 8, y + 8);
}
```

**Sprite Layout:**
```
[0][1]  ← Top row (sprites 0-1)
[2][3]  ← Bottom row (sprites 2-3)
```

### 24×24 Metasprite (9 Hardware Sprites)

```c
// Metasprite structure
typedef struct {
    uint8_t tile_ids[9];  // Tile indices (3×3 grid)
    uint8_t x, y;
    uint8_t palette;
} Metasprite24x24;

// Display function
void draw_metasprite_24x24(uint8_t sprite_id, uint8_t x, uint8_t y,
                           const uint8_t* tiles, uint8_t palette) {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int idx = row * 3 + col;
            set_sprite_tile(sprite_id + idx, tiles[idx]);
            set_sprite_prop(sprite_id + idx, palette);
            move_sprite(sprite_id + idx, x + (col * 8), y + (row * 8));
        }
    }
}
```

**Sprite Layout:**
```
[0][1][2]  ← Top row (sprites 0-2)
[3][4][5]  ← Middle row (sprites 3-5)
[6][7][8]  ← Bottom row (sprites 6-8)
```

### 32×32 Metasprite (16 Hardware Sprites)

```c
// For epic final bosses only
typedef struct {
    uint8_t tile_ids[16]; // 4×4 grid
    uint8_t x, y;
    uint8_t palette;
} Metasprite32x32;
```

**Sprite Layout:**
```
[00][01][02][03]  ← Row 0 (sprites 0-3)
[04][05][06][07]  ← Row 1 (sprites 4-7)
[08][09][10][11]  ← Row 2 (sprites 8-11)
[12][13][14][15]  ← Row 3 (sprites 12-15)
```

---

## Graphics Asset List

### Character Sprites (Exploration)

**Player Character (6 Job Classes):**
- Knight (16×16, 4 directions × 2-3 frames = 8-12 frames)
- Black Belt (16×16, 8-12 frames)
- Thief (16×16, 8-12 frames)
- Mage (16×16, 8-12 frames)
- Sage (16×16, 8-12 frames)
- Priest (16×16, 8-12 frames)

**Total:** ~60-72 sprite frames for all player classes

**NPCs:**
- Townspeople (16×16, 2-4 types × 2 frames = 8 frames)
- Merchants (16×16, 2 frames)
- Quest givers (16×16, 2 frames)
- Elders/important NPCs (16×16, 2 frames)

**Total:** ~16-20 NPC sprite frames

### Enemy Sprites (Battle)

**Regular Enemies @ 16×16 (4 sprites each):**
- Slime family (3 variants)
- Goblin family (3 variants)
- Skeleton family (3 variants)
- Beast family (3 variants)
- Elemental family (4 variants: fire, water, earth, wind)
- Undead family (3 variants)
- Dragon family (3 variants)

**Estimated:** ~25-30 regular enemy sprites

**Boss Enemies @ 24×24 (9 sprites each):**
- Earth Guardian (Cave of Earth)
- Water Guardian (Water Temple)
- Fire Guardian (Volcano Keep)
- Sky Guardian (Sky Tower)
- Mini-bosses (5-7 unique bosses)
- Side dungeon bosses (8 unique bosses)

**Estimated:** ~18-20 boss sprites

**Epic Bosses @ 32×32 (16 sprites each):**
- Final Boss (multi-phase: 2-3 forms)
- Ultimate Bonus Boss (1 sprite)

**Estimated:** 3-4 epic boss sprites

**Total Enemy Sprites:** ~46-54 unique sprites

### UI Elements

**Battle UI:**
- Cursor (8×8, 2 frames for animation)
- Action icons (8×8, 4 icons: Attack, Skill, Item, Run)
- Status icons (8×8, for buffs/debuffs display)

**Exploration UI:**
- Menu cursor (8×8)
- Treasure sparkle effect (8×8, 2-3 frames)
- Interaction indicator (8×8)

**Total UI Sprites:** ~12-15 sprite frames

---

## Asset Production Guidelines

### Sprite Design Rules

1. **Color Palette:**
   - GameBoy: 4 shades of gray (white, light gray, dark gray, black)
   - GameBoy Color: 4 colors per palette, 8 palettes available
   - Keep designs simple and readable at small size

2. **Readability:**
   - Clear silhouettes (distinct enemy shapes)
   - High contrast between foreground and background
   - Avoid excessive detail (won't be visible at 16×16)

3. **Animation:**
   - Exploration: 2-3 frame walk cycles
   - Battle: Static sprites (save ROM space)
   - Effects: 2-3 frame simple animations

4. **Consistency:**
   - All player sprites same size (16×16)
   - Enemy size indicates threat level (16×16 regular, 24×24 boss, 32×32 epic)
   - NPCs match player size (16×16)

### File Organization

```
GB/
├── sprites/
│   ├── player/
│   │   ├── knight_16x16.png
│   │   ├── blackbelt_16x16.png
│   │   ├── thief_16x16.png
│   │   ├── mage_16x16.png
│   │   ├── sage_16x16.png
│   │   └── priest_16x16.png
│   ├── enemies/
│   │   ├── slime_16x16.png
│   │   ├── goblin_16x16.png
│   │   ├── skeleton_16x16.png
│   │   └── ... (regular enemies)
│   ├── bosses/
│   │   ├── earth_guardian_24x24.png
│   │   ├── water_guardian_24x24.png
│   │   └── ... (boss enemies)
│   ├── npcs/
│   │   ├── merchant_16x16.png
│   │   ├── elder_16x16.png
│   │   └── ... (town NPCs)
│   └── ui/
│       ├── cursor_8x8.png
│       ├── icons_8x8.png
│       └── effects_8x8.png
└── generated/
    └── ... (exported C arrays)
```

---

## Development Priorities

### Phase 1: Core Sprites (MVP)
- [ ] Player character (1 class for testing) - 16×16
- [ ] 3-4 basic enemy types - 16×16
- [ ] 1 boss sprite - 24×24
- [ ] Basic UI elements (cursor, icons)

### Phase 2: Full Character Roster
- [ ] All 6 player classes - 16×16 each
- [ ] NPC sprites (townspeople, merchants)
- [ ] Basic animations (walk cycles)

### Phase 3: Complete Enemy Roster
- [ ] All regular enemies (25-30 sprites) - 16×16
- [ ] All boss sprites (18-20 sprites) - 24×24
- [ ] Epic final boss sprites (3-4 sprites) - 32×32

### Phase 4: Polish
- [ ] Additional animations
- [ ] Status effect indicators
- [ ] GameBoy Color palette variants
- [ ] Battle effects and UI polish

---

## Technical Notes

### ROM Space Considerations

**Sprite Data Size:**
- Each 8×8 tile = 16 bytes (8×8 pixels, 2 bits per pixel)
- 16×16 sprite (4 tiles) = 64 bytes
- 24×24 sprite (9 tiles) = 144 bytes
- 32×32 sprite (16 tiles) = 256 bytes

**Total Sprite Data (Estimated):**
- Player characters (6 × 12 frames × 64 bytes) = ~4.6 KB
- NPCs (20 frames × 64 bytes) = ~1.3 KB
- Regular enemies (30 × 64 bytes) = ~1.9 KB
- Bosses (20 × 144 bytes) = ~2.9 KB
- Epic bosses (4 × 256 bytes) = ~1 KB
- UI elements (15 × 16 bytes) = ~0.2 KB

**Total: ~12-15 KB of sprite data** (easily fits in ROM banks)

### VRAM Management

- GameBoy VRAM: 8 KB total
- Background tiles: ~4-6 KB (tilesets)
- Sprite tiles: ~2-4 KB (active sprites loaded as needed)
- Bank switching can load different sprite sets per area

---

## Summary

**Sprite Size Standards:**
- ✅ Exploration: 16×16 for all characters
- ✅ Battle enemies: 16×16 regular, 24×24 bosses, 32×32 epic
- ✅ Battle party: Text UI only (no sprites)
- ✅ UI elements: 8×8

**Hardware Limits:**
- ✅ All scenarios use 20-30 sprites (well under 40 limit)
- ✅ Scanline limit never exceeded (max 8 sprites per line)
- ✅ ROM space for sprites: ~12-15 KB (manageable)

**Battle Style:**
- ✅ DQ3-style (enemy sprites + text party UI)
- ✅ Shows both HP and MP for all party members
- ✅ Clear, readable enemy sprites
- ✅ Classic GameBoy RPG aesthetic

**Asset Count:**
- ~60-72 player sprite frames (6 classes)
- ~16-20 NPC sprite frames
- ~46-54 enemy sprites (regular + boss + epic)
- ~12-15 UI sprite frames
- **Total: ~140-160 sprite frames**

---

**Last Updated:** 2025-01-23
**Status:** Planning phase
**Next Action:** Create core player sprite (Knight 16×16) for testing

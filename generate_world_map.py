#!/usr/bin/env python3
"""
Generate a 128x128 world map for Dungeon Quest RPG
With all 8 towns and 9 dungeons properly placed
"""

import random

# Map dimensions
WIDTH = 128
HEIGHT = 128

# Tile definitions
TILE_GRASS = 0x00
TILE_FOREST = 0x10
TILE_FOREST_DARK = 0x13
TILE_WATER = 0x20
TILE_MOUNTAIN = 0x30
TILE_MOUNTAIN_PEAK = 0x32
TILE_DESERT = 0x40

# Structure tiles (2x2)
TILE_TOWN_TL = 0x70
TILE_TOWN_TR = 0x71
TILE_TOWN_BL = 0x72
TILE_TOWN_BR = 0x73

TILE_DUNGEON_TL = 0x78
TILE_DUNGEON_TR = 0x79
TILE_DUNGEON_BL = 0x7A
TILE_DUNGEON_BR = 0x7B

TILE_SANCTUM_TL = 0x7C
TILE_SANCTUM_TR = 0x7D
TILE_SANCTUM_BL = 0x7E
TILE_SANCTUM_BR = 0x7F

# Encounter zones
ZONE_NONE = 0
ZONE_GRASSLAND = 1
ZONE_FOREST = 2
ZONE_DESERT = 3
ZONE_MOUNTAIN = 4
ZONE_COAST = 5
ZONE_DEEP_WATER = 6
ZONE_HIGHLAND = 7
ZONE_SANCTUM_PATH = 8

# Initialize map
world_map = [[TILE_GRASS for _ in range(WIDTH)] for _ in range(HEIGHT)]
zone_map = [[ZONE_GRASSLAND for _ in range(WIDTH)] for _ in range(HEIGHT)]

def fill_rect(tile, x, y, w, h):
    """Fill a rectangle with a specific tile"""
    for row in range(y, min(y + h, HEIGHT)):
        for col in range(x, min(x + w, WIDTH)):
            world_map[row][col] = tile

def fill_circle(tile, cx, cy, radius):
    """Fill a circular area with a specific tile"""
    for row in range(max(0, cy - radius), min(HEIGHT, cy + radius + 1)):
        for col in range(max(0, cx - radius), min(WIDTH, cx + radius + 1)):
            dist = ((row - cy) ** 2 + (col - cx) ** 2) ** 0.5
            if dist <= radius:
                world_map[row][col] = tile

def fill_zone_rect(zone, x, y, w, h):
    """Fill a zone rectangle"""
    for row in range(y, min(y + h, HEIGHT)):
        for col in range(x, min(x + w, WIDTH)):
            zone_map[row][col] = zone

def place_town(x, y):
    """Place a 2x2 town at coordinates"""
    world_map[y][x] = TILE_TOWN_TL
    world_map[y][x+1] = TILE_TOWN_TR
    world_map[y+1][x] = TILE_TOWN_BL
    world_map[y+1][x+1] = TILE_TOWN_BR
    # Towns are safe zones
    for dy in range(-2, 4):
        for dx in range(-2, 4):
            if 0 <= y+dy < HEIGHT and 0 <= x+dx < WIDTH:
                zone_map[y+dy][x+dx] = ZONE_NONE

def place_dungeon(x, y):
    """Place a 2x2 dungeon entrance at coordinates"""
    world_map[y][x] = TILE_DUNGEON_TL
    world_map[y][x+1] = TILE_DUNGEON_TR
    world_map[y+1][x] = TILE_DUNGEON_BL
    world_map[y+1][x+1] = TILE_DUNGEON_BR

def place_sanctum(x, y):
    """Place a 2x2 Final Sanctum at coordinates"""
    world_map[y][x] = TILE_SANCTUM_TL
    world_map[y][x+1] = TILE_SANCTUM_TR
    world_map[y+1][x] = TILE_SANCTUM_BL
    world_map[y+1][x+1] = TILE_SANCTUM_BR

print("🗺️  Generating 128x128 World Map...")
print("=" * 60)

# === OCEAN BORDERS (N, S, E, W edges) ===
print("Creating ocean borders...")
fill_rect(TILE_WATER, 0, 0, WIDTH, 15)  # North ocean
fill_rect(TILE_WATER, 0, HEIGHT-15, WIDTH, 15)  # South ocean
fill_rect(TILE_WATER, 0, 0, 12, HEIGHT)  # West ocean
fill_rect(TILE_WATER, WIDTH-12, 0, 12, HEIGHT)  # East ocean

# Mark ocean as impassable
fill_zone_rect(ZONE_DEEP_WATER, 0, 0, WIDTH, 15)
fill_zone_rect(ZONE_DEEP_WATER, 0, HEIGHT-15, WIDTH, 15)
fill_zone_rect(ZONE_DEEP_WATER, 0, 0, 12, HEIGHT)
fill_zone_rect(ZONE_DEEP_WATER, WIDTH-12, 0, 12, HEIGHT)

# === NW REGION: Windspire (mountains/highlands) ===
print("Creating NW region (Windspire area)...")
fill_rect(TILE_MOUNTAIN, 15, 18, 35, 30)
fill_rect(TILE_FOREST, 12, 15, 40, 5)
fill_zone_rect(ZONE_MOUNTAIN, 15, 18, 35, 30)
fill_zone_rect(ZONE_HIGHLAND, 20, 25, 25, 20)

# Windspire town (NW highlands)
place_town(30, 28)
# Sky Tower dungeon (near Windspire)
place_dungeon(25, 40)

# === NE REGION: Scholar Outpost + Elderwood Grove (forest/ruins) ===
print("Creating NE region (Forest area)...")
fill_rect(TILE_FOREST_DARK, 75, 20, 35, 35)
fill_rect(TILE_FOREST, 70, 18, 45, 40)
fill_zone_rect(ZONE_FOREST, 70, 18, 45, 40)

# Scholar Outpost (NE ruins)
place_town(95, 25)
# Elderwood Grove (E forest)
place_town(85, 42)
# Forest Shrine dungeon (deep forest)
place_dungeon(78, 35)
# Bonus Dungeon (far NE, post-game)
place_dungeon(105, 22)

# === CENTER: Crossroads Market + Final Sanctum Island ===
print("Creating center region...")
# Grassland hub area
fill_rect(TILE_GRASS, 50, 50, 30, 30)
fill_zone_rect(ZONE_GRASSLAND, 45, 45, 40, 40)

# Crossroads Market (center-east)
place_town(72, 60)

# Final Sanctum island (center, surrounded by water)
fill_circle(TILE_WATER, 60, 64, 12)
fill_zone_rect(ZONE_SANCTUM_PATH, 48, 52, 24, 24)
fill_circle(TILE_GRASS, 60, 64, 6)
place_sanctum(59, 63)

# === SW REGION: Emberforge (mountains) + Sandfall (desert) ===
print("Creating SW region (Mountains and Desert)...")
# Mountains (Emberforge area)
fill_rect(TILE_MOUNTAIN, 15, 75, 30, 35)
fill_zone_rect(ZONE_MOUNTAIN, 15, 75, 30, 35)

# Emberforge town (W mountains)
place_town(22, 85)
# Volcano Keep dungeon (volcanic mountains)
place_dungeon(18, 95)

# Desert (Sandfall area)
fill_rect(TILE_DESERT, 30, 95, 40, 20)
fill_zone_rect(ZONE_DESERT, 30, 95, 40, 20)

# Sandfall Oasis (SW desert)
place_town(45, 102)
# Pyramid of Trials (desert dungeon)
place_dungeon(55, 108)

# === SE REGION: Greenleaf (grasslands) + Coral Bay (coast) ===
print("Creating SE region (Starter area)...")
# Grasslands (starter area)
fill_rect(TILE_GRASS, 75, 85, 40, 30)
fill_zone_rect(ZONE_GRASSLAND, 75, 85, 40, 30)

# Greenleaf (SE grasslands, STARTER TOWN)
place_town(95, 100)
# Cave of Earth (first dungeon, near Greenleaf)
place_dungeon(105, 95)

# Coastal area (Coral Bay)
fill_rect(TILE_WATER, 70, 110, 48, 8)
fill_zone_rect(ZONE_COAST, 70, 105, 48, 10)

# Coral Bay (S coast)
place_town(85, 107)
# Water Temple (coastal dungeon)
place_dungeon(75, 106)
# Smuggler's Cave (coastal side dungeon)
place_dungeon(100, 110)

# === Add some varied terrain ===
print("Adding terrain variation...")
# Scattered forests
for _ in range(20):
    x = random.randint(20, 110)
    y = random.randint(20, 110)
    if world_map[y][x] == TILE_GRASS:
        fill_circle(TILE_FOREST, x, y, random.randint(3, 6))

# === LOCATIONS SUMMARY ===
locations = {
    "Towns": [
        ("Greenleaf", 95, 100, "SE - Starter Town"),
        ("Coral Bay", 85, 107, "S - Coastal Port"),
        ("Emberforge", 22, 85, "W - Mountain Town"),
        ("Windspire", 30, 28, "NW - Highland Town"),
        ("Crossroads Market", 72, 60, "Center - Trade Hub"),
        ("Sandfall Oasis", 45, 102, "SW - Desert Oasis"),
        ("Elderwood Grove", 85, 42, "E - Forest Village"),
        ("Scholar Outpost", 95, 25, "NE - Ruins Camp")
    ],
    "Dungeons": [
        ("Cave of Earth", 105, 95, "Near Greenleaf"),
        ("Water Temple", 75, 106, "Near Coral Bay"),
        ("Volcano Keep", 18, 95, "Near Emberforge"),
        ("Sky Tower", 25, 40, "Near Windspire"),
        ("Final Sanctum", 59, 63, "Center Island (LOCKED)"),
        ("Smuggler's Cave", 100, 110, "Coastal Side"),
        ("Pyramid of Trials", 55, 108, "Desert Side"),
        ("Forest Shrine", 78, 35, "Forest Side"),
        ("Bonus Dungeon", 105, 22, "NE Post-game")
    ]
}

print("\n📍 Placed Locations:")
print(f"   Towns: {len(locations['Towns'])}")
for name, x, y, desc in locations['Towns']:
    print(f"     - {name:20} ({x:3}, {y:3})  {desc}")

print(f"\n   Dungeons: {len(locations['Dungeons'])}")
for name, x, y, desc in locations['Dungeons']:
    print(f"     - {name:20} ({x:3}, {y:3})  {desc}")

# === Generate C code ===
print("\n💾 Generating C code...")

with open("SRC/world_map_generated.c", "w") as f:
    f.write("""#include "world_map.h"
#include <string.h>

// Global world map instance
WorldMap g_world_map;

// World Map Data: 128×128 tiles
// Generated by generate_world_map.py
// Total size: 16,384 bytes (16 KB)

static const uint8_t world_map_tiles[WORLD_MAP_HEIGHT][WORLD_MAP_WIDTH] = {
""")

    # Write tile data
    for y in range(HEIGHT):
        f.write("    {")
        for x in range(WIDTH):
            f.write(f"0x{world_map[y][x]:02X}")
            if x < WIDTH - 1:
                f.write(",")
        f.write("}")
        if y < HEIGHT - 1:
            f.write(",")
        f.write(f"  // Row {y}\n")

    f.write("};\n\n")

    # Write zone data
    f.write("""// Encounter zones for each tile
static const uint8_t world_map_zones[WORLD_MAP_HEIGHT][WORLD_MAP_WIDTH] = {
""")

    for y in range(HEIGHT):
        f.write("    {")
        for x in range(WIDTH):
            f.write(f"{zone_map[y][x]}")
            if x < WIDTH - 1:
                f.write(",")
        f.write("}")
        if y < HEIGHT - 1:
            f.write(",")
        f.write(f"  // Row {y}\n")

    f.write("};\n\n")

    # Write location data
    f.write("""// Town coordinates (x, y) and names
static const WorldLocation default_towns[NUM_TOWNS] = {
""")

    for i, (name, x, y, desc) in enumerate(locations['Towns']):
        f.write(f'    {{{x}, {y}, "{name}"}},  // {desc}\n')

    f.write("};\n\n")

    f.write("""// Dungeon coordinates and names
static const WorldLocation default_dungeons[NUM_DUNGEONS] = {
""")

    for i, (name, x, y, desc) in enumerate(locations['Dungeons']):
        f.write(f'    {{{x}, {y}, "{name}"}},  // {desc}\n')

    f.write("};\n\n")

    # Write initialization and helper functions
    f.write("""// Initialize world map
void world_map_init(void) {
    // Copy tile data
    memcpy(g_world_map.tiles, world_map_tiles, sizeof(world_map_tiles));
    memcpy(g_world_map.zones, world_map_zones, sizeof(world_map_zones));

    // Initialize collision based on tile types
    for (int y = 0; y < WORLD_MAP_HEIGHT; y++) {
        for (int x = 0; x < WORLD_MAP_WIDTH; x++) {
            uint8_t tile = g_world_map.tiles[y][x];
            // Water (0x20-0x2F) and mountains (0x30-0x3F) are impassable
            if ((tile >= 0x20 && tile < 0x30) || (tile >= 0x30 && tile < 0x40)) {
                g_world_map.collision[y][x] = 1;  // Blocked
            } else {
                g_world_map.collision[y][x] = 0;  // Walkable
            }
        }
    }

    // Copy location data
    memcpy(g_world_map.towns, default_towns, sizeof(default_towns));
    memcpy(g_world_map.dungeons, default_dungeons, sizeof(default_dungeons));
}

uint8_t world_map_get_tile(uint8_t x, uint8_t y) {
    if (x >= WORLD_MAP_WIDTH || y >= WORLD_MAP_HEIGHT) return 0;
    return g_world_map.tiles[y][x];
}

uint8_t world_map_get_zone(uint8_t x, uint8_t y) {
    if (x >= WORLD_MAP_WIDTH || y >= WORLD_MAP_HEIGHT) return ZONE_NONE;
    return g_world_map.zones[y][x];
}

uint8_t world_map_is_passable(uint8_t x, uint8_t y) {
    if (x >= WORLD_MAP_WIDTH || y >= WORLD_MAP_HEIGHT) return 0;
    return !g_world_map.collision[y][x];
}

void world_map_set_player_position(uint8_t x, uint8_t y) {
    // Player position handling - implement as needed
    (void)x;
    (void)y;
}

WorldLocation* world_map_get_location_at(uint8_t x, uint8_t y) {
    // Check if coordinates match any town or dungeon (within 2x2 area)
    for (int i = 0; i < NUM_TOWNS; i++) {
        int tx = g_world_map.towns[i].x;
        int ty = g_world_map.towns[i].y;
        if (x >= tx && x <= tx+1 && y >= ty && y <= ty+1) {
            return &g_world_map.towns[i];
        }
    }
    for (int i = 0; i < NUM_DUNGEONS; i++) {
        int dx = g_world_map.dungeons[i].x;
        int dy = g_world_map.dungeons[i].y;
        if (x >= dx && x <= dx+1 && y >= dy && y <= dy+1) {
            return &g_world_map.dungeons[i];
        }
    }
    return NULL;
}
""")

print("✅ Generated: SRC/world_map_generated.c")
print(f"   File size: ~{(HEIGHT * WIDTH * 2) / 1024:.1f} KB (tile + zone data)")
print("\n✨ Done! Replace SRC/world_map.c with SRC/world_map_generated.c")

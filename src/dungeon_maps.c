#include "dungeon_maps.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>

// Cave of Earth - Floor 1
static const char cave_of_earth_floor1[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#.....#........#",
    "#.###.#.####...#",
    "#.#.........##.#",
    "#.#.#######..#.#",
    "#...#........#.#",
    "#.###.#T#.#..#.#",
    "#.#...###.#..#.#",
    "#.#.......#..#.#",
    "#.#########..#.#",
    "#............#.#",
    "#.############.#",
    "#..............#",
    "#U.........D...#",
    "################",
    "################"
};

// Cave of Earth - Floor 2
static const char cave_of_earth_floor2[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#.####.####.##.#",
    "#.#.........##.#",
    "#.#.##T####.##.#",
    "#.#.##.....###.#",
    "#.#.##.###...#.#",
    "#.#....#.#...#.#",
    "#.#####.##...#.#",
    "#.......##T..#.#",
    "#U#######....#.#",
    "#............#.#",
    "####.##########",
    "#..............#",
    "#.......D......#",
    "################"
};

// Cave of Earth - Floor 3 (Boss)
static const char cave_of_earth_floor3[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#.............T#",
    "#..............#",
    "#.....####.....#",
    "#.....#..#.....#",
    "#.....#..#.....#",
    "#U....#..#.....#",
    "#.....#..#.....#",
    "#.....####.....#",
    "#..............#",
    "#.......B......#",
    "#..............#",
    "#..............#",
    "#..............#",
    "################"
};

// Water Temple - Floor 1
static const char water_temple_floor1[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#.#.#.#.#.#.#..#",
    "#.#.#.#.#.#.#..#",
    "#.#.#.#.#.#.#..#",
    "#.............T#",
    "##.###########.#",
    "#..............#",
    "#.############.#",
    "#.#..........#.#",
    "#.#.T#######.#.#",
    "#.#..........#.#",
    "#.######.#####.#",
    "#..............#",
    "#U............D#",
    "################",
    "################"
};

// Water Temple - Floor 2
static const char water_temple_floor2[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#......#.......#",
    "#.####.#.#####.#",
    "#.#....#.....#.#",
    "#.#.########.#.#",
    "#.#..........#.#",
    "#.#T########.#.#",
    "#.#..........#.#",
    "#.############.#",
    "#..............#",
    "##############.#",
    "#U...T.........#",
    "#.############.#",
    "#..............#",
    "#.............D#",
    "################"
};

// Water Temple - Floor 3 (Boss)
static const char water_temple_floor3[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#...##....##...#",
    "#...#......#...#",
    "#...#......#...#",
    "#...#......#...#",
    "#..............#",
    "#U.....B.......#",
    "#..............#",
    "#...#......#...#",
    "#...#......#...#",
    "#...#......#T..#",
    "#...##....##...#",
    "#..............#",
    "#..............#",
    "################"
};

// Volcano Keep - Floor 1
static const char volcano_keep_floor1[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#.###.#########",
    "#.#.#.#........#",
    "#.#...#.######.#",
    "#.#####.#....#.#",
    "#.......#.##.#.#",
    "########.T##...#",
    "#........###.###",
    "#.######.....#T#",
    "#.#....#####.#.#",
    "#.#..........#.#",
    "#.############.#",
    "#U.............#",
    "##..D..........#",
    "################"
};

// Volcano Keep - Floor 2
static const char volcano_keep_floor2[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#.......#......#",
    "#.#####.#.####.#",
    "#.#...#.#.#..#.#",
    "#.#.#.#...#..#.#",
    "#.#.#.#####..#.#",
    "#.#.#........#.#",
    "#U#.##########.#",
    "#.#.#.......T#.#",
    "#.#.#.######..#.",
    "#.#...#.......##",
    "#.#####.####.###",
    "#.......#......#",
    "#########.####.#",
    "#.....D........#",
    "################"
};

// Volcano Keep - Floor 3 (Boss)
static const char volcano_keep_floor3[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#..##########..#",
    "#..#........#..#",
    "#..#.T####..#..#",
    "#..#..#..#..#..#",
    "#..#..#..#..#..#",
    "#U....#..#.....#",
    "#..#..#..#..#..#",
    "#..#..B..#..#..#",
    "#..#..####..#..#",
    "#..#........#..#",
    "#..##########..#",
    "#..............#",
    "#..............#",
    "################"
};

// Sky Tower - Floor 1
static const char sky_tower_floor1[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#..##########..#",
    "#..#........#..#",
    "#..#.######.#..#",
    "#..#.#T...#.#..#",
    "#..#...##.#.#..#",
    "#..#.#....#.#..#",
    "#..#.######.#..#",
    "#..#...........#",
    "#..##########..#",
    "#..............#",
    "#U.............#",
    "#..............#",
    "#......D.......#",
    "################"
};

// Sky Tower - Floor 2
static const char sky_tower_floor2[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#.############.#",
    "#.#..........#.#",
    "#.#.########.#.#",
    "#.#.#......#.#.#",
    "#.#.#.#.##T#.#.#",
    "#U#.#.#....#.#.#",
    "#.#.#.#.##.#.#.#",
    "#.#.#.#....#.#.#",
    "#.#.#.######.#.#",
    "#.#............#",
    "##############.#",
    "#..............#",
    "#.......D......#",
    "################"
};

// Sky Tower - Floor 3 (Boss)
static const char sky_tower_floor3[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#.#.#.#.#.#.#.##",
    "#.#.#.#.#.#.#..#",
    "#.#.#.#.#.#.#..#",
    "#.#.#.#.#.#.#..#",
    "#..............#",
    "#U.............#",
    "#..............#",
    "#.#.#.#.#.#.#..#",
    "#.#.#.#.#.#.#..#",
    "#.#.#.#.#.#.#..#",
    "##.#.#.#.#.#.#.#",
    "#........B.....#",
    "#.....T........#",
    "################"
};

// Final Sanctum - Floor 1
static const char final_sanctum_floor1[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#.####....####.#",
    "#.#..#....#..#.#",
    "#.#..#....#..#.#",
    "#.#..#T..T#..#.#",
    "#.#..#....#..#.#",
    "#.#..########..#",
    "#.#............#",
    "#.##############",
    "#..............#",
    "#U............D#",
    "################",
    "################",
    "################",
    "################"
};

// Final Sanctum - Floor 2
static const char final_sanctum_floor2[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#U...........#.#",
    "####.#######.#.#",
    "#....#.....#.#.#",
    "#.####.###.#.#.#",
    "#.#....#.#.#.#.#",
    "#.#.####.#T#.#.#",
    "#.#..........#.#",
    "#.#####.#####.#.",
    "#.....#.#.....##",
    "#####.#.#.######",
    "#.....#........#",
    "#.#############",
    "#..............#",
    "#.............D#",
    "################"
};

// Final Sanctum - Floor 3
static const char final_sanctum_floor3[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#.############.#",
    "#.#U.........#.#",
    "#.#.########.#.#",
    "#.#.#T.......#.#",
    "#.#.#.####.#.#.#",
    "#.#.#.#..#.#.#.#",
    "#.#.#.#..#.#.#.#",
    "#.#.#.####.#.#.#",
    "#.#.#......#.#.#",
    "#.#.########.#.#",
    "#.#..........#.#",
    "#.########.###.#",
    "#.......D......#",
    "################"
};

// Final Sanctum - Floor 4
static const char final_sanctum_floor4[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..#........#..#",
    "#..#.######.#..#",
    "#..#.#....#.#..#",
    "#..#.#.##.#.#..#",
    "#..#.#.##.#.#..#",
    "#..#.#T##.#.#..#",
    "#U........#.#..#",
    "#..#.######.#..#",
    "#..#........#..#",
    "#..##########..#",
    "#..............#",
    "#.#############",
    "#..............#",
    "#.......D......#",
    "################"
};

// Final Sanctum - Floor 5 (Final Boss)
static const char final_sanctum_floor5[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1] = {
    "################",
    "#..............#",
    "#..............#",
    "#....######....#",
    "#....#....#....#",
    "#....#.##.#....#",
    "#....#.##.#....#",
    "#U...#T##.#....#",
    "#....#.##.#....#",
    "#..............#",
    "#....######....#",
    "#..............#",
    "#.......B......#",
    "#..............#",
    "#..............#",
    "################"
};

// Map lookup table
const char (*dungeon_get_fixed_map(uint8_t dungeon_id, uint8_t floor_number))[DUNGEON_WIDTH + 1] {
    switch (dungeon_id) {
        case 0: // Cave of Earth
            switch (floor_number) {
                case 0: return cave_of_earth_floor1;
                case 1: return cave_of_earth_floor2;
                case 2: return cave_of_earth_floor3;
            }
            break;

        case 1: // Water Temple
            switch (floor_number) {
                case 0: return water_temple_floor1;
                case 1: return water_temple_floor2;
                case 2: return water_temple_floor3;
            }
            break;

        case 2: // Volcano Keep
            switch (floor_number) {
                case 0: return volcano_keep_floor1;
                case 1: return volcano_keep_floor2;
                case 2: return volcano_keep_floor3;
            }
            break;

        case 3: // Sky Tower
            switch (floor_number) {
                case 0: return sky_tower_floor1;
                case 1: return sky_tower_floor2;
                case 2: return sky_tower_floor3;
            }
            break;

        case 4: // Final Sanctum
            switch (floor_number) {
                case 0: return final_sanctum_floor1;
                case 1: return final_sanctum_floor2;
                case 2: return final_sanctum_floor3;
                case 3: return final_sanctum_floor4;
                case 4: return final_sanctum_floor5;
            }
            break;
    }

    return NULL;
}

void dungeon_load_fixed_map(DungeonFloor* floor, const char map[DUNGEON_HEIGHT][DUNGEON_WIDTH + 1], uint8_t floor_number) {
    if (!floor || !map) return;

    // Clear floor
    memset(floor, 0, sizeof(DungeonFloor));

    // Initialize encounter counter (random 15-30 steps)
    floor->encounter_steps = random_range(15, 30);

    // Parse map
    for (int y = 0; y < DUNGEON_HEIGHT; y++) {
        for (int x = 0; x < DUNGEON_WIDTH; x++) {
            DungeonTile* tile = &floor->tiles[y][x];
            tile->explored = false;
            tile->encounter_rate = 20 + (floor_number * 5); // Still used for certain mechanics

            char c = map[y][x];
            switch (c) {
                case '#':
                    tile->type = TILE_WALL;
                    break;

                case '.':
                    tile->type = TILE_FLOOR;
                    break;

                case 'E':
                    tile->type = TILE_ENTRANCE;
                    floor->player_x = x;
                    floor->player_y = y;
                    tile->explored = true;
                    break;

                case 'D':
                    tile->type = TILE_STAIRS_DOWN;
                    break;

                case 'U':
                    tile->type = TILE_STAIRS_UP;
                    floor->player_x = x;
                    floor->player_y = y;
                    tile->explored = true;
                    break;

                case 'T':
                    tile->type = TILE_TREASURE;
                    break;

                case 'B':
                    tile->type = TILE_BOSS_ROOM;
                    break;

                default:
                    tile->type = TILE_FLOOR;
                    break;
            }
        }
    }
}

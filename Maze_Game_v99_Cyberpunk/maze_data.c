/*
 * MAZE_DATA.C - Maze layouts and reference point definitions
 *
 * CYBERPUNK CORPORATE GRID
 * "Sector 7-G: Maximum Security Zone"
 *
 * This file contains the neon maze map with reference points
 * for cyberpunk gameplay elements.
 *
 * By: VonHoltenCodes (2025)
 */

#include "maze.h"
#include <string.h>

/*
 * Cyberpunk Maze Map Legend:
 * 0  = Empty cyberspace (walkable)
 * 1  = Neon grid panels (cyan)
 * 2  = Circuit boards (green)
 * 3  = Holographic barriers (magenta)
 * 4  = Data streams (purple)
 * 10 = Jack-in point
 * 20 = Data extraction point
 * 30-32 = Security bot spawn points
 * 40-42 = Item pickups (nanobots, energy, keycard)
 * 50-51 = Security terminals
 * 60 = Security firewall
 */

/* Global maze map - 24x24 grid */
int mazeLayout[MAP_HEIGHT][MAP_WIDTH] = {
    /* 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 */
    {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 0 */
    {  1,10, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 1 - Spawn at (1,1) */
    {  1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1 },  /* 2 */
    {  1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1 },  /* 3 */
    {  1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1 },  /* 4 */
    {  1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,40, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 5 - Health at (11,5) */
    {  1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },  /* 6 */
    {  1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 7 */
    {  1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 },  /* 8 */
    {  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,30, 0, 0, 0, 1 },  /* 9 - Enemy at (19,9) */
    {  1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },  /* 10 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 11 */
    {  1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },  /* 12 */
    {  1, 0, 1, 0, 0, 0, 0, 0, 0,60, 0, 1, 0,50, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 13 - Door at (9,13), Trigger at (13,13) */
    {  1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },  /* 14 */
    {  1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,42, 0, 1 },  /* 15 - Key at (21,15) */
    {  1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1 },  /* 16 */
    {  1, 0, 0, 0, 0, 0, 1, 0, 0, 0,31, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },  /* 17 - Enemy at (10,17) */
    {  1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1 },  /* 18 */
    {  1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,41, 0, 0, 0, 0, 1 },  /* 19 - Ammo at (18,19) */
    {  1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },  /* 20 */
    {  1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },  /* 21 */
    {  1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,20, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },  /* 22 - Exit at (12,22) */
    {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }   /* 23 */
};

/* Reference points array - cyberpunk themed locations */
ReferencePoint referencePoints[] = {
    { REF_PLAYER_SPAWN, 1,  1,  1, "Neural Jack-In Point" },
    { REF_EXIT,         12, 22, 1, "Data Extraction Node" },
    { REF_ENEMY_1,      19, 9,  1, "Security Bot Alpha" },
    { REF_ENEMY_2,      10, 17, 1, "Security Bot Beta" },
    { REF_ITEM_HEALTH,  11, 5,  1, "Med-Pack Nanobots" },
    { REF_ITEM_AMMO,    18, 19, 1, "Energy Cell Depot" },
    { REF_ITEM_KEY,     21, 15, 1, "Access Keycard" },
    { REF_TRIGGER_1,    13, 13, 1, "Security Terminal" },
    { REF_DOOR,         9,  13, 1, "Corporate Firewall" },
    { 0, 0, 0, 0, "" } /* Terminator */
};

/* Initialize maze with the predefined layout */
void initMaze(Maze *maze) {
    int x, y;

    maze->width = MAP_WIDTH;
    maze->height = MAP_HEIGHT;

    /* Copy the maze layout */
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            maze->cells[y][x] = mazeLayout[y][x];
        }
    }
}

/* Get cell value at position */
int getMazeCell(Maze *maze, int x, int y) {
    if (x < 0 || x >= maze->width || y < 0 || y >= maze->height) {
        return CELL_WALL_BRICK; /* Treat out-of-bounds as walls */
    }
    return maze->cells[y][x];
}

/* Set cell value at position */
void setMazeCell(Maze *maze, int x, int y, int value) {
    if (x >= 0 && x < maze->width && y >= 0 && y < maze->height) {
        maze->cells[y][x] = value;
    }
}

/* Check if a cell value represents a wall */
int isWall(int cellValue) {
    return (cellValue >= CELL_WALL_BRICK && cellValue <= CELL_WALL_WOOD);
}

/* Check if a cell value is a reference point */
int isReferencePoint(int cellValue) {
    return (cellValue >= REF_PLAYER_SPAWN);
}

/* Initialize reference points system */
void initReferencePoints(void) {
    /* Reference points are already defined in the array above */
    /* This function can be expanded for dynamic initialization */
}

/* Get reference point at specific grid location */
ReferencePoint* getReferencePoint(int x, int y) {
    int i = 0;

    while (referencePoints[i].id != 0) {
        if (referencePoints[i].x == x && referencePoints[i].y == y) {
            return &referencePoints[i];
        }
        i++;
    }

    return NULL; /* No reference point at this location */
}

/* Print all reference points (for debugging) */
void printReferencePoints(void) {
    int i = 0;

    printf("\n");
    printf("========================================\n");
    printf(" GRID REFERENCE POINTS - SECTOR 7-G\n");
    printf("========================================\n\n");

    while (referencePoints[i].id != 0) {
        printf(" [%2d] <%2d,%2d> %s %s\n",
            referencePoints[i].id,
            referencePoints[i].x,
            referencePoints[i].y,
            referencePoints[i].name,
            referencePoints[i].active ? "[ONLINE]" : "[OFFLINE]"
        );
        i++;
    }

    printf("\n========================================\n\n");
}

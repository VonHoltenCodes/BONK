/*
 * MAZE.H - Header file for Von Holten Maze Game
 *
 * CYBERPUNK 3D Raycasting Maze Explorer
 * "Welcome to the Grid, Runner..."
 *
 * Inspired by Doom, Descent, and BONK DOS Edition
 * Theme: Neon-Lit Corporate Cyberspace
 *
 * By: VonHoltenCodes (2025)
 */

#ifndef MAZE_H
#define MAZE_H

/* VGA Mode 13h constants */
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define SCREEN_CENTER (SCREEN_HEIGHT / 2)

/* Maze dimensions */
#define MAP_WIDTH  24
#define MAP_HEIGHT 24

/* Map cell types - CYBERPUNK THEME */
#define CELL_EMPTY           0   /* Walkable cyberspace */
#define CELL_WALL_NEON_GRID  1   /* Cyan neon grid panels */
#define CELL_WALL_CIRCUIT    2   /* Green circuit boards */
#define CELL_WALL_HOLOGRAM   3   /* Magenta holographic barriers */
#define CELL_WALL_DATA       4   /* Purple data streams */

/* Reference point IDs (10+) - CYBERPUNK THEME */
#define REF_PLAYER_SPAWN  10  /* Jack-in point */
#define REF_EXIT          20  /* Data extraction point */
#define REF_ENEMY_1       30  /* Security bot spawn 1 */
#define REF_ENEMY_2       31  /* Security bot spawn 2 */
#define REF_ENEMY_3       32  /* Security bot spawn 3 */
#define REF_ITEM_HEALTH   40  /* Med-pack nanobots */
#define REF_ITEM_AMMO     41  /* Energy cell */
#define REF_ITEM_KEY      42  /* Access keycard */
#define REF_TRIGGER_1     50  /* Security terminal */
#define REF_TRIGGER_2     51  /* Override switch */
#define REF_DOOR          60  /* Security firewall */

/* Player movement constants */
#define MOVE_SPEED     0.1    /* Forward/backward movement */
#define ROTATE_SPEED   0.05   /* Rotation speed */

/* Rendering constants */
#define FOV            66     /* Field of view in degrees */
#define RAY_STEP       0.02   /* Step size for ray marching */
#define MAX_DEPTH      20.0   /* Maximum ray distance */

/* VGA color palette (Mode 13h) - CYBERPUNK NEON THEME */
#define COLOR_BLACK      0
#define COLOR_BLUE       1
#define COLOR_GREEN      2
#define COLOR_CYAN       3   /* Neon cyan - primary accent */
#define COLOR_RED        4
#define COLOR_MAGENTA    5   /* Neon magenta - secondary accent */
#define COLOR_BROWN      6
#define COLOR_WHITE      7
#define COLOR_GRAY       8
#define COLOR_LBLUE      9   /* Light blue - highlights */
#define COLOR_LGREEN     10  /* Neon green - circuit glow */
#define COLOR_LCYAN      11  /* Bright cyan - active elements */
#define COLOR_LRED       12  /* Neon pink/red - danger */
#define COLOR_LMAGENTA   13  /* Bright magenta - holograms */
#define COLOR_YELLOW     14  /* Warning yellow */
#define COLOR_BWHITE     15  /* Bright white - maximum glow */

/* Cyberpunk themed color aliases */
#define NEON_CYAN        COLOR_LCYAN
#define NEON_MAGENTA     COLOR_LMAGENTA
#define NEON_GREEN       COLOR_LGREEN
#define NEON_PINK        COLOR_LRED
#define NEON_BLUE        COLOR_LBLUE
#define CYBER_PURPLE     COLOR_MAGENTA
#define GRID_DARK        (COLOR_CYAN - 2)
#define VOID_BLACK       COLOR_BLACK

/* Maze map structure */
typedef struct {
    int cells[MAP_HEIGHT][MAP_WIDTH];
    int width;
    int height;
} Maze;

/* Player structure */
typedef struct {
    double x;           /* X position in maze */
    double y;           /* Y position in maze */
    double angle;       /* Direction angle (radians) */
    double dirX;        /* Direction vector X */
    double dirY;        /* Direction vector Y */
    double planeX;      /* Camera plane X */
    double planeY;      /* Camera plane Y */
} Player;

/* Reference point structure for special locations */
typedef struct {
    int id;             /* Reference ID (10+) */
    int x;              /* Grid X position */
    int y;              /* Grid Y position */
    int active;         /* Is this reference point active? */
    char name[32];      /* Description */
} ReferencePoint;

/* Function prototypes */

/* Maze functions */
void initMaze(Maze *maze);
int getMazeCell(Maze *maze, int x, int y);
void setMazeCell(Maze *maze, int x, int y, int value);
int isWall(int cellValue);
int isReferencePoint(int cellValue);

/* Reference point functions */
void initReferencePoints(void);
ReferencePoint* getReferencePoint(int x, int y);
void printReferencePoints(void);

/* Player functions */
void initPlayer(Player *player, Maze *maze);
void movePlayer(Player *player, Maze *maze, double moveDir);
void rotatePlayer(Player *player, double rotateDir);

/* Rendering functions */
void renderFrame(Player *player, Maze *maze);
void drawVerticalLine(int x, int drawStart, int drawEnd, unsigned char color);
unsigned char getWallColor(int cellType, double distance);

/* VGA functions (from BONK DOS) */
void setVideoMode(int mode);
void setPixel(int x, int y, unsigned char color);
void clearScreen(unsigned char color);

/* Input functions */
void handleInput(Player *player, Maze *maze, int *running);

/* Utility functions */
double degreesToRadians(double degrees);
void delay_ms(int ms);

#endif /* MAZE_H */

/*
 * ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
 * █ ▄▄▀█▄█▄▄▀█▄▄▀█▄▄▀█▄▄▀█▄▄ █▄▄ █▄▄ █▄▄▀
 * █ ▀▀ ████▀▀▄█▀▀▄█▀▀▄█▄▄▀█▄▄ █▄▄ █▄▄ █▀▀▄
 * ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
 *
 * CYBERPUNK MAZE RUNNER
 * "Welcome to the Grid, Runner..."
 *
 * A Neon-Lit 3D Raycasting Corporate Cyberspace
 *
 * Inspired by: Doom, Descent, Wolfenstein 3D
 * Based on: BONK v9.5 DOS Edition techniques
 * Theme: Cyberpunk 2077 / Blade Runner / Tron
 *
 * Compile: gcc -Wall -O2 -march=i386 maze.c maze_data.c -o MAZE.EXE -lm -s
 * Target: MS-DOS 3.3+, 386+ CPU
 * Graphics: VGA Mode 13h (320x200, 256 colors)
 * Input: Keyboard (arrow keys)
 *
 * By: VonHoltenCodes (2025)
 * License: Educational/Recreational Use Only
 */

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "maze.h"

#ifdef __DJGPP__
#include <sys/nearptr.h>
#include <sys/farptr.h>
#include <go32.h>
#include <dpmi.h>
#include <pc.h>
#define outp(port, val) outportb(port, val)
#define inp(port) inportb(port)
#define VGA_MEMORY ((unsigned char *)(0xA0000 + __djgpp_conventional_base))
#else
#define VGA_MEMORY ((unsigned char far *)0xA0000000L)
#endif

/*============================================================================
 * VGA GRAPHICS FUNCTIONS
 *===========================================================================*/

void setVideoMode(int mode) {
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = mode;
    int86(0x10, &regs, &regs);
}

void setPixel(int x, int y, unsigned char color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        VGA_MEMORY[y * SCREEN_WIDTH + x] = color;
    }
}

void clearScreen(unsigned char color) {
    int i;
    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        VGA_MEMORY[i] = color;
    }
}

void drawVerticalLine(int x, int drawStart, int drawEnd, unsigned char color) {
    int y;
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (drawStart < 0) drawStart = 0;
    if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

    for (y = drawStart; y <= drawEnd; y++) {
        VGA_MEMORY[y * SCREEN_WIDTH + x] = color;
    }
}

/*============================================================================
 * PLAYER FUNCTIONS
 *===========================================================================*/

void initPlayer(Player *player, Maze *maze) {
    int x, y;

    /* Find spawn point in maze (ID 10) */
    for (y = 0; y < maze->height; y++) {
        for (x = 0; x < maze->width; x++) {
            if (getMazeCell(maze, x, y) == REF_PLAYER_SPAWN) {
                player->x = x + 0.5;
                player->y = y + 0.5;

                /* Clear spawn marker so player can walk there */
                setMazeCell(maze, x, y, CELL_EMPTY);
                goto spawn_found;
            }
        }
    }

spawn_found:
    /* Initialize direction (facing east) */
    player->angle = 0.0;
    player->dirX = 1.0;
    player->dirY = 0.0;

    /* Initialize camera plane (perpendicular to direction) */
    player->planeX = 0.0;
    player->planeY = 0.66; /* FOV adjustment */
}

void movePlayer(Player *player, Maze *maze, double moveDir) {
    double newX = player->x + player->dirX * moveDir * MOVE_SPEED;
    double newY = player->y + player->dirY * moveDir * MOVE_SPEED;

    /* Collision detection - check if new position is walkable */
    if (!isWall(getMazeCell(maze, (int)newX, (int)player->y))) {
        player->x = newX;
    }
    if (!isWall(getMazeCell(maze, (int)player->x, (int)newY))) {
        player->y = newY;
    }
}

void rotatePlayer(Player *player, double rotateDir) {
    double angle = rotateDir * ROTATE_SPEED;
    double oldDirX = player->dirX;
    double oldPlaneX = player->planeX;

    /* Rotate direction vector */
    player->dirX = player->dirX * cos(angle) - player->dirY * sin(angle);
    player->dirY = oldDirX * sin(angle) + player->dirY * cos(angle);

    /* Rotate camera plane */
    player->planeX = player->planeX * cos(angle) - player->planeY * sin(angle);
    player->planeY = oldPlaneX * sin(angle) + player->planeY * cos(angle);
}

/*============================================================================
 * RENDERING FUNCTIONS
 *===========================================================================*/

unsigned char getWallColor(int cellType, double distance) {
    unsigned char baseColor;

    /* Select CYBERPUNK base color based on wall type */
    switch (cellType) {
        case CELL_WALL_NEON_GRID:
            baseColor = NEON_CYAN;      /* Cyan neon grid panels */
            break;
        case CELL_WALL_CIRCUIT:
            baseColor = NEON_GREEN;     /* Green circuit boards */
            break;
        case CELL_WALL_HOLOGRAM:
            baseColor = NEON_MAGENTA;   /* Magenta holographic barriers */
            break;
        case CELL_WALL_DATA:
            baseColor = CYBER_PURPLE;   /* Purple data streams */
            break;
        default:
            baseColor = NEON_CYAN;      /* Default to cyan */
            break;
    }

    /* Cyberpunk fog effect - fade to void */
    if (distance > 15.0) {
        return VOID_BLACK;              /* Deep void */
    } else if (distance > 10.0) {
        return COLOR_BLUE;              /* Dark blue haze */
    } else if (distance > 5.0) {
        return baseColor - 3;           /* Dimmed neon */
    } else {
        return baseColor;               /* Full neon brightness */
    }
}

void renderFrame(Player *player, Maze *maze) {
    int x;

    /* Draw ceiling (deep blue cyberspace void) */
    for (x = 0; x < SCREEN_WIDTH * SCREEN_CENTER; x++) {
        VGA_MEMORY[x] = COLOR_BLUE;
    }

    /* Draw floor (dark grid with slight cyan tint) */
    for (x = SCREEN_WIDTH * SCREEN_CENTER; x < SCREEN_WIDTH * SCREEN_HEIGHT; x++) {
        VGA_MEMORY[x] = VOID_BLACK;
    }

    /* Raycasting for each vertical stripe */
    for (x = 0; x < SCREEN_WIDTH; x++) {
        /* Calculate ray position and direction */
        double cameraX = 2 * x / (double)SCREEN_WIDTH - 1;
        double rayDirX = player->dirX + player->planeX * cameraX;
        double rayDirY = player->dirY + player->planeY * cameraX;

        /* Current map position */
        int mapX = (int)player->x;
        int mapY = (int)player->y;

        /* Length of ray from current position to next x or y-side */
        double sideDistX, sideDistY;

        /* Length of ray from one x or y-side to next x or y-side */
        double deltaDistX = fabs(1 / rayDirX);
        double deltaDistY = fabs(1 / rayDirY);
        double perpWallDist;

        /* What direction to step in x or y-direction (either +1 or -1) */
        int stepX, stepY;

        int hit = 0; /* Was there a wall hit? */
        int side; /* Was a NS or a EW wall hit? */

        /* Calculate step and initial sideDist */
        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (player->x - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player->x) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (player->y - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - player->y) * deltaDistY;
        }

        /* Perform DDA */
        while (hit == 0) {
            /* Jump to next map square, either in x-direction, or in y-direction */
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            /* Check if ray has hit a wall */
            int cellValue = getMazeCell(maze, mapX, mapY);
            if (isWall(cellValue) || isReferencePoint(cellValue)) {
                hit = 1;
            }
        }

        /* Calculate distance to wall */
        if (side == 0) {
            perpWallDist = (mapX - player->x + (1 - stepX) / 2) / rayDirX;
        } else {
            perpWallDist = (mapY - player->y + (1 - stepY) / 2) / rayDirY;
        }

        /* Calculate height of line to draw on screen */
        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

        /* Calculate lowest and highest pixel to fill in current stripe */
        int drawStart = -lineHeight / 2 + SCREEN_CENTER;
        int drawEnd = lineHeight / 2 + SCREEN_CENTER;

        /* Get wall color */
        int cellValue = getMazeCell(maze, mapX, mapY);
        unsigned char color = getWallColor(cellValue, perpWallDist);

        /* Make EW walls darker for depth perception */
        if (side == 1) {
            color = (color > 0) ? color - 1 : color;
        }

        /* Draw the vertical line */
        drawVerticalLine(x, drawStart, drawEnd, color);
    }
}

/*============================================================================
 * INPUT HANDLING
 *===========================================================================*/

void handleInput(Player *player, Maze *maze, int *running) {
    if (kbhit()) {
        int key = getch();

        /* Check for extended keys (arrows) */
        if (key == 0 || key == 224) {
            key = getch(); /* Get the actual key code */

            switch (key) {
                case 72: /* Up arrow - move forward */
                    movePlayer(player, maze, 1.0);
                    break;
                case 80: /* Down arrow - move backward */
                    movePlayer(player, maze, -1.0);
                    break;
                case 75: /* Left arrow - rotate left */
                    rotatePlayer(player, 1.0);
                    break;
                case 77: /* Right arrow - rotate right */
                    rotatePlayer(player, -1.0);
                    break;
            }
        } else if (key == 27) { /* ESC */
            *running = 0;
        }
    }
}

/*============================================================================
 * UTILITY FUNCTIONS
 *===========================================================================*/

double degreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

void delay_ms(int ms) {
    unsigned long start = clock();
    unsigned long ticks = ms * CLOCKS_PER_SEC / 1000;
    while (clock() - start < ticks);
}

/*============================================================================
 * MAIN PROGRAM
 *===========================================================================*/

int main(void) {
    Maze maze;
    Player player;
    int running = 1;

    printf("\n");
    printf("========================================\n");
    printf("  CYBERPUNK MAZE RUNNER\n");
    printf("  >> NEURAL INTERFACE ACTIVE <<\n");
    printf("========================================\n");
    printf("\n");
    printf("[ SYSTEM ] Initializing neural uplink...\n");

#ifdef __DJGPP__
    if (!__djgpp_nearptr_enable()) {
        printf("ERROR: Could not enable near pointers!\n");
        return 1;
    }
#endif

    /* Initialize game systems */
    initMaze(&maze);
    initReferencePoints();
    initPlayer(&player, &maze);

    printf("[ GRID  ] Corporate grid loaded: %dx%d sectors\n", maze.width, maze.height);
    printf("[ SPAWN ] Jack-in coordinates: (%.1f, %.1f)\n", player.x, player.y);
    printf("\n");

    /* Print reference points for debugging */
    printReferencePoints();

    printf("========================================\n");
    printf(" RUNNER CONTROLS:\n");
    printf("========================================\n");
    printf("  [UP]    Navigate forward\n");
    printf("  [DOWN]  Navigate backward\n");
    printf("  [LEFT]  Rotate left\n");
    printf("  [RIGHT] Rotate right\n");
    printf("  [ESC]   Disconnect from grid\n");
    printf("\n");
    printf("[ READY ] Press any key to jack in...\n");
    getch();

    /* Enter VGA mode */
    setVideoMode(0x13);

    /* Main game loop */
    while (running) {
        /* Render current frame */
        renderFrame(&player, &maze);

        /* Handle input */
        handleInput(&player, &maze, &running);

        /* Small delay to prevent CPU spinning */
        delay_ms(20);
    }

    /* Exit VGA mode */
    setVideoMode(0x03);

    printf("\n");
    printf("========================================\n");
    printf("  >> CONNECTION TERMINATED <<\n");
    printf("  Neural interface disconnected\n");
    printf("  Thank you for running the grid\n");
    printf("\n");
    printf("  VonHoltenCodes 2025\n");
    printf("  Cyberpunk Maze Runner v1.0\n");
    printf("========================================\n");
    printf("\n");

#ifdef __DJGPP__
    __djgpp_nearptr_disable();
#endif

    return 0;
}

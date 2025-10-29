/*
 * ██████   ██████  ███    ██ ██   ██     ██    ██  █████
 * ██   ██ ██    ██ ████   ██ ██  ██      ██    ██ ██   ██
 * ██████  ██    ██ ██ ██  ██ █████       ██    ██  █████
 * ██   ██ ██    ██ ██  ██ ██ ██  ██       ██  ██  ██   ██
 * ██████   ██████  ██   ████ ██   ██       ████    █████
 *
 * BONK v9.5 - PURE DOS EDITION
 *
 * Compile: DJGPP or Turbo C++
 * Target: MS-DOS 3.3+, Windows 95/98 DOS mode
 * Graphics: VGA Mode 13h (320x200, 256 colors)
 * Input: Microsoft Mouse driver (int 0x33)
 * Sound: PC Speaker / Sound Blaster
 *
 * Hardware Requirements:
 *   - 386+ CPU (486 recommended)
 *   - VGA graphics
 *   - MS-DOS mouse driver
 *   - 512 KB RAM
 *
 * Optimized for: RetroNobilis (800MHz Celeron, Voodoo3, SB Live!)
 *
 * By: VonHoltenCodes (2025)
 * License: Educational/Recreational Use Only
 */

#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#ifdef __DJGPP__
#include <sys/nearptr.h>
#include <sys/farptr.h>
#include <go32.h>
#include <dpmi.h>
#include <pc.h>
/* DJGPP port I/O compatibility */
#define outp(port, val) outportb(port, val)
#define inp(port) inportb(port)
#endif

/* VGA Mode 13h constants */
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200

#ifdef __DJGPP__
#define VGA_MEMORY    ((unsigned char *)(0xA0000 + __djgpp_conventional_base))
#else
#define VGA_MEMORY    ((unsigned char far *)0xA0000000L)
#endif

/* Game constants */
#define TARGET_SIZE   24
#define SPECIAL_SIZE  16
#define GAME_TIME     45
#define MAX_TARGETS   5

/* VGA Color palette indices */
#define BLACK      0
#define BLUE       1
#define GREEN      2
#define CYAN       3
#define RED        4
#define MAGENTA    5
#define BROWN      6
#define WHITE      7
#define GRAY       8
#define LBLUE      9
#define LGREEN     10
#define LCYAN      11
#define LRED       12
#define LMAGENTA   13
#define YELLOW     14
#define BWHITE     15
#define ORANGE     208
#define DARK_GRAY  242

/* Game variant types */
typedef enum {
    VARIANT_CHICKEN = 0,
    VARIANT_BUNNY = 1,
    VARIANT_DRAGON = 2
} GameVariant;

/* Difficulty levels */
typedef enum {
    DIFF_EASY = 0,
    DIFF_HARD = 1
} GameDifficulty;

/* Target structure */
typedef struct {
    int x, y;
    int active;
    unsigned long spawnTime;
    unsigned long displayTime;
} Target;

/* Special item structure */
typedef struct {
    int x, y;
    int active;
} SpecialItem;

/* Game state */
typedef struct {
    GameVariant variant;
    GameDifficulty difficulty;
    int score;
    int specialScore;
    int timeLeft;
    int bonkStreak;
    int frenzyMode;
    Target targets[MAX_TARGETS];
    SpecialItem special;
    unsigned long lastSpawn;
    unsigned long lastSecond;
    unsigned long startTime;
} GameState;

/* Mouse state */
typedef struct {
    int x, y;
    int buttons;
    int visible;
} MouseState;

/* Function prototypes */
void setVideoMode(int mode);
void setPixel(int x, int y, unsigned char color);
void drawLine(int x1, int y1, int x2, int y2, unsigned char color);
void drawRect(int x1, int y1, int x2, int y2, unsigned char color);
void fillRect(int x1, int y1, int x2, int y2, unsigned char color);
void drawCircle(int cx, int cy, int radius, unsigned char color);
void fillCircle(int cx, int cy, int radius, unsigned char color);
void clearScreen(unsigned char color);
void drawText(int x, int y, const char *text, unsigned char color);

/* Mouse functions */
int initMouse(void);
void showMouse(void);
void hideMouse(void);
void readMouse(MouseState *mouse);

/* Sound functions */
void playBeep(int freq, int duration);
void playBonkSound(void);
void playSpecialSound(void);
void playMenuJingle(void);

/* Game drawing functions */
void drawChicken(int x, int y, int withFlames);
void drawBunny(int x, int y, int withFlames);
void drawDragon(int x, int y, int withFlames);
void drawFlame(int x, int y);
void drawEgg(int x, int y);
void drawFish(int x, int y);

/* Game logic functions */
void initGame(GameState *game);
void updateGame(GameState *game);
void spawnTarget(GameState *game);
void checkHit(GameState *game, int mx, int my);
void drawGameScreen(GameState *game);
void drawTitleScreen(void);
GameVariant showMenu(void);

/* Utility functions */
unsigned long getTickCount(void);
void delay_ms(int ms);

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

void drawLine(int x1, int y1, int x2, int y2, unsigned char color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        setPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void fillRect(int x1, int y1, int x2, int y2, unsigned char color) {
    int x, y;
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }

    for (y = y1; y <= y2; y++) {
        for (x = x1; x <= x2; x++) {
            setPixel(x, y, color);
        }
    }
}

void fillCircle(int cx, int cy, int radius, unsigned char color) {
    int x, y;
    for (y = -radius; y <= radius; y++) {
        for (x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                setPixel(cx + x, cy + y, color);
            }
        }
    }
}

void clearScreen(unsigned char color) {
    memset(VGA_MEMORY, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

/* Simple ASCII lookup for font */
unsigned char getFont(char c, int row) {
    /* Returns font data for character c at row (0-7) */
    if (row < 0 || row >= 8) return 0;

    switch(c) {
        case ' ': return ((unsigned char[]){0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00})[row];
        case '!': return ((unsigned char[]){0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00})[row];
        case '#': return ((unsigned char[]){0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00})[row];
        case '*': return ((unsigned char[]){0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00})[row];
        case '+': return ((unsigned char[]){0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00})[row];
        case '-': return ((unsigned char[]){0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00})[row];
        case '.': return ((unsigned char[]){0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00})[row];
        case '/': return ((unsigned char[]){0x00,0x06,0x0C,0x18,0x30,0x60,0x00,0x00})[row];
        case ':': return ((unsigned char[]){0x00,0x00,0x18,0x18,0x00,0x18,0x18,0x00})[row];
        case '?': return ((unsigned char[]){0x3C,0x66,0x0C,0x18,0x18,0x00,0x18,0x00})[row];
        case '0': return ((unsigned char[]){0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00})[row];
        case '1': return ((unsigned char[]){0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00})[row];
        case '2': return ((unsigned char[]){0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00})[row];
        case '3': return ((unsigned char[]){0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00})[row];
        case '4': return ((unsigned char[]){0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00})[row];
        case '5': return ((unsigned char[]){0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00})[row];
        case '6': return ((unsigned char[]){0x3C,0x60,0x60,0x7C,0x66,0x66,0x3C,0x00})[row];
        case '7': return ((unsigned char[]){0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00})[row];
        case '8': return ((unsigned char[]){0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00})[row];
        case '9': return ((unsigned char[]){0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00})[row];
        case 'A': case 'a': return ((unsigned char[]){0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00})[row];
        case 'B': case 'b': return ((unsigned char[]){0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00})[row];
        case 'C': case 'c': return ((unsigned char[]){0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00})[row];
        case 'D': case 'd': return ((unsigned char[]){0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00})[row];
        case 'E': case 'e': return ((unsigned char[]){0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00})[row];
        case 'F': case 'f': return ((unsigned char[]){0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00})[row];
        case 'G': case 'g': return ((unsigned char[]){0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00})[row];
        case 'H': case 'h': return ((unsigned char[]){0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00})[row];
        case 'I': case 'i': return ((unsigned char[]){0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00})[row];
        case 'J': case 'j': return ((unsigned char[]){0x06,0x06,0x06,0x06,0x06,0x66,0x3C,0x00})[row];
        case 'K': case 'k': return ((unsigned char[]){0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00})[row];
        case 'L': case 'l': return ((unsigned char[]){0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00})[row];
        case 'M': case 'm': return ((unsigned char[]){0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00})[row];
        case 'N': case 'n': return ((unsigned char[]){0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00})[row];
        case 'O': case 'o': return ((unsigned char[]){0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00})[row];
        case 'P': case 'p': return ((unsigned char[]){0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00})[row];
        case 'Q': case 'q': return ((unsigned char[]){0x3C,0x66,0x66,0x66,0x66,0x3C,0x0E,0x00})[row];
        case 'R': case 'r': return ((unsigned char[]){0x7C,0x66,0x66,0x7C,0x6C,0x66,0x66,0x00})[row];
        case 'S': case 's': return ((unsigned char[]){0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00})[row];
        case 'T': case 't': return ((unsigned char[]){0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00})[row];
        case 'U': case 'u': return ((unsigned char[]){0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00})[row];
        case 'V': case 'v': return ((unsigned char[]){0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00})[row];
        case 'W': case 'w': return ((unsigned char[]){0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00})[row];
        case 'X': case 'x': return ((unsigned char[]){0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00})[row];
        case 'Y': case 'y': return ((unsigned char[]){0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00})[row];
        case 'Z': case 'z': return ((unsigned char[]){0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00})[row];
        case '(': return ((unsigned char[]){0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00})[row];
        case ')': return ((unsigned char[]){0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00})[row];
        default: return 0x00;
    }
}

void drawChar(int x, int y, char c, unsigned char color) {
    int row, col;
    unsigned char bits;

    for (row = 0; row < 8; row++) {
        bits = getFont(c, row);
        for (col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                setPixel(x + col, y + row, color);
            }
        }
    }
}

void drawText(int x, int y, const char *text, unsigned char color) {
    int i;
    for (i = 0; text[i]; i++) {
        drawChar(x + i*8, y, text[i], color);
    }
}

/*============================================================================
 * MOUSE FUNCTIONS
 *===========================================================================*/

int initMouse(void) {
    union REGS regs;
    regs.x.ax = 0x00;
    int86(0x33, &regs, &regs);
    return regs.x.ax; /* Returns 0xFFFF if mouse installed */
}

void showMouse(void) {
    union REGS regs;
    regs.x.ax = 0x01;
    int86(0x33, &regs, &regs);
}

void hideMouse(void) {
    union REGS regs;
    regs.x.ax = 0x02;
    int86(0x33, &regs, &regs);
}

void readMouse(MouseState *mouse) {
    union REGS regs;
    regs.x.ax = 0x03;
    int86(0x33, &regs, &regs);

    mouse->buttons = regs.x.bx;
    mouse->x = regs.x.cx / 2; /* Scale to 320x200 */
    mouse->y = regs.x.dx;
}

/*============================================================================
 * SOUND FUNCTIONS
 *===========================================================================*/

void playBeep(int freq, int duration) {
    /* PC Speaker via port 0x61 and timer */
    /* Calculate timer divisor: 1193180 Hz / desired frequency */
    unsigned int divisor = 1193180 / freq;

    /* Set PIT to square wave mode */
    outp(0x43, 0xB6);

    /* Send divisor to timer channel 2 */
    outp(0x42, (unsigned char)(divisor & 0xFF));
    outp(0x42, (unsigned char)(divisor >> 8));

    /* Enable speaker (bits 0 and 1) */
    unsigned char tmp = inp(0x61);
    if ((tmp & 3) != 3) {
        outp(0x61, tmp | 3);
    }

    delay_ms(duration);

    /* Disable speaker */
    outp(0x61, tmp & 0xFC);
}

void playBonkSound(void) {
    playBeep(1000, 50);
}

void playSpecialSound(void) {
    playBeep(1500, 80);
    delay_ms(20);
    playBeep(2000, 80);
}

void playMenuJingle(void) {
    /* Short upbeat melody - non-blocking */
    int notes[] = {523, 659, 784, 1047}; /* C E G C */
    int i;
    for (i = 0; i < 4; i++) {
        playBeep(notes[i], 100);
        delay_ms(50);
    }
}

/*============================================================================
 * CHARACTER DRAWING FUNCTIONS
 *===========================================================================*/

void drawChicken(int x, int y, int withFlames) {
    int cx = x + TARGET_SIZE/2;
    int cy = y + TARGET_SIZE/2;

    /* Body */
    fillCircle(cx, cy + 2, TARGET_SIZE/3, YELLOW);

    /* Head */
    fillCircle(cx, cy - TARGET_SIZE/4, TARGET_SIZE/4, YELLOW);

    /* Eyes */
    setPixel(cx - 3, cy - TARGET_SIZE/4 - 2, BLACK);
    setPixel(cx + 3, cy - TARGET_SIZE/4 - 2, BLACK);

    /* Beak */
    fillCircle(cx, cy - TARGET_SIZE/4, 2, ORANGE);

    /* Comb */
    fillCircle(cx, cy - TARGET_SIZE/4 - 5, 2, RED);

    if (withFlames) {
        drawFlame(cx, cy + TARGET_SIZE/2);
    }
}

void drawBunny(int x, int y, int withFlames) {
    int cx = x + TARGET_SIZE/2;
    int cy = y + TARGET_SIZE/2;

    /* Body */
    fillCircle(cx, cy + 2, TARGET_SIZE/3, WHITE);

    /* Head */
    fillCircle(cx, cy - 4, TARGET_SIZE/4, WHITE);

    /* Ears */
    fillRect(cx - 4, cy - 12, cx - 2, cy - 4, WHITE);
    fillRect(cx + 2, cy - 12, cx + 4, cy - 4, WHITE);

    /* Eyes */
    setPixel(cx - 3, cy - 5, BLACK);
    setPixel(cx + 3, cy - 5, BLACK);

    /* Nose */
    setPixel(cx, cy - 2, MAGENTA);
}

void drawDragon(int x, int y, int withFlames) {
    int cx = x + TARGET_SIZE/2;
    int cy = y + TARGET_SIZE/2;

    /* Body */
    fillCircle(cx, cy + 3, TARGET_SIZE/3, DARK_GRAY);

    /* Head */
    fillCircle(cx, cy - 4, TARGET_SIZE/4, DARK_GRAY);

    /* Eyes (GREEN for Night Fury!) */
    fillCircle(cx - 3, cy - 5, 2, GREEN);
    fillCircle(cx + 3, cy - 5, 2, GREEN);
    setPixel(cx - 3, cy - 5, BLACK);
    setPixel(cx + 3, cy - 5, BLACK);

    /* Wings */
    drawLine(cx - 8, cy, cx - 14, cy + 5, DARK_GRAY);
    drawLine(cx + 8, cy, cx + 14, cy + 5, DARK_GRAY);

    if (withFlames) {
        /* Plasma blast! */
        fillCircle(cx, cy - 8, 2, CYAN);
    }
}

void drawFlame(int x, int y) {
    fillCircle(x, y, 4, RED);
    fillCircle(x, y, 2, YELLOW);
}

void drawEgg(int x, int y) {
    fillCircle(x, y, SPECIAL_SIZE/2, WHITE);
    setPixel(x - 2, y - 2, MAGENTA);
    setPixel(x + 2, y + 2, CYAN);
}

void drawFish(int x, int y) {
    fillCircle(x, y, SPECIAL_SIZE/2, CYAN);
    drawLine(x + 4, y, x + 8, y - 3, CYAN);
    drawLine(x + 4, y, x + 8, y + 3, CYAN);
}

/*============================================================================
 * GAME LOGIC
 *===========================================================================*/

unsigned long getTickCount(void) {
    /* Read BIOS timer tick count (18.2 Hz) */
#ifdef __DJGPP__
    return _farpeekl(_dos_ds, 0x046C);
#else
    unsigned long far *timer = (unsigned long far *)0x0000046CL;
    return *timer;
#endif
}

void delay_ms(int ms) {
    unsigned long start = getTickCount();
    unsigned long ticks = (ms * 182) / 10000; /* Convert ms to ticks */
    while (getTickCount() - start < ticks);
}

void initGame(GameState *game) {
    int i;
    game->score = 0;
    game->specialScore = 0;
    game->timeLeft = GAME_TIME;
    game->bonkStreak = 0;
    game->frenzyMode = 0;

    for (i = 0; i < MAX_TARGETS; i++) {
        game->targets[i].active = 0;
    }

    game->special.active = 0;
    game->lastSpawn = getTickCount();
    game->lastSecond = getTickCount();
    game->startTime = getTickCount();
}

void spawnTarget(GameState *game) {
    int i;
    unsigned long now = getTickCount();

    /* Find inactive target slot */
    for (i = 0; i < MAX_TARGETS; i++) {
        if (!game->targets[i].active) {
            game->targets[i].x = 20 + rand() % (SCREEN_WIDTH - TARGET_SIZE - 40);
            game->targets[i].y = 40 + rand() % (SCREEN_HEIGHT - TARGET_SIZE - 60);
            game->targets[i].active = 1;
            game->targets[i].spawnTime = now;
            game->targets[i].displayTime = (game->difficulty == DIFF_EASY) ? 18 : 12; /* Ticks */
            game->lastSpawn = now;
            break;
        }
    }

    /* Spawn special item occasionally */
    if (!game->special.active && game->score > 3 && (rand() % 100) < 10) {
        game->special.x = 20 + rand() % (SCREEN_WIDTH - SPECIAL_SIZE - 40);
        game->special.y = 40 + rand() % (SCREEN_HEIGHT - SPECIAL_SIZE - 60);
        game->special.active = 1;
    }
}

void checkHit(GameState *game, int mx, int my) {
    int i;

    /* Check regular targets */
    for (i = 0; i < MAX_TARGETS; i++) {
        if (game->targets[i].active) {
            int x = game->targets[i].x;
            int y = game->targets[i].y;

            if (mx >= x && mx <= x + TARGET_SIZE &&
                my >= y && my <= y + TARGET_SIZE) {
                game->score++;
                game->bonkStreak++;
                game->targets[i].active = 0;

                /* Clear target area */
                fillRect(x - 5, y - 5, x + TARGET_SIZE + 5, y + TARGET_SIZE + 5, BLACK);

                playBonkSound();

                /* Frenzy mode in hard */
                if (game->difficulty == DIFF_HARD && game->bonkStreak >= 5) {
                    game->frenzyMode = 1;
                }

                return;
            }
        }
    }

    /* Check special item - larger hitbox for easier clicking */
    if (game->special.active) {
        int x = game->special.x;
        int y = game->special.y;
        int hitSize = SPECIAL_SIZE + 8; /* Bigger hitbox */

        if (mx >= x - hitSize/2 && mx <= x + hitSize/2 &&
            my >= y - hitSize/2 && my <= y + hitSize/2) {
            game->specialScore++;
            game->special.active = 0;
            fillRect(x - 12, y - 12, x + SPECIAL_SIZE + 12, y + SPECIAL_SIZE + 12, BLACK);
            playSpecialSound();
        }
    }
}

void updateGame(GameState *game) {
    unsigned long now = getTickCount();
    int i;

    /* Update timer */
    if (now - game->lastSecond >= 18) { /* ~1 second in ticks */
        game->timeLeft--;
        game->lastSecond = now;
    }

    /* Spawn new targets */
    int spawnDelay = (game->difficulty == DIFF_EASY) ? 15 : 10;
    if (now - game->lastSpawn >= spawnDelay) {
        spawnTarget(game);
    }

    /* Remove expired targets */
    for (i = 0; i < MAX_TARGETS; i++) {
        if (game->targets[i].active) {
            if (now - game->targets[i].spawnTime >= game->targets[i].displayTime) {
                int x = game->targets[i].x;
                int y = game->targets[i].y;
                fillRect(x - 5, y - 5, x + TARGET_SIZE + 5, y + TARGET_SIZE + 5, BLACK);
                game->targets[i].active = 0;
                game->bonkStreak = 0; /* Reset streak on miss */
            }
        }
    }
}

void drawGameScreen(GameState *game) {
    int i;
    char buffer[40];

    /* Clear HUD area */
    fillRect(0, 0, SCREEN_WIDTH, 15, BLACK);

    /* Draw score */
    sprintf(buffer, "SCORE: %d", game->score);
    drawText(10, 5, buffer, YELLOW);

    sprintf(buffer, "TIME: %d", game->timeLeft);
    drawText(240, 5, buffer, WHITE);

    /* Draw active targets */
    for (i = 0; i < MAX_TARGETS; i++) {
        if (game->targets[i].active) {
            int x = game->targets[i].x;
            int y = game->targets[i].y;

            switch (game->variant) {
                case VARIANT_CHICKEN:
                    drawChicken(x, y, game->frenzyMode);
                    break;
                case VARIANT_BUNNY:
                    drawBunny(x, y, game->frenzyMode);
                    break;
                case VARIANT_DRAGON:
                    drawDragon(x, y, game->frenzyMode);
                    break;
            }
        }
    }

    /* Draw special item */
    if (game->special.active) {
        switch (game->variant) {
            case VARIANT_CHICKEN:
                drawFlame(game->special.x, game->special.y);
                break;
            case VARIANT_BUNNY:
                drawEgg(game->special.x, game->special.y);
                break;
            case VARIANT_DRAGON:
                drawFish(game->special.x, game->special.y);
                break;
        }
    }
}

void drawTitleScreen(void) {
    clearScreen(BLACK);

    /* BONK ASCII art - BIG letters */
    drawText(40, 30, "######   ######  ###    ## ##   ##", YELLOW);
    drawText(40, 40, "##   ## ##    ## ####   ## ##  ##", YELLOW);
    drawText(40, 50, "######  ##    ## ## ##  ## #####", YELLOW);
    drawText(40, 60, "##   ## ##    ## ##  ## ## ##  ##", YELLOW);
    drawText(40, 70, "######   ######  ##   #### ##   ##", YELLOW);

    drawText(70, 90, "*** DOS EDITION v9.5 ***", CYAN);
    drawText(50, 110, "Whack-a-Mole meets DOS gaming!", WHITE);

    /* Attribution */
    drawText(45, 140, "Created by Trent Von Holten 2025", LGREEN);
    drawText(75, 150, "VonHoltenCodes", LMAGENTA);

    drawText(55, 175, "Press any key to start...", GRAY);
}

GameVariant showMenu(void) {
    /* Returns selected variant (0=chicken, 1=bunny, 2=dragon) */
    clearScreen(BLACK);
    drawText(100, 60, "Choose your game:", YELLOW);
    drawText(120, 80, "1 - CHICKEN", GREEN);
    drawText(120, 90, "2 - BUNNY", MAGENTA);
    drawText(120, 100, "3 - DRAGON", CYAN);

    while (1) {
        if (kbhit()) {
            int ch = getch();
            if (ch == '1') {
                playBeep(600, 100); /* Chicken cluck */
                return VARIANT_CHICKEN;
            }
            if (ch == '2') {
                playBeep(1200, 100); /* Bunny hop */
                return VARIANT_BUNNY;
            }
            if (ch == '3') {
                playBeep(400, 100); /* Dragon roar */
                return VARIANT_DRAGON;
            }
        }
    }
}

/*============================================================================
 * MAIN PROGRAM
 *===========================================================================*/

int main(void) {
    GameState game;
    MouseState mouse;
    int oldButtons = 0;
    int playAgain = 1;

    /* Initialize */
    srand(time(NULL));

#ifdef __DJGPP__
    /* Enable near pointers for direct VGA memory access */
    if (!__djgpp_nearptr_enable()) {
        printf("ERROR: Could not enable near pointers!\n");
        return 1;
    }
#endif

    /* Check mouse */
    if (!initMouse()) {
        printf("ERROR: Mouse driver not found!\n");
        printf("Please load MOUSE.COM first.\n");
        return 1;
    }

    /* Set VGA mode once */
    setVideoMode(0x13); /* VGA Mode 13h: 320x200, 256 colors */

    /* Title screen - only show once */
    drawTitleScreen();
    playMenuJingle(); /* Welcome music */
    getch();

    /* Main game loop - allows replay */
    while (playAgain) {
        /* Menu */
        game.variant = showMenu();
        clearScreen(BLACK);
        drawText(80, 90, "Difficulty: E)asy or H)ard?", WHITE);

        while (1) {
            int ch = getch();
            if (ch == 'e' || ch == 'E') {
                game.difficulty = DIFF_EASY;
                playBeep(800, 100); /* Selection beep */
                break;
            }
            if (ch == 'h' || ch == 'H') {
                game.difficulty = DIFF_HARD;
                playBeep(800, 100); /* Selection beep */
                break;
            }
        }

        /* Game starting indicator */
        clearScreen(BLACK);
        drawText(90, 95, "GET READY!", YELLOW);
        playMenuJingle(); /* Starting jingle */
        delay_ms(500);

        /* Initialize game */
        initGame(&game);
        clearScreen(BLACK);
        showMouse();

    /* Main game loop */
    while (game.timeLeft > 0 && !kbhit()) {
        /* Update game state */
        updateGame(&game);

        /* Draw */
        drawGameScreen(&game);

        /* Check mouse clicks */
        readMouse(&mouse);
        if ((mouse.buttons & 1) && !(oldButtons & 1)) {
            /* Left button pressed */
            checkHit(&game, mouse.x, mouse.y);
        }
        oldButtons = mouse.buttons;

        /* Small delay to prevent CPU spinning */
        delay_ms(10);
    }

        /* Game over */
        hideMouse();
        clearScreen(BLACK);

        char buffer[60];
        drawText(90, 60, "GAME OVER!", YELLOW);

        /* Game over sound - descending tones */
        playBeep(800, 200);
        playBeep(600, 200);
        playBeep(400, 300);

        sprintf(buffer, "Final Score: %d", game.score);
        drawText(80, 85, buffer, WHITE);
        sprintf(buffer, "Specials: %d", game.specialScore);
        drawText(85, 100, buffer, CYAN);

        /* Attribution on game over */
        drawText(55, 130, "Created by Trent Von Holten", LGREEN);
        drawText(75, 140, "VonHoltenCodes 2025", LMAGENTA);

        drawText(50, 170, "Play again? Y)es or N)o", WHITE);

        /* Wait for replay choice */
        while (1) {
            int ch = getch();
            if (ch == 'y' || ch == 'Y') {
                playAgain = 1;
                break;
            }
            if (ch == 'n' || ch == 'N' || ch == 27) { /* ESC */
                playAgain = 0;
                break;
            }
        }
    } /* End of while(playAgain) */

    /* Exit - Return to text mode */
    setVideoMode(0x03);

    printf("\n");
    printf("================================\n");
    printf("  BONK v9.5 - DOS EDITION\n");
    printf("================================\n");
    printf("\n");
    printf("  Final Score: %d\n", game.score);
    printf("  Specials:    %d\n", game.specialScore);
    printf("\n");
    printf("  Created by Trent Von Holten\n");
    printf("  VonHoltenCodes 2025\n");
    printf("\n");
    printf("  * BUILT IN 1995 *\n");
    printf("  * RUNNING IN 2025 *\n");
    printf("\n");
    printf("  Thanks for playing!\n");
    printf("\n");

#ifdef __DJGPP__
    __djgpp_nearptr_disable();
#endif

    return 0;
}

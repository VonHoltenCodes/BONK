/*
 * ██████   ██████  ███    ██ ██   ██     ██    ██  █████
 * ██   ██ ██    ██ ████   ██ ██  ██      ██    ██ ██   ██
 * ██████  ██    ██ ██ ██  ██ █████       ██    ██  █████
 * ██   ██ ██    ██ ██  ██ ██ ██  ██       ██  ██  ██   ██
 * ██████   ██████  ██   ████ ██   ██       ████    █████
 *
 * FLOPPY EDITION - Fits on 1.44MB 3.5" Disk!
 *
 * Hardware: Teensy 4.1 + ILI9488 3.5" TFT + XPT2046 Touch
 * Code Size: < 100 KB
 * By: VonHoltenCodes (2025)
 *
 * License: Educational/Recreational Use Only
 */

#include <SPI.h>
#include <ILI9488_t3.h>
#include "sprites.h"

// ==================== PIN DEFINITIONS ====================
// TFT Display Pins (Teensy 4.1 → ILI9488)
#define TFT_CS     20
#define TFT_DC     21
#define TFT_RST    22
#define TFT_MOSI   11
#define TFT_SCK    13
#define TFT_MISO   12

// XPT2046 Touch Pins (Teensy 4.1 → XPT2046)
#define TOUCH_CS   23
#define TOUCH_IRQ  24

// PWM Audio Pin (optional beeper)
#define AUDIO_PIN  5

// ==================== DISPLAY SETUP ====================
ILI9488_t3 tft = ILI9488_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

// Colors (16-bit RGB565)
#define BLACK      0x0000
#define WHITE      0xFFFF
#define RED        0xF800
#define GREEN      0x07E0
#define BLUE       0x001F
#define YELLOW     0xFFE0
#define CYAN       0x07FF
#define MAGENTA    0xF81F
#define ORANGE     0xFD20
#define PURPLE     0x8010
#define DARK_GRAY  0x39C7
#define PLASMA_BLUE 0x03FF

// Screen dimensions
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 320

// ==================== GAME STATES ====================
enum GameState {
  LOAD_SCREEN,
  MENU,
  PLAYER_SELECT,
  MODE_SELECT,
  GAMEPLAY,
  RESULT_SCREEN
};

enum GameDifficulty {
  EASY,
  HARD
};

enum GameVariant {
  CHICKEN,
  BUNNY,
  DRAGON
};

// ==================== GAME VARIABLES ====================
GameState currentState = LOAD_SCREEN;
GameDifficulty difficulty = EASY;
GameVariant variant = CHICKEN;
bool twoPlayerMode = false;
unsigned long stateStartTime = 0;

// Scores
int p1Score = 0;
int p1SpecialScore = 0;
int p2Score = 0;
int p2SpecialScore = 0;
int currentScore = 0;
int currentSpecialScore = 0;

// Game timing
int timeLeft = 45;
unsigned long lastTimerUpdate = 0;
unsigned long lastTargetTime = 0;

// Target variables
int targetX, targetY;
int targetSize = 40;
bool targetVisible = false;
bool showFlameEffect = false;

// Special target
int specialX, specialY;
bool specialVisible = false;
int specialSize = 30;

// Progressive difficulty
int targetDuration = 1000;
int minTargetDuration = 400;
int speedIncrease = 20;

// Touch debounce
unsigned long lastTouchTime = 0;
const int touchDebounce = 150;

// Frenzy mode
int bonkStreak = 0;
bool frenzyMode = false;
unsigned long frenzyStartTime = 0;
const int frenzyDuration = 3000;

// Current player (for 2P mode)
int currentPlayer = 1;

// ==================== TOUCH FUNCTIONS ====================
struct TouchPoint {
  int x;
  int y;
  bool touched;
};

TouchPoint readTouch() {
  TouchPoint tp;
  tp.touched = false;

  if (digitalRead(TOUCH_IRQ) == LOW) {
    // XPT2046 touch detected
    digitalWrite(TOUCH_CS, LOW);

    // Read X coordinate
    SPI.transfer(0x90); // X position command
    delayMicroseconds(10);
    int x_raw = (SPI.transfer(0) << 8) | SPI.transfer(0x00);

    // Read Y coordinate
    SPI.transfer(0xD0); // Y position command
    delayMicroseconds(10);
    int y_raw = (SPI.transfer(0) << 8) | SPI.transfer(0x00);

    digitalWrite(TOUCH_CS, HIGH);

    // Map to screen coordinates (calibrated for ILI9488)
    tp.x = map(x_raw, 300, 3700, 0, SCREEN_WIDTH);
    tp.y = map(y_raw, 400, 3800, 0, SCREEN_HEIGHT);
    tp.x = constrain(tp.x, 0, SCREEN_WIDTH - 1);
    tp.y = constrain(tp.y, 0, SCREEN_HEIGHT - 1);
    tp.touched = true;
  }

  return tp;
}

// ==================== AUDIO FUNCTIONS ====================
void beep(int freq, int duration) {
  tone(AUDIO_PIN, freq, duration);
}

void playStartupSound() {
  beep(880, 100);
  delay(50);
  beep(1047, 100);
  delay(50);
  beep(1319, 150);
}

void playBonkSound() {
  beep(1000, 50);
}

void playSpecialSound() {
  beep(1500, 100);
}

// ==================== SPRITE DRAWING ====================
void drawSprite1Bit(int x, int y, const uint8_t* data, int w, int h, uint16_t color) {
  int byteIndex = 0;
  int bitIndex = 7;

  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      if (data[byteIndex] & (1 << bitIndex)) {
        tft.drawPixel(x + col, y + row, color);
      }

      bitIndex--;
      if (bitIndex < 0) {
        bitIndex = 7;
        byteIndex++;
      }
    }
  }
}

// UPGRADED geometric sprites with more detail!
void drawChicken(int x, int y, bool withFlames) {
  int cx = x + targetSize/2;
  int cy = y + targetSize/2;

  // Body - plump and round
  tft.fillCircle(cx, cy + 2, targetSize/3, YELLOW);

  // Head
  tft.fillCircle(cx, cy - targetSize/4, targetSize/4, YELLOW);

  // Beak - more prominent
  tft.fillTriangle(cx - 5, cy - targetSize/4, cx + 5, cy - targetSize/4,
                   cx, cy - targetSize/3 - 5, ORANGE);

  // Eyes - bigger and more expressive
  tft.fillCircle(cx - 5, cy - targetSize/4 - 2, 3, WHITE);
  tft.fillCircle(cx - 5, cy - targetSize/4 - 2, 2, BLACK);
  tft.fillCircle(cx + 5, cy - targetSize/4 - 2, 3, WHITE);
  tft.fillCircle(cx + 5, cy - targetSize/4 - 2, 2, BLACK);

  // Comb - more detailed with 3 parts
  tft.fillCircle(cx - 3, cy - targetSize/4 - 7, 3, RED);
  tft.fillCircle(cx, cy - targetSize/4 - 9, 4, RED);
  tft.fillCircle(cx + 3, cy - targetSize/4 - 7, 3, RED);

  // Wattle (chin thing)
  tft.fillCircle(cx - 2, cy - targetSize/4 + 3, 2, RED);
  tft.fillCircle(cx + 2, cy - targetSize/4 + 3, 2, RED);

  // Wings
  tft.fillCircle(cx - targetSize/4, cy, targetSize/6, ORANGE);
  tft.fillCircle(cx + targetSize/4, cy, targetSize/6, ORANGE);

  // Feet
  tft.drawLine(cx - 3, cy + targetSize/3, cx - 3, cy + targetSize/3 + 5, ORANGE);
  tft.drawLine(cx + 3, cy + targetSize/3, cx + 3, cy + targetSize/3 + 5, ORANGE);

  if (withFlames) {
    tft.fillCircle(cx, cy + targetSize/2, 5, RED);
    tft.fillCircle(cx, cy + targetSize/2, 3, ORANGE);
    tft.fillCircle(cx, cy + targetSize/2, 1, YELLOW);
  }
}

void drawBunny(int x, int y, bool withFlames) {
  int cx = x + targetSize/2;
  int cy = y + targetSize/2;

  // Body - fluffy and round
  tft.fillCircle(cx, cy + 3, targetSize/3, WHITE);

  // Head
  tft.fillCircle(cx, cy - 5, targetSize/4, WHITE);

  // Ears - bigger and more prominent
  tft.fillEllipse(cx - 6, cy - 15, 3, 10, WHITE);
  tft.fillEllipse(cx + 6, cy - 15, 3, 10, WHITE);
  // Pink inner ear
  tft.fillEllipse(cx - 6, cy - 15, 2, 6, MAGENTA);
  tft.fillEllipse(cx + 6, cy - 15, 2, 6, MAGENTA);

  // Eyes - bigger and cuter
  tft.fillCircle(cx - 5, cy - 6, 3, BLACK);
  tft.fillCircle(cx - 5, cy - 7, 1, WHITE); // Eye shine
  tft.fillCircle(cx + 5, cy - 6, 3, BLACK);
  tft.fillCircle(cx + 5, cy - 7, 1, WHITE); // Eye shine

  // Nose - pink triangle
  tft.fillTriangle(cx - 2, cy - 2, cx + 2, cy - 2, cx, cy, MAGENTA);

  // Whiskers
  tft.drawLine(cx - 8, cy - 3, cx - 12, cy - 4, BLACK);
  tft.drawLine(cx - 8, cy - 2, cx - 12, cy - 2, BLACK);
  tft.drawLine(cx + 8, cy - 3, cx + 12, cy - 4, BLACK);
  tft.drawLine(cx + 8, cy - 2, cx + 12, cy - 2, BLACK);

  // Fluffy tail
  tft.fillCircle(cx, cy + targetSize/3, targetSize/6, WHITE);

  // Feet - cute little paws
  tft.fillCircle(cx - 6, cy + targetSize/3 + 2, 3, MAGENTA);
  tft.fillCircle(cx + 6, cy + targetSize/3 + 2, 3, MAGENTA);

  if (withFlames) {
    // Sparkles instead of flames for bunny
    tft.fillCircle(cx - 8, cy + targetSize/2, 3, CYAN);
    tft.fillCircle(cx + 8, cy + targetSize/2, 3, MAGENTA);
    tft.fillCircle(cx, cy + targetSize/2 + 5, 3, YELLOW);
  }
}

void drawDragon(int x, int y, bool withFlames) {
  // Toothless the Night Fury - UPGRADED with more details!
  int cx = x + targetSize/2;
  int cy = y + targetSize/2;

  // Body - sleek dragon shape
  tft.fillCircle(cx, cy + 5, targetSize/2 - 5, DARK_GRAY);

  // Head - more angular and dragon-like
  tft.fillCircle(cx, cy - 8, targetSize/3, DARK_GRAY);

  // Snout
  tft.fillTriangle(cx - 8, cy - 8,
                   cx + 8, cy - 8,
                   cx, cy - 15, DARK_GRAY);

  // Ears/head fins - distinctive Night Fury ears
  tft.fillTriangle(cx - 10, cy - 10,
                   cx - 15, cy - 18,
                   cx - 5, cy - 15, DARK_GRAY);
  tft.fillTriangle(cx + 10, cy - 10,
                   cx + 15, cy - 18,
                   cx + 5, cy - 15, DARK_GRAY);

  // Side fins
  tft.fillTriangle(cx - 12, cy - 5,
                   cx - 18, cy - 8,
                   cx - 10, cy, DARK_GRAY);
  tft.fillTriangle(cx + 12, cy - 5,
                   cx + 18, cy - 8,
                   cx + 10, cy, DARK_GRAY);

  // GREEN EYES with BLACK PUPILS - bigger and more expressive
  // Left eye
  tft.fillCircle(cx - 6, cy - 8, 5, GREEN);
  tft.fillCircle(cx - 6, cy - 8, 3, BLACK);
  tft.fillCircle(cx - 7, cy - 9, 1, WHITE); // Eye shine

  // Right eye
  tft.fillCircle(cx + 6, cy - 8, 5, GREEN);
  tft.fillCircle(cx + 6, cy - 8, 3, BLACK);
  tft.fillCircle(cx + 5, cy - 9, 1, WHITE); // Eye shine

  // Wings - more detailed
  // Left wing
  tft.fillTriangle(cx - 8, cy,
                   cx - 20, cy + 5,
                   cx - 15, cy + 15, DARK_GRAY);
  tft.drawLine(cx - 8, cy, cx - 20, cy + 5, BLACK);
  tft.drawLine(cx - 20, cy + 5, cx - 15, cy + 15, BLACK);

  // Right wing
  tft.fillTriangle(cx + 8, cy,
                   cx + 20, cy + 5,
                   cx + 15, cy + 15, DARK_GRAY);
  tft.drawLine(cx + 8, cy, cx + 20, cy + 5, BLACK);
  tft.drawLine(cx + 20, cy + 5, cx + 15, cy + 15, BLACK);

  // Tail with distinctive tail fins
  tft.fillRect(cx - 3, cy + 12, 6, 12, DARK_GRAY);
  // Left tail fin
  tft.fillTriangle(cx - 3, cy + 20,
                   cx - 8, cy + 24,
                   cx - 3, cy + 28, DARK_GRAY);
  // Right tail fin (red like in the movie!)
  tft.fillTriangle(cx + 3, cy + 20,
                   cx + 8, cy + 24,
                   cx + 3, cy + 28, RED);

  // Pattern details
  tft.drawCircle(cx - 4, cy + 3, 2, BLACK);
  tft.drawCircle(cx + 4, cy + 3, 2, BLACK);

  if (withFlames) {
    // Blue plasma charging in mouth
    tft.fillCircle(cx, cy - 12, 3, PLASMA_BLUE);
    tft.fillCircle(cx, cy - 12, 2, CYAN);
    tft.fillCircle(cx, cy - 12, 1, WHITE);
  }
}

void drawTarget(int x, int y, bool withFlames) {
  switch (variant) {
    case CHICKEN:
      drawChicken(x, y, withFlames);
      break;
    case BUNNY:
      drawBunny(x, y, withFlames);
      break;
    case DRAGON:
      drawDragon(x, y, withFlames);
      break;
  }
}

void drawSpecialItem(int x, int y) {
  int cx = x + specialSize/2;
  int cy = y + specialSize/2;

  switch (variant) {
    case CHICKEN:
      // Flame - UPGRADED with more layers!
      tft.fillCircle(cx, cy, specialSize/2, RED);
      tft.fillCircle(cx, cy, specialSize/3, ORANGE);
      tft.fillCircle(cx, cy, specialSize/4, YELLOW);
      tft.fillCircle(cx, cy, specialSize/6, WHITE); // Hot center
      // Flame flickers
      tft.fillTriangle(cx - 5, cy - 8, cx, cy - 15, cx + 2, cy - 8, ORANGE);
      tft.fillTriangle(cx + 3, cy - 10, cx + 5, cy - 18, cx + 7, cy - 10, RED);
      break;
    case BUNNY:
      // Egg - UPGRADED with spots!
      tft.fillEllipse(cx, cy, specialSize/2, specialSize/2 + 4, WHITE);
      tft.drawEllipse(cx, cy, specialSize/2, specialSize/2 + 4, CYAN);
      // Easter egg spots
      tft.fillCircle(cx - 4, cy - 3, 2, MAGENTA);
      tft.fillCircle(cx + 5, cy + 2, 2, CYAN);
      tft.fillCircle(cx - 2, cy + 5, 2, GREEN);
      tft.fillCircle(cx + 3, cy - 5, 2, YELLOW);
      break;
    case DRAGON:
      // Fish - UPGRADED with scales and details!
      // Body - silver/gray
      tft.fillCircle(cx, cy, specialSize/2 - 2, CYAN);

      // Fish tail
      tft.fillTriangle(cx + specialSize/2 - 5, cy,
                       cx + specialSize/2 + 5, cy - 8,
                       cx + specialSize/2 + 5, cy + 8, CYAN);

      // Eye with shine
      tft.fillCircle(cx - 6, cy, 3, WHITE);
      tft.fillCircle(cx - 6, cy, 2, BLACK);
      tft.fillCircle(cx - 7, cy - 1, 1, WHITE); // Eye shine

      // Scales effect
      tft.drawCircle(cx + 2, cy, 3, WHITE);
      tft.drawCircle(cx + 6, cy - 3, 3, WHITE);
      tft.drawCircle(cx + 6, cy + 3, 3, WHITE);

      // Sparkles around fish (it's magical!)
      tft.fillCircle(cx - specialSize/2, cy - specialSize/2, 2, BLUE);
      tft.fillCircle(cx + specialSize/2, cy - specialSize/2, 2, BLUE);
      tft.fillCircle(cx, cy + specialSize/2, 2, CYAN);
      break;
  }
}

void clearArea(int x, int y, int size) {
  tft.fillRect(x - 10, y - 10, size + 20, size + 20, BLACK);
}

// ==================== SCREEN HANDLERS ====================
void drawLoadScreen() {
  tft.fillScreen(BLACK);
  tft.setTextColor(CYAN);
  tft.setTextSize(4);
  tft.setCursor(80, 100);
  tft.print("BONK v9");

  tft.setTextSize(2);
  tft.setCursor(100, 150);
  tft.setTextColor(WHITE);
  tft.print("FLOPPY EDITION");

  tft.setTextSize(1);
  tft.setCursor(130, 200);
  tft.setTextColor(GREEN);
  tft.print("* 1.44 MB LEGEND *");

  // ASCII floppy art
  tft.setTextColor(MAGENTA);
  tft.setCursor(180, 250);
  tft.print("[___]");
  tft.setCursor(180, 260);
  tft.print("|   |");
  tft.setCursor(180, 270);
  tft.print("|___|");
}

void drawMenu() {
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW);
  tft.setTextSize(3);
  tft.setCursor(100, 30);
  tft.print("CHOOSE GAME");

  // Game variant buttons
  tft.fillRect(50, 90, 120, 60, RED);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(60, 110);
  tft.print("CHICKEN");

  tft.fillRect(190, 90, 120, 60, MAGENTA);
  tft.setCursor(210, 110);
  tft.print("BUNNY");

  tft.fillRect(330, 90, 120, 60, DARK_GRAY);
  tft.setCursor(340, 110);
  tft.print("DRAGON");

  // Continue button
  tft.fillRect(150, 200, 180, 60, GREEN);
  tft.setTextSize(3);
  tft.setCursor(165, 220);
  tft.print("START!");
}

void drawPlayerSelect() {
  tft.fillScreen(BLACK);
  tft.setTextColor(CYAN);
  tft.setTextSize(3);
  tft.setCursor(80, 40);
  tft.print("PLAYERS?");

  // 1P button
  tft.fillRect(70, 120, 150, 80, BLUE);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.setCursor(110, 145);
  tft.print("1P");

  // 2P button
  tft.fillRect(260, 120, 150, 80, GREEN);
  tft.setCursor(300, 145);
  tft.print("2P");
}

void drawModeSelect() {
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW);
  tft.setTextSize(3);
  tft.setCursor(60, 40);
  if (twoPlayerMode && currentPlayer == 2) {
    tft.print("PLAYER 2:");
  } else {
    tft.print("DIFFICULTY");
  }

  // Easy button
  tft.fillRect(50, 120, 180, 80, GREEN);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(90, 150);
  tft.print("EASY");

  // Hard button
  tft.fillRect(250, 120, 180, 80, RED);
  tft.setCursor(280, 150);
  tft.print("HARD");
}

void drawGameScreen() {
  tft.fillScreen(BLACK);

  // Score area
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);

  if (variant == CHICKEN) {
    tft.print("BONKS: ");
  } else if (variant == BUNNY) {
    tft.print("HOPS: ");
  } else {
    tft.print("RIDES: ");
  }
  tft.print(currentScore);

  // Special score
  tft.setCursor(10, 35);
  tft.setTextColor(CYAN);
  if (variant == CHICKEN) {
    tft.print("FLAMES: ");
  } else if (variant == BUNNY) {
    tft.print("EGGS: ");
  } else {
    tft.print("FISH: ");
  }
  tft.print(currentSpecialScore);

  // Timer
  tft.fillRect(SCREEN_WIDTH - 120, 10, 110, 40, BLUE);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(SCREEN_WIDTH - 100, 18);
  tft.print(timeLeft);
  tft.print("s");

  // Player indicator (2P mode)
  if (twoPlayerMode) {
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(SCREEN_WIDTH - 120, 55);
    tft.print("P");
    tft.print(currentPlayer);
  }
}

void drawResultScreen() {
  tft.fillScreen(BLACK);
  tft.setTextColor(YELLOW);
  tft.setTextSize(3);
  tft.setCursor(120, 30);
  tft.print("RESULTS!");

  if (twoPlayerMode) {
    // Player 1
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.setCursor(50, 100);
    tft.print("PLAYER 1:");
    tft.setCursor(50, 125);
    tft.setTextColor(WHITE);
    tft.print("Score: ");
    tft.print(p1Score);
    tft.setCursor(50, 150);
    tft.print("Special: ");
    tft.print(p1SpecialScore);

    // Player 2
    tft.setTextColor(GREEN);
    tft.setTextSize(2);
    tft.setCursor(270, 100);
    tft.print("PLAYER 2:");
    tft.setCursor(270, 125);
    tft.setTextColor(WHITE);
    tft.print("Score: ");
    tft.print(p2Score);
    tft.setCursor(270, 150);
    tft.print("Special: ");
    tft.print(p2SpecialScore);

    // Winner
    tft.setTextSize(3);
    tft.setCursor(120, 200);
    if (p1Score + p1SpecialScore > p2Score + p2SpecialScore) {
      tft.setTextColor(CYAN);
      tft.print("P1 WINS!");
    } else if (p2Score + p2SpecialScore > p1Score + p1SpecialScore) {
      tft.setTextColor(GREEN);
      tft.print("P2 WINS!");
    } else {
      tft.setTextColor(MAGENTA);
      tft.print("TIE!");
    }
  } else {
    // Single player
    tft.setTextColor(CYAN);
    tft.setTextSize(3);
    tft.setCursor(100, 120);
    tft.print("SCORE: ");
    tft.print(p1Score);

    tft.setCursor(80, 160);
    tft.setTextColor(YELLOW);
    tft.print("SPECIAL: ");
    tft.print(p1SpecialScore);
  }

  // Restart prompt
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.setCursor(100, 280);
  tft.print("TAP TO RESTART");
}

// ==================== GAME LOGIC ====================
bool isTapInArea(int tx, int ty, int x, int y, int size) {
  int margin = 30;
  return (tx >= x - margin && tx <= x + size + margin &&
          ty >= y - margin && ty <= y + size + margin);
}

void spawnTarget() {
  targetX = random(40, SCREEN_WIDTH - targetSize - 40);
  targetY = random(70, SCREEN_HEIGHT - targetSize - 20);
  targetVisible = true;
  lastTargetTime = millis();

  drawTarget(targetX, targetY, showFlameEffect);
}

void spawnSpecial() {
  specialX = random(40, SCREEN_WIDTH - specialSize - 40);
  specialY = random(70, SCREEN_HEIGHT - specialSize - 20);
  specialVisible = true;

  drawSpecialItem(specialX, specialY);
}

void updateGameplay() {
  unsigned long currentTime = millis();

  // Update timer
  if (currentTime - lastTimerUpdate >= 1000) {
    lastTimerUpdate = currentTime;
    timeLeft--;

    // Redraw timer
    tft.fillRect(SCREEN_WIDTH - 120, 10, 110, 40, BLUE);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.setCursor(SCREEN_WIDTH - 100, 18);
    tft.print(timeLeft);
    tft.print("s");

    if (timeLeft <= 0) {
      // Game over
      if (twoPlayerMode && currentPlayer == 1) {
        // Switch to player 2
        p1Score = currentScore;
        p1SpecialScore = currentSpecialScore;
        currentPlayer = 2;
        currentState = MODE_SELECT;
        stateStartTime = currentTime;
        currentScore = 0;
        currentSpecialScore = 0;
        timeLeft = 45;
        targetVisible = false;
        specialVisible = false;
        bonkStreak = 0;
        frenzyMode = false;
      } else {
        // End game
        if (twoPlayerMode) {
          p2Score = currentScore;
          p2SpecialScore = currentSpecialScore;
        } else {
          p1Score = currentScore;
          p1SpecialScore = currentSpecialScore;
        }
        currentState = RESULT_SCREEN;
        stateStartTime = currentTime;
        targetVisible = false;
        specialVisible = false;
      }
      drawResultScreen();
      return;
    }
  }

  // Spawn targets
  if (!targetVisible && currentTime - lastTargetTime >= targetDuration) {
    spawnTarget();
  }

  // Hide target after duration
  if (targetVisible && currentTime - lastTargetTime >= targetDuration) {
    clearArea(targetX, targetY, targetSize);
    targetVisible = false;
    bonkStreak = 0; // Reset streak if missed
    frenzyMode = false;
  }

  // Spawn special items occasionally
  if (!specialVisible && random(0, 100) < 5 && currentScore > 5) {
    spawnSpecial();
  }

  // Frenzy mode timeout
  if (frenzyMode && currentTime - frenzyStartTime >= frenzyDuration) {
    frenzyMode = false;
    showFlameEffect = false;
  }
}

void handleGameplayTouch(int tx, int ty) {
  // Check target hit
  if (targetVisible && isTapInArea(tx, ty, targetX, targetY, targetSize)) {
    currentScore++;
    playBonkSound();
    clearArea(targetX, targetY, targetSize);
    targetVisible = false;

    // Update score display
    tft.fillRect(10, 10, 220, 30, BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    if (variant == CHICKEN) {
      tft.print("BONKS: ");
    } else if (variant == BUNNY) {
      tft.print("HOPS: ");
    } else {
      tft.print("RIDES: ");
    }
    tft.print(currentScore);

    // Progressive difficulty
    if (difficulty == HARD) {
      bonkStreak++;
      if (bonkStreak >= 5 && !frenzyMode) {
        frenzyMode = true;
        frenzyStartTime = millis();
        showFlameEffect = true;
      }

      if (targetDuration > minTargetDuration) {
        targetDuration -= speedIncrease;
        if (targetDuration < minTargetDuration) {
          targetDuration = minTargetDuration;
        }
      }
    }
  }

  // Check special hit
  if (specialVisible && isTapInArea(tx, ty, specialX, specialY, specialSize)) {
    currentSpecialScore++;
    playSpecialSound();
    clearArea(specialX, specialY, specialSize);
    specialVisible = false;

    // Update special score
    tft.fillRect(10, 35, 220, 25, BLACK);
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.setCursor(10, 35);
    if (variant == CHICKEN) {
      tft.print("FLAMES: ");
    } else if (variant == BUNNY) {
      tft.print("EGGS: ");
    } else {
      tft.print("FISH: ");
    }
    tft.print(currentSpecialScore);
  }
}

// ==================== SETUP & LOOP ====================
void setup() {
  Serial.begin(115200);

  // Initialize SPI
  SPI.begin();

  // Initialize TFT
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(BLACK);

  // Initialize touch
  pinMode(TOUCH_CS, OUTPUT);
  digitalWrite(TOUCH_CS, HIGH);
  pinMode(TOUCH_IRQ, INPUT);

  // Initialize audio
  pinMode(AUDIO_PIN, OUTPUT);

  // Random seed
  randomSeed(analogRead(0));

  // Show load screen
  drawLoadScreen();
  playStartupSound();
  stateStartTime = millis();

  Serial.println("BONK v9 FLOPPY EDITION - Ready!");
}

void loop() {
  unsigned long currentTime = millis();

  // Read touch
  TouchPoint touch = readTouch();

  // Handle state machine
  switch (currentState) {
    case LOAD_SCREEN:
      if (currentTime - stateStartTime >= 3000) {
        currentState = MENU;
        drawMenu();
        stateStartTime = currentTime;
      }
      break;

    case MENU:
      if (touch.touched && currentTime - lastTouchTime >= touchDebounce) {
        lastTouchTime = currentTime;

        // Check variant selection
        if (touch.y >= 90 && touch.y <= 150) {
          if (touch.x >= 50 && touch.x <= 170) {
            variant = CHICKEN;
          } else if (touch.x >= 190 && touch.x <= 310) {
            variant = BUNNY;
          } else if (touch.x >= 330 && touch.x <= 450) {
            variant = DRAGON;
          }
        }

        // Check start button
        if (touch.x >= 150 && touch.x <= 330 && touch.y >= 200 && touch.y <= 260) {
          currentState = PLAYER_SELECT;
          drawPlayerSelect();
          stateStartTime = currentTime;
        }
      }
      break;

    case PLAYER_SELECT:
      if (touch.touched && currentTime - lastTouchTime >= touchDebounce) {
        lastTouchTime = currentTime;

        if (touch.x >= 70 && touch.x <= 220 && touch.y >= 120 && touch.y <= 200) {
          twoPlayerMode = false;
          currentState = MODE_SELECT;
          drawModeSelect();
        } else if (touch.x >= 260 && touch.x <= 410 && touch.y >= 120 && touch.y <= 200) {
          twoPlayerMode = true;
          currentState = MODE_SELECT;
          drawModeSelect();
        }
      }
      break;

    case MODE_SELECT:
      if (touch.touched && currentTime - lastTouchTime >= touchDebounce) {
        lastTouchTime = currentTime;

        if (touch.x >= 50 && touch.x <= 230 && touch.y >= 120 && touch.y <= 200) {
          difficulty = EASY;
          targetDuration = 1000;
          currentState = GAMEPLAY;
          drawGameScreen();
          timeLeft = 45;
          lastTimerUpdate = currentTime;
          lastTargetTime = currentTime;
        } else if (touch.x >= 250 && touch.x <= 430 && touch.y >= 120 && touch.y <= 200) {
          difficulty = HARD;
          targetDuration = 800;
          currentState = GAMEPLAY;
          drawGameScreen();
          timeLeft = 45;
          lastTimerUpdate = currentTime;
          lastTargetTime = currentTime;
        }
      }
      break;

    case GAMEPLAY:
      updateGameplay();

      if (touch.touched && currentTime - lastTouchTime >= touchDebounce) {
        lastTouchTime = currentTime;
        handleGameplayTouch(touch.x, touch.y);
      }
      break;

    case RESULT_SCREEN:
      if (touch.touched && currentTime - lastTouchTime >= touchDebounce) {
        lastTouchTime = currentTime;

        // Reset game
        p1Score = 0;
        p1SpecialScore = 0;
        p2Score = 0;
        p2SpecialScore = 0;
        currentScore = 0;
        currentSpecialScore = 0;
        currentPlayer = 1;
        bonkStreak = 0;
        frenzyMode = false;
        targetVisible = false;
        specialVisible = false;

        currentState = MENU;
        drawMenu();
      }
      break;
  }

  delay(10); // Small delay to prevent excessive CPU usage
}

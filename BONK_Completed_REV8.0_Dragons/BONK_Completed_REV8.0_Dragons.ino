#include <SPI.h>
#include <ILI9488_t3.h>
#include <Wire.h>
#include <FT6236G.h>

// Define pins for the TFT display
#define TFT_CS     10
#define TFT_DC     9
#define TFT_RST    8

// Touch panel settings
#define CTP_INT    5
#define CTP_RST    7
#define CTP_ADDR   0x38

// Create display instance
ILI9488_t3 tft = ILI9488_t3(TFT_CS, TFT_DC, TFT_RST);

// Create touch instance
FT6236G ts;

// Define colors (inverted from original)
#define BLACK     0xFFFF  // Was 0x0000 (now white)
#define WHITE     0x0000  // Was 0xFFFF (now black)
#define RED       0x07FF  // Was 0xF800 (~F800 = 07FF, inverted red)
#define GREEN     0xF81F  // Was 0x07E0 (~07E0 = F81F, inverted green)
#define BLUE      0xFFE0  // Was 0x001F (~001F = FFE0, inverted blue)
#define YELLOW    0x001F  // Was 0xFFE0 (~FFE0 = 001F, inverted yellow)
#define CYAN      0xF800  // Was 0x07FF (~07FF = F800, inverted cyan)
#define MAGENTA   0x07E0  // Was 0xF81F (~F81F = 07E0, inverted magenta)
#define ORANGE    0x02DF  // Was 0xFD20 (~FD20 = 02DF, inverted orange)
#define PURPLE    0x7FEF  // Was 0x8010 (~8010 = 7FEF, inverted purple)
#define DARK_ORANGE 0x019F  // Darker orange for Bingo
#define DARK_GRAY 0xC638  // Dark gray for Toothless
#define LIGHT_BLUE 0xFFB0  // Light blue for Light Fury
#define PLASMA_BLUE 0xFFC0  // Bright blue for plasma blasts

// Screen dimensions
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 320

// Game states
enum GameState {
  LOAD_SCREEN,
  GAME_SELECT,       // Choose game variant (Chicken/Bunny/Patriots)
  PLAYER_SELECT,     // Choose 1P or 2P
  MODE_SELECT_P1,    // Difficulty for Player 1
  PRESS_START_P1,    // Player 1 press start screen
  GAMEPLAY_P1,       // Player 1 gameplay
  MODE_SELECT_P2,    // Difficulty for Player 2 (if in 2P mode)
  PRESS_START_P2,    // Player 2 press start screen (if in 2P mode)
  GAMEPLAY_P2,       // Player 2 gameplay (if in 2P mode)
  RESULT_SCREEN      // Final results (comparing P1 and P2 in 2P mode)
};

// Game difficulty modes
enum GameDifficulty {
  EASY,
  SHRED_GNAR
};

// Game variants
enum GameVariant {
  BONK_CHICKEN,
  BONK_BUNNY,
  BONK_PATRIOTS,
  BONK_BLUEY,
  BONK_DRAGONS  // Nolan's How to Train Your Dragon variant!
};

// Game variables
GameState currentState = LOAD_SCREEN;
GameDifficulty gameDifficulty = EASY;
GameVariant gameVariant = BONK_CHICKEN;
bool twoPlayerMode = false;
unsigned long stateStartTime = 0;
unsigned long loadScreenPhase = 0;

// Player scores and states
int p1Score = 0;
int p1SpecialScore = 0;
int p2Score = 0;
int p2SpecialScore = 0;
GameDifficulty p1Difficulty = EASY;
GameDifficulty p2Difficulty = EASY;

// Current player tracking (used in gameplay)
int score = 0;
int level2Score = 0;
GameDifficulty currentPlayerDifficulty = EASY;

// Game timing variables
int timeLeft = 45;
unsigned long lastChickenTime = 0;
unsigned long lastTimerUpdate = 0;

// Game object variables
int chickenX, chickenY;
const int initialChickenSize = 40;
int currentChickenSize = initialChickenSize;
bool chickenVisible = false;
unsigned long lastTapTime = 0;
const int debounceDelay = 200;
bool showFeedback = false;
unsigned long feedbackTime = 0;

// Text display variables
bool showFrenzyText = false;
unsigned long frenzyTextTime = 0;

// Variables for chicken tap timing
int lastChickenX = 0;
int lastChickenY = 0;
unsigned long chickenDisappearTime = 0;
unsigned long afterDisappearGracePeriod = 800;

// Variables for second chicken (level 2)
int specialChickenX, specialChickenY;
bool specialChickenVisible = false;
unsigned long lastSpecialChickenTime = 0;
int specialChickenSize = 30;
int lastSpecialChickenX = 0;
int lastSpecialChickenY = 0;
unsigned long specialChickenDisappearTime = 0;
bool showSpecialChicken = false;

// Progressive difficulty variables
int initialChickenDuration = 1000;
int currentChickenDuration = initialChickenDuration;
int minChickenDuration = 500;
int speedIncreasePerBonk = 25;
bool showFlameEffect = false;
int flameEffectIntensity = 0;

// Gnar-shredding variables
int bonkStreak = 0;
bool frenzyMode = false;
unsigned long frenzyStartTime = 0;
const int frenzyDuration = 3000;

// Function prototypes
void transformTouchCoordinates(uint16_t *x, uint16_t *y);
void handleLoadScreen();
void handleGameSelection();
void handlePlayerSelection();
void handleModeSelectionP1();
void handlePressStartP1();
void handleGameplayP1();
void handleModeSelectionP2();
void handlePressStartP2();
void handleGameplayP2();
void handleResultScreen();
void drawChicken(int x, int y, bool withFlames);
void drawBunny(int x, int y, bool withFlames);
void drawFlag(int x, int y, bool withFlames);
void drawBluey(int x, int y, bool withFlames);
void drawBingo(int x, int y, bool withFlames);
void drawToothless(int x, int y, bool withFlames);  // Nolan's Night Fury with green eyes!
void drawLightFury(int x, int y, bool withFlames);   // White dragon
void drawEgg(int x, int y);
void drawStar(int x, int y);
void drawBalloon(int x, int y);
void drawFish(int x, int y);  // Fish for dragons to collect!
void clearCharacter(int x, int y, int size);
bool isTapInCharacter(int tapX, int tapY, int checkX, int checkY, int size);
void checkTouchEvents();
void updateScoreDisplay();
void resetGameVariables();
void preparePlayerOneGame();
void preparePlayerTwoGame();
void savePlayerOneResults();
void drawFlameEffect(int x, int y, int intensity);
void drawFireworksEffect(int x, int y, int intensity);
void drawPlasmaBlast(int x, int y);  // Nolan's plasma blast effect!
void drawSpecialCharacter(int x, int y);
void clearSpecialCharacter(int x, int y);
bool shouldShowSpecialCharacter();
void handleOriginalGameplayLogic(unsigned long currentTime);
void handleGameplay(GameState currentGameState);

// Unified clear function for any game character
void clearCharacter(int x, int y, int size) {
  int clearMargin = 15 + (showFlameEffect ? 10 : 0);
  tft.fillRect(x-clearMargin, y-clearMargin, 
               size+clearMargin*2, 
               size+clearMargin*2, BLACK);
}

// Compatibility functions for existing code
void clearChicken(int x, int y) {
  clearCharacter(x, y, currentChickenSize);
}

void clearSpecialChicken(int x, int y) {
  clearCharacter(x, y, specialChickenSize);
}

// Unified tap detection function
bool isTapInCharacter(int tapX, int tapY, int checkX, int checkY, int size) {
  int tapMargin = 40;
  return (tapX >= checkX - tapMargin && 
          tapX <= checkX + size + tapMargin &&
          tapY >= checkY - tapMargin && 
          tapY <= checkY + size + tapMargin);
}

// Compatibility function for existing code
bool isTapInChicken(int tapX, int tapY, int checkX, int checkY, int size) {
  return isTapInCharacter(tapX, tapY, checkX, checkY, size);
}

void updateScoreDisplay() {
  tft.fillRect(240, 0, 240, 50, BLACK);
  tft.setCursor(240, 10);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  
  // Display different text based on game variant
  if (gameVariant == BONK_CHICKEN) {
    tft.print("BONKS: ");
  } else if (gameVariant == BONK_BUNNY) {
    tft.print("HOPS: ");
  } else if (gameVariant == BONK_PATRIOTS) {
    tft.print("FLAGS: ");
  } else if (gameVariant == BONK_BLUEY) {
    tft.print("TAPS: ");
  } else if (gameVariant == BONK_DRAGONS) {
    tft.print("RIDES: ");  // Nolan's dragon rides!
  }
  tft.print(score);
  
  tft.setCursor(240, 30);
  tft.setTextColor(CYAN);
  
  if (gameVariant == BONK_CHICKEN) {
    tft.print("SPECIAL: ");
  } else if (gameVariant == BONK_BUNNY) {
    tft.print("EGGS: ");
  } else if (gameVariant == BONK_PATRIOTS) {
    tft.print("STARS: ");
  } else if (gameVariant == BONK_BLUEY) {
    tft.print("BALLOONS: ");
  } else if (gameVariant == BONK_DRAGONS) {
    tft.print("FISH: ");  // Fish for dragons!
  }
  tft.print(level2Score);
  
  // Add player indicator in 2-player mode
  if (twoPlayerMode) {
    tft.setCursor(380, 10);
    tft.setTextColor(GREEN);
    if (currentState == GAMEPLAY_P1) {
      tft.print("P1");
    } else {
      tft.print("P2");
    }
  }
  
  String gamePrefix = (gameVariant == BONK_CHICKEN) ? "BONK! " : 
                     (gameVariant == BONK_BUNNY) ? "HOP! " : 
                     (gameVariant == BONK_PATRIOTS) ? "FLAG! " : 
                     (gameVariant == BONK_BLUEY) ? "TAP! " : "RIDE! ";
  Serial.print(gamePrefix);
  Serial.print("Regular Score: ");
  Serial.print(score);
  Serial.print(" | Special Score: ");
  Serial.println(level2Score);
}

void setup() {
  Serial.begin(9600);
  Wire.end();
  delay(100);
  Wire.begin();
  delay(100);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  if (CTP_RST > 0) {
    pinMode(CTP_RST, OUTPUT);
    digitalWrite(CTP_RST, LOW);
    delay(10);
    digitalWrite(CTP_RST, HIGH);
    delay(50);
  }
  stateStartTime = millis();
  randomSeed(analogRead(0));
}

void loop() {
  checkTouchEvents();
  switch (currentState) {
    case LOAD_SCREEN:
      handleLoadScreen();
      break;
    case GAME_SELECT:
      handleGameSelection();
      break;
    case PLAYER_SELECT:
      handlePlayerSelection();
      break;
    case MODE_SELECT_P1:
      handleModeSelectionP1();
      break;
    case PRESS_START_P1:
      handlePressStartP1();
      break;
    case GAMEPLAY_P1:
      handleGameplayP1();
      break;
    case MODE_SELECT_P2:
      handleModeSelectionP2();
      break;
    case PRESS_START_P2:
      handlePressStartP2();
      break;
    case GAMEPLAY_P2:
      handleGameplayP2();
      break;
    case RESULT_SCREEN:
      handleResultScreen();
      break;
  }
}

void transformTouchCoordinates(uint16_t *x, uint16_t *y) {
  uint16_t tempX = *x;
  uint16_t tempY = *y;
  *x = tempY;
  *y = SCREEN_HEIGHT - tempX;
  *x = constrain(*x, 0, SCREEN_WIDTH - 1);
  *y = constrain(*y, 0, SCREEN_HEIGHT - 1);
}

void checkTouchEvents() {
  uint8_t data[16];
  Wire.beginTransmission(CTP_ADDR);
  Wire.write(0);
  Wire.endTransmission(false);
  Wire.requestFrom(CTP_ADDR, 16);
  
  if (Wire.available()) {
    for (int i = 0; i < 16; i++) {
      data[i] = Wire.read();
    }
    if (data[2] > 0) {
      uint16_t x = ((data[3] & 0x0F) << 8) | data[4];
      uint16_t y = ((data[5] & 0x0F) << 8) | data[6];
      transformTouchCoordinates(&x, &y);
      Serial.print("Touch at X: ");
      Serial.print(x);
      Serial.print(", Y: ");
      Serial.println(y);
      unsigned long currentTime = millis();
      if (currentTime - lastTapTime > debounceDelay) {
        lastTapTime = currentTime;
        
        switch (currentState) {
          case GAME_SELECT:
            // Handle game variant selection (2x2 grid for 4 options)
            if (x > 50 && x < 230 && y > 70 && y < 160) {
              Serial.println("BONK Chicken selected");
              gameVariant = BONK_CHICKEN;
              currentState = PLAYER_SELECT;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            else if (x > 250 && x < 430 && y > 70 && y < 160) {
              Serial.println("BONK Bunny selected");
              gameVariant = BONK_BUNNY;
              currentState = PLAYER_SELECT;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            else if (x > 50 && x < 230 && y > 180 && y < 270) {
              Serial.println("BONK Patriots selected");
              gameVariant = BONK_PATRIOTS;
              currentState = PLAYER_SELECT;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            else if (x > 250 && x < 430 && y > 180 && y < 270) {
              Serial.println("BONK Dragons selected - Nolan's game!");
              gameVariant = BONK_DRAGONS;
              currentState = PLAYER_SELECT;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            break;
            
          case PLAYER_SELECT:
            // Handle player count selection (1P or 2P)
            if (x > 50 && x < 230 && y > 120 && y < 220) {
              Serial.println("1 Player selected");
              twoPlayerMode = false;
              currentState = MODE_SELECT_P1;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            else if (x > 250 && x < 430 && y > 120 && y < 220) {
              Serial.println("2 Players selected");
              twoPlayerMode = true;
              currentState = MODE_SELECT_P1;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            break;
            
          case MODE_SELECT_P1:
            // Handle difficulty selection for Player 1
            if (x > 50 && x < 230 && y > 120 && y < 220) {
              Serial.println("EASY mode selected for Player 1");
              p1Difficulty = EASY;
              currentState = PRESS_START_P1;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            else if (x > 250 && x < 430 && y > 120 && y < 220) {
              Serial.println("SHRED GNAR mode selected for Player 1");
              p1Difficulty = SHRED_GNAR;
              currentState = PRESS_START_P1;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            break;
            
          case PRESS_START_P1:
            // Handle Player 1 press start screen - any tap starts the game
            if (x > 50 && x < 430 && y > 120 && y < 220) {
              Serial.println("Player 1 starting game");
              preparePlayerOneGame();
              currentState = GAMEPLAY_P1;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            break;
            
          case MODE_SELECT_P2:
            // Handle difficulty selection for Player 2
            if (x > 50 && x < 230 && y > 120 && y < 220) {
              Serial.println("EASY mode selected for Player 2");
              p2Difficulty = EASY;
              currentState = PRESS_START_P2;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            else if (x > 250 && x < 430 && y > 120 && y < 220) {
              Serial.println("SHRED GNAR mode selected for Player 2");
              p2Difficulty = SHRED_GNAR;
              currentState = PRESS_START_P2;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            break;
            
          case PRESS_START_P2:
            // Handle Player 2 press start screen - any tap starts the game
            if (x > 50 && x < 430 && y > 120 && y < 220) {
              Serial.println("Player 2 starting game");
              preparePlayerTwoGame();
              currentState = GAMEPLAY_P2;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            break;
            
          case GAMEPLAY_P1:
          case GAMEPLAY_P2:
            // Handle gameplay touch events (common for both players)
            if (chickenVisible && isTapInCharacter(x, y, chickenX, chickenY, currentChickenSize)) {
              score++;
              chickenVisible = false;
              clearCharacter(chickenX, chickenY, currentChickenSize);
              showFeedback = true;
              feedbackTime = currentTime;
              updateScoreDisplay();
              if (currentPlayerDifficulty == SHRED_GNAR) {
                bonkStreak++;
                if (bonkStreak >= 10 && !frenzyMode) {
                  frenzyMode = true;
                  frenzyStartTime = currentTime;
                  currentChickenDuration = currentChickenDuration * 3 / 4;
                  showFrenzyText = true;
                  frenzyTextTime = currentTime;
                }
                if (currentChickenDuration > minChickenDuration) {
                  currentChickenDuration -= speedIncreasePerBonk;
                  currentChickenDuration = max(currentChickenDuration, minChickenDuration);
                  flameEffectIntensity = map(initialChickenDuration - currentChickenDuration, 
                                          0, initialChickenDuration - minChickenDuration, 
                                          0, 5);
                  showFlameEffect = (flameEffectIntensity > 0);
                }
              }
            }
            else if (specialChickenVisible && isTapInCharacter(x, y, specialChickenX, specialChickenY, specialChickenSize)) {
              level2Score++;
              specialChickenVisible = false;
              clearCharacter(specialChickenX, specialChickenY, specialChickenSize);
              showFeedback = true;
              feedbackTime = currentTime;
              updateScoreDisplay();
              if (currentPlayerDifficulty == SHRED_GNAR) {
                bonkStreak++;
                if (bonkStreak >= 10 && !frenzyMode) {
                  frenzyMode = true;
                  frenzyStartTime = currentTime;
                  currentChickenDuration = currentChickenDuration * 3 / 4;
                  showFrenzyText = true;
                  frenzyTextTime = currentTime;
                }
                // Show appropriate special effects based on game variant
                if (gameVariant == BONK_PATRIOTS) {
                  // Fireworks effect for Patriots
                  drawFireworksEffect(specialChickenX, specialChickenY, 5);
                } else if (gameVariant == BONK_BLUEY) {
                  // Confetti effect for Bluey balloon pop
                  drawConfettiEffect(specialChickenX, specialChickenY);
                } else {
                  // Explosion effects for Chicken/Bunny
                  for (int i = 0; i < 5; i++) {
                    int explodeX = specialChickenX + random(-20, 20);
                    int explodeY = specialChickenY + random(-20, 20);
                    uint16_t explodeColor = random(3) == 0 ? RED : (random(2) == 0 ? ORANGE : YELLOW);
                    tft.fillCircle(explodeX, explodeY, random(5, 15), explodeColor);
                    delay(20);
                  }
                }
                
                // Store the position info for proper clearing later
                int textX = specialChickenX - 20;
                int textY = specialChickenY - 20;
                int textWidth = 100;
                int textHeight = 40;
                
                // Show appropriate text based on game variant
                tft.setTextColor(YELLOW);
                tft.setTextSize(4);
                tft.setCursor(textX, textY);
                if (gameVariant == BONK_PATRIOTS) {
                  tft.print("USA!");
                  textWidth = 80; // USA! is shorter than GNAR!
                } else if (gameVariant == BONK_BLUEY) {
                  tft.print("WOW!");
                  textWidth = 80; // WOW! is short like USA!
                } else {
                  tft.print("GNAR!");
                }
                
                // Set up a timer to clear this text after a brief delay
                showFrenzyText = true;
                frenzyTextTime = currentTime;
                
                // Store the area to clear in global variables
                lastSpecialChickenX = textX;
                lastSpecialChickenY = textY;
                specialChickenSize = max(textWidth, textHeight);
                
                clearCharacter(specialChickenX, specialChickenY, specialChickenSize);
              }
            }
            else if (currentPlayerDifficulty == EASY && !chickenVisible && 
                   (currentTime - chickenDisappearTime < afterDisappearGracePeriod) && 
                   isTapInCharacter(x, y, lastChickenX, lastChickenY, currentChickenSize)) {
              score++;
              showFeedback = true;
              feedbackTime = currentTime;
              updateScoreDisplay();
            }
            else if (currentPlayerDifficulty == EASY && !specialChickenVisible && 
                   (currentTime - specialChickenDisappearTime < afterDisappearGracePeriod) && 
                   isTapInCharacter(x, y, lastSpecialChickenX, lastSpecialChickenY, specialChickenSize)) {
              level2Score++;
              showFeedback = true;
              feedbackTime = currentTime;
              updateScoreDisplay();
            }
            break;
            
          default:
            break;
        }
      }
    }
  }
}

void resetGameVariables() {
  score = 0;
  level2Score = 0;
  timeLeft = 45;
  lastChickenTime = 0;
  lastSpecialChickenTime = 0;
  lastTimerUpdate = 0;
  chickenVisible = false;
  specialChickenVisible = false;
  showFeedback = false;
  showSpecialChicken = false;
  showFrenzyText = false;
  currentChickenDuration = initialChickenDuration;
  flameEffectIntensity = 0;
  showFlameEffect = false;
  bonkStreak = 0;
  frenzyMode = false;
  afterDisappearGracePeriod = (currentPlayerDifficulty == EASY) ? 800 : 0;
}

// Prepare the game for Player 1
void preparePlayerOneGame() {
  // Set current player difficulty
  currentPlayerDifficulty = p1Difficulty;
  // Reset game variables for a new game
  resetGameVariables();
  // Reset scores for player 1
  score = 0;
  level2Score = 0;
}

// Save Player 1's results for comparison later
void savePlayerOneResults() {
  p1Score = score;
  p1SpecialScore = level2Score;
}

// Prepare the game for Player 2
void preparePlayerTwoGame() {
  // Set current player difficulty
  currentPlayerDifficulty = p2Difficulty;
  // Reset game variables for a new game
  resetGameVariables();
  // Reset scores for player 2
  score = 0;
  level2Score = 0;
}

void handleLoadScreen() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - stateStartTime;
  if (loadScreenPhase == 0 && elapsedTime < 2000) {
    if (elapsedTime < 100) {
      tft.fillScreen(BLACK);
      tft.setTextColor(WHITE);
      tft.setTextSize(6);
      tft.setCursor(140, 120);
      tft.println("BONK");
    }
  }
  else if (loadScreenPhase <= 1 && elapsedTime >= 2000 && elapsedTime < 4000) {
    if (loadScreenPhase == 0) {
      loadScreenPhase = 1;
      tft.fillScreen(BLACK);
      tft.setTextColor(WHITE);
      tft.setTextSize(2);
      tft.setCursor(70, 140);
      tft.println("Created 2025 by Trent Von Holten");
    }
  }
  else if (loadScreenPhase <= 2 && elapsedTime >= 4000 && elapsedTime < 7000) {
    if (loadScreenPhase == 1) {
      loadScreenPhase = 2;
      tft.fillScreen(BLACK);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.setCursor(50, 100);
      tft.println("RULES: BONK the");
      tft.setCursor(50, 140);
      tft.println("character... if you can");
    }
  }
  else if (elapsedTime >= 7000) {
    // After splash screens, go to game variant selection
    currentState = GAME_SELECT;
    stateStartTime = currentTime;
    tft.fillScreen(BLACK);
    Serial.println("Switching to GAME_SELECT");
  }
}

// Updated function to handle FOUR game variant options in 2x2 grid
void handleGameSelection() {
  unsigned long currentTime = millis();
  
  // Draw the screen once when entering this state or refresh periodically
  if ((currentTime - stateStartTime) < 100 || 
      ((currentTime - stateStartTime) % 5000 >= 0 && (currentTime - stateStartTime) % 5000 < 100)) {
    
    Serial.println("Drawing GAME SELECT screen");
    tft.fillScreen(BLACK);
    
    // Title
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.setCursor(120, 20);
    tft.println("SELECT GAME");
    
    // BONK Chicken option (top left)
    tft.drawRect(50, 70, 180, 90, YELLOW);
    tft.drawRect(51, 71, 178, 88, YELLOW);
    tft.fillRect(52, 72, 176, 86, RED);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(90, 95);
    tft.println("BONK");
    tft.setCursor(80, 120);
    tft.println("CHICKEN");
    
    // BONK Bunny option (top right)
    tft.drawRect(250, 70, 180, 90, WHITE);
    tft.drawRect(251, 71, 178, 88, WHITE);
    tft.fillRect(252, 72, 176, 86, PURPLE);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(290, 95);
    tft.println("BONK");
    tft.setCursor(285, 120);
    tft.println("BUNNY");
    
    // BONK Patriots option (bottom left)
    tft.drawRect(50, 180, 180, 90, BLUE);
    tft.drawRect(51, 181, 178, 88, BLUE);
    tft.fillRect(52, 182, 176, 86, RED);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(90, 205);
    tft.println("BONK");
    tft.setCursor(75, 230);
    tft.println("PATRIOTS");
    
    // BONK Dragons option (bottom right) - NOLAN'S!
    tft.drawRect(250, 180, 180, 90, PLASMA_BLUE);
    tft.drawRect(251, 181, 178, 88, PLASMA_BLUE);
    tft.fillRect(252, 182, 176, 86, DARK_GRAY);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(290, 205);
    tft.println("BONK");
    tft.setCursor(275, 230);
    tft.println("DRAGONS");
    
    // Small character previews
    tft.setTextSize(1);
    tft.setCursor(105, 145);
    tft.setTextColor(YELLOW);
    tft.print("Classic!");
    
    tft.setCursor(295, 145);
    tft.setTextColor(MAGENTA);
    tft.print("Easter!");
    
    tft.setCursor(95, 255);
    tft.setTextColor(RED);
    tft.print("Patriotic!");
    
    tft.setCursor(280, 255);
    tft.setTextColor(GREEN);
    tft.print("Train Dragons!");
  }
}

// New function to handle player count selection
void handlePlayerSelection() {
  unsigned long currentTime = millis();
  
  // Draw the screen once when entering this state or refresh periodically
  if ((currentTime - stateStartTime) < 100 || 
      ((currentTime - stateStartTime) % 5000 >= 0 && (currentTime - stateStartTime) % 5000 < 100)) {
    
    Serial.println("Drawing PLAYER SELECT screen");
    tft.fillScreen(BLACK);
    
    // Title
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.setCursor(60, 40);
    tft.println("SELECT PLAYERS");
    
    // 1 Player option
    tft.drawRect(50, 120, 180, 100, GREEN);
    tft.drawRect(51, 121, 178, 98, GREEN);
    tft.fillRect(52, 122, 176, 96, BLUE);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.setCursor(120, 160);
    tft.println("1P");
    
    // 2 Player option
    tft.drawRect(250, 120, 180, 100, RED);
    tft.drawRect(251, 121, 178, 98, RED);
    tft.fillRect(252, 122, 176, 96, MAGENTA);
    tft.setTextColor(YELLOW);
    tft.setTextSize(4);
    tft.setCursor(320, 160);
    tft.println("2P");
    
    // Descriptions
    tft.setTextSize(1);
    tft.setTextColor(GREEN);
    tft.setCursor(85, 230);
    tft.println("Single Player Mode");
    
    tft.setTextColor(WHITE);
    tft.setCursor(270, 230);
    tft.println("Play with a Friend!");
  }
}

// Player 1 mode selection
void handleModeSelectionP1() {
  unsigned long currentTime = millis();
  // Draw the screen once when entering this state or every 5 seconds as a fallback
  if ((currentTime - stateStartTime) < 100 || 
      ((currentTime - stateStartTime) % 5000 >= 0 && (currentTime - stateStartTime) % 5000 < 100)) {
    Serial.println("Drawing MODE SELECT screen for Player 1");
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.setCursor(50, 40);
    tft.println("PLAYER 1 MODE");
    tft.drawRect(50, 120, 180, 100, GREEN);
    tft.drawRect(51, 121, 178, 98, GREEN);
    tft.fillRect(52, 122, 176, 96, BLUE);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.setCursor(105, 160);
    tft.println("EASY");
    tft.drawRect(250, 120, 180, 100, RED);
    tft.drawRect(251, 121, 178, 98, RED);
    tft.fillRect(252, 122, 176, 96, MAGENTA);
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(270, 160);
    tft.println("SHRED GNAR");
    tft.setTextSize(1);
    tft.setTextColor(GREEN);
    tft.setCursor(60, 230);
    tft.println("Kid-friendly");
    tft.setTextColor(WHITE);
    tft.setCursor(270, 230);
    tft.println("Gets faster. CHALLENGE!");
  }
}

// Player 2 mode selection
void handleModeSelectionP2() {
  unsigned long currentTime = millis();
  // Draw the screen once when entering this state or every 5 seconds as a fallback
  if ((currentTime - stateStartTime) < 100 || 
      ((currentTime - stateStartTime) % 5000 >= 0 && (currentTime - stateStartTime) % 5000 < 100)) {
    Serial.println("Drawing MODE SELECT screen for Player 2");
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.setCursor(50, 40);
    tft.println("PLAYER 2 MODE");
    tft.drawRect(50, 120, 180, 100, GREEN);
    tft.drawRect(51, 121, 178, 98, GREEN);
    tft.fillRect(52, 122, 176, 96, BLUE);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.setCursor(105, 160);
    tft.println("EASY");
    tft.drawRect(250, 120, 180, 100, RED);
    tft.drawRect(251, 121, 178, 98, RED);
    tft.fillRect(252, 122, 176, 96, MAGENTA);
    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(270, 160);
    tft.println("SHRED GNAR");
    tft.setTextSize(1);
    tft.setTextColor(GREEN);
    tft.setCursor(60, 230);
    tft.println("Kid-friendly");
    tft.setTextColor(WHITE);
    tft.setCursor(270, 230);
    tft.println("Gets faster. CHALLENGE!");
  }
}

// Player 1 press start screen
void handlePressStartP1() {
  unsigned long currentTime = millis();
  // Draw the screen once when entering this state or refresh periodically
  if ((currentTime - stateStartTime) < 100 || 
      ((currentTime - stateStartTime) % 5000 >= 0 && (currentTime - stateStartTime) % 5000 < 100)) {
    
    Serial.println("Drawing PRESS START screen for Player 1");
    tft.fillScreen(BLACK);
    
    // Title
    tft.setTextColor(GREEN);
    tft.setTextSize(4);
    tft.setCursor(120, 40);
    tft.println("PLAYER 1");
    
    // Game variant and difficulty
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(100, 90);
    tft.print("Game: BONK ");
    String variantName = (gameVariant == BONK_CHICKEN) ? "CHICKEN" : 
                        (gameVariant == BONK_BUNNY) ? "BUNNY" : 
                        (gameVariant == BONK_PATRIOTS) ? "PATRIOTS" : "BLUEY";
    tft.print(variantName);
    
    tft.setCursor(100, 120);
    tft.print("Mode: ");
    if (p1Difficulty == EASY) {
      tft.setTextColor(GREEN);
      tft.print("EASY");
    } else {
      tft.setTextColor(RED);
      tft.print("SHRED GNAR");
    }
    
    // Press start button
    tft.drawRect(120, 160, 240, 80, BLUE);
    tft.drawRect(121, 161, 238, 78, BLUE);
    tft.fillRect(122, 162, 236, 76, GREEN);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.setCursor(140, 190);
    tft.println("PRESS START");
    
    // Flashing effect
    if ((currentTime / 500) % 2 == 0) {
      tft.setTextColor(YELLOW);
      tft.setTextSize(2);
      tft.setCursor(150, 250);
      tft.println("TAP TO BEGIN!");
    }
  }
}

// Player 2 press start screen
void handlePressStartP2() {
  unsigned long currentTime = millis();
  // Draw the screen once when entering this state or refresh periodically
  if ((currentTime - stateStartTime) < 100 || 
      ((currentTime - stateStartTime) % 5000 >= 0 && (currentTime - stateStartTime) % 5000 < 100)) {
    
    Serial.println("Drawing PRESS START screen for Player 2");
    tft.fillScreen(BLACK);
    
    // Player 1 results
    tft.setTextColor(BLUE);
    tft.setTextSize(2);
    tft.setCursor(50, 10);
    tft.println("Player 1 Score:");
    
    tft.setTextColor(WHITE);
    tft.setCursor(70, 40);
    String scoreLabel = (gameVariant == BONK_CHICKEN) ? "BONKS: " : 
                       (gameVariant == BONK_BUNNY) ? "HOPS: " : 
                       (gameVariant == BONK_PATRIOTS) ? "FLAGS: " : "TAPS: ";
    tft.print(scoreLabel);
    tft.print(p1Score);
    
    tft.setCursor(70, 60);
    String specialLabel = (gameVariant == BONK_CHICKEN) ? "SPECIAL: " : 
                         (gameVariant == BONK_BUNNY) ? "EGGS: " : 
                         (gameVariant == BONK_PATRIOTS) ? "STARS: " : "BALLOONS: ";
    tft.print(specialLabel);
    tft.print(p1SpecialScore);
    
    // Title
    tft.setTextColor(RED);
    tft.setTextSize(4);
    tft.setCursor(120, 90);
    tft.println("PLAYER 2");
    
    // Game variant and difficulty
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(100, 140);
    tft.print("Mode: ");
    if (p2Difficulty == EASY) {
      tft.setTextColor(GREEN);
      tft.print("EASY");
    } else {
      tft.setTextColor(RED);
      tft.print("SHRED GNAR");
    }
    
    // Press start button
    tft.drawRect(120, 180, 240, 80, BLUE);
    tft.drawRect(121, 181, 238, 78, BLUE);
    tft.fillRect(122, 182, 236, 76, RED);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.setCursor(140, 210);
    tft.println("PRESS START");
    
    // Flashing effect
    if ((currentTime / 500) % 2 == 0) {
      tft.setTextColor(YELLOW);
      tft.setTextSize(2);
      tft.setCursor(150, 270);
      tft.println("YOUR TURN!");
    }
  }
}

// New function to replace shouldShowSpecialChicken
bool shouldShowSpecialCharacter() {
  return random(100) < 15; // 15% chance
}

// Compatibility function
bool shouldShowSpecialChicken() {
  return shouldShowSpecialCharacter();
}

// Draws a bunny character for BONK_BUNNY variant
void drawBunny(int x, int y, bool withFlames) {
  uint16_t bunnyColor = WHITE;
  tft.fillRect(x, y, currentChickenSize, currentChickenSize, bunnyColor);
  
  // Bunny ears
  tft.fillRect(x+currentChickenSize/4, y-10, 6, 15, bunnyColor);
  tft.fillRect(x+currentChickenSize*3/4, y-10, 6, 15, bunnyColor);
  
  // Bunny face features
  tft.fillRect(x+currentChickenSize/4, y+currentChickenSize/4, 5, 5, BLACK);  // Eye
  tft.fillRect(x+currentChickenSize*2/3, y+currentChickenSize/4, 5, 5, BLACK); // Eye
  tft.fillRect(x+currentChickenSize*2/5, y+currentChickenSize*3/5, 10, 5, MAGENTA); // Nose
  
  // Bunny feet
  tft.fillRect(x+currentChickenSize*2/3, y+currentChickenSize-5, 15, 5, PURPLE);
  tft.fillRect(x-5, y+currentChickenSize-5, 15, 5, PURPLE);
  
  // Outline
  tft.drawRect(x-1, y-1, currentChickenSize+2, currentChickenSize+2, MAGENTA);
  
  // Flame effect (same as chicken)
  if (withFlames) {
    drawFlameEffect(x, y, flameEffectIntensity);
  }
}

// Draws Bluey character for BONK_BLUEY variant
void drawBluey(int x, int y, bool withFlames) {
  // Bluey's body (blue)
  tft.fillRect(x, y, currentChickenSize, currentChickenSize, BLUE);
  
  // Bluey's white chest/belly
  tft.fillRect(x+currentChickenSize/3, y+currentChickenSize/2, currentChickenSize/3, currentChickenSize/2, WHITE);
  
  // Bluey's ears (pointed up)
  tft.fillTriangle(x+5, y, x+10, y-8, x+15, y, BLUE);
  tft.fillTriangle(x+currentChickenSize-15, y, x+currentChickenSize-10, y-8, x+currentChickenSize-5, y, BLUE);
  
  // Bluey's eyes
  tft.fillCircle(x+currentChickenSize/3, y+currentChickenSize/3, 3, WHITE);
  tft.fillCircle(x+currentChickenSize*2/3, y+currentChickenSize/3, 3, WHITE);
  tft.fillCircle(x+currentChickenSize/3, y+currentChickenSize/3, 2, BLACK);
  tft.fillCircle(x+currentChickenSize*2/3, y+currentChickenSize/3, 2, BLACK);
  
  // Bluey's black nose
  tft.fillCircle(x+currentChickenSize/2, y+currentChickenSize/2, 3, BLACK);
  
  // Bluey's tail
  tft.fillRect(x+currentChickenSize-3, y+currentChickenSize/3, 8, 5, BLUE);
  
  // Outline
  tft.drawRect(x-1, y-1, currentChickenSize+2, currentChickenSize+2, CYAN);
  
  if (withFlames) {
    drawFlameEffect(x, y, flameEffectIntensity);
  }
}

// Draws Bingo character for BONK_BLUEY variant
void drawBingo(int x, int y, bool withFlames) {
  // Bingo's body (orange)
  tft.fillRect(x, y, currentChickenSize, currentChickenSize, ORANGE);
  
  // Bingo's darker orange patches
  tft.fillRect(x+5, y+5, currentChickenSize/4, currentChickenSize/4, DARK_ORANGE);
  tft.fillRect(x+currentChickenSize*2/3, y+currentChickenSize/2, currentChickenSize/4, currentChickenSize/3, DARK_ORANGE);
  
  // Bingo's ears (floppy)
  tft.fillRect(x+5, y-5, 10, 8, ORANGE);
  tft.fillRect(x+currentChickenSize-15, y-5, 10, 8, ORANGE);
  
  // Bingo's eyes
  tft.fillCircle(x+currentChickenSize/3, y+currentChickenSize/3, 3, WHITE);
  tft.fillCircle(x+currentChickenSize*2/3, y+currentChickenSize/3, 3, WHITE);
  tft.fillCircle(x+currentChickenSize/3, y+currentChickenSize/3, 2, BLACK);
  tft.fillCircle(x+currentChickenSize*2/3, y+currentChickenSize/3, 2, BLACK);
  
  // Bingo's black nose
  tft.fillCircle(x+currentChickenSize/2, y+currentChickenSize/2, 3, BLACK);
  
  // Bingo's tail
  tft.fillRect(x+currentChickenSize-3, y+currentChickenSize/3, 8, 5, ORANGE);
  
  // Outline
  tft.drawRect(x-1, y-1, currentChickenSize+2, currentChickenSize+2, RED);
  
  if (withFlames) {
    drawFlameEffect(x, y, flameEffectIntensity);
  }
}

// NEW: Draws an American flag for BONK_PATRIOTS variant
void drawFlag(int x, int y, bool withFlames) {
  // Flag background - white base
  tft.fillRect(x, y, currentChickenSize, currentChickenSize, WHITE);
  
  // Red stripes
  for (int i = 0; i < 5; i++) {
    int stripeY = y + (i * 2 + 1) * currentChickenSize / 10;
    tft.fillRect(x, stripeY, currentChickenSize, currentChickenSize/10, RED);
  }
  
  // Blue canton (upper left corner)
  int cantonWidth = currentChickenSize * 2 / 5;
  int cantonHeight = currentChickenSize / 2;
  tft.fillRect(x, y, cantonWidth, cantonHeight, BLUE);
  
  // Stars in canton (simplified - just a few white dots)
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int starX = x + (col + 1) * cantonWidth / 4;
      int starY = y + (row + 1) * cantonHeight / 4;
      tft.fillCircle(starX, starY, 2, WHITE);
    }
  }
  
  // Flag pole
  tft.fillRect(x-3, y-5, 3, currentChickenSize+10, YELLOW);
  
  // Outline
  tft.drawRect(x-1, y-1, currentChickenSize+2, currentChickenSize+2, BLUE);
  
  // Flame effect
  if (withFlames) {
    drawFlameEffect(x, y, flameEffectIntensity);
  }
}

// Draws an egg for BONK_BUNNY special character
void drawEgg(int x, int y) {
  tft.fillRect(x, y, specialChickenSize, specialChickenSize, CYAN);
  
  // Egg pattern
  for (int i = 0; i < 5; i++) {
    int spotX = x + random(5, specialChickenSize - 5);
    int spotY = y + random(5, specialChickenSize - 5);
    tft.fillCircle(spotX, spotY, 3, YELLOW);
  }
  
  // Egg outline
  tft.drawRect(x-1, y-1, specialChickenSize+2, specialChickenSize+2, BLUE);
  
  // Easter sparkles
  tft.fillCircle(x-2, y-2, 2, WHITE);
  tft.fillCircle(x+specialChickenSize+2, y-2, 2, YELLOW);
  tft.fillCircle(x-2, y+specialChickenSize+2, 2, GREEN);
  tft.fillCircle(x+specialChickenSize+2, y+specialChickenSize+2, 2, RED);
}

// NEW: Draws a star for BONK_PATRIOTS special character
void drawStar(int x, int y) {
  // Star background
  tft.fillRect(x, y, specialChickenSize, specialChickenSize, WHITE);
  
  // Draw a simple 5-pointed star using lines and filled triangles
  int centerX = x + specialChickenSize/2;
  int centerY = y + specialChickenSize/2;
  int radius = specialChickenSize/3;
  
  // Outer star points (simplified as a filled circle with radiating lines)
  tft.fillCircle(centerX, centerY, radius, BLUE);
  
  // Star rays
  for (int i = 0; i < 8; i++) {
    float angle = i * 3.14159 / 4;
    int endX = centerX + radius * 1.3 * cos(angle);
    int endY = centerY + radius * 1.3 * sin(angle);
    tft.drawLine(centerX, centerY, endX, endY, WHITE);
    tft.drawLine(centerX+1, centerY, endX, endY, WHITE); // Thicker lines
  }
  
  // Center white star
  tft.fillCircle(centerX, centerY, radius/2, WHITE);
  
  // Outline
  tft.drawRect(x-1, y-1, specialChickenSize+2, specialChickenSize+2, RED);
  
  // Patriotic sparkles
  tft.fillCircle(x-2, y-2, 2, RED);
  tft.fillCircle(x+specialChickenSize+2, y-2, 2, WHITE);
  tft.fillCircle(x-2, y+specialChickenSize+2, 2, BLUE);
  tft.fillCircle(x+specialChickenSize+2, y+specialChickenSize+2, 2, RED);
}

// Draws a red balloon for BONK_BLUEY special character
void drawBalloon(int x, int y) {
  // Balloon body (red circle)
  int centerX = x + specialChickenSize/2;
  int centerY = y + specialChickenSize/2 - 5;
  int balloonRadius = specialChickenSize/2 - 5;
  
  // Main balloon
  tft.fillCircle(centerX, centerY, balloonRadius, RED);
  
  // Balloon highlight (to make it look shiny)
  tft.fillCircle(centerX - balloonRadius/3, centerY - balloonRadius/3, balloonRadius/4, WHITE);
  
  // Balloon string
  tft.drawLine(centerX, centerY + balloonRadius, centerX, y + specialChickenSize + 5, WHITE);
  tft.drawLine(centerX+1, centerY + balloonRadius, centerX+1, y + specialChickenSize + 5, WHITE);
  
  // Little triangle at bottom of balloon
  tft.fillTriangle(centerX-2, centerY + balloonRadius, 
                   centerX+2, centerY + balloonRadius,
                   centerX, centerY + balloonRadius + 3, RED);
  
  // Sparkles around balloon
  tft.fillCircle(x-2, y-2, 2, YELLOW);
  tft.fillCircle(x+specialChickenSize+2, y-2, 2, CYAN);
  tft.fillCircle(x-2, y+specialChickenSize+2, 2, GREEN);
  tft.fillCircle(x+specialChickenSize+2, y+specialChickenSize+2, 2, MAGENTA);
}

// NEW: Fireworks effect for Patriots special character
void drawFireworksEffect(int x, int y, int intensity) {
  // Multiple bursts of red, white, and blue
  for (int burst = 0; burst < 3; burst++) {
    int burstX = x + random(-15, 15);
    int burstY = y + random(-15, 15);
    
    // Radiating firework particles
    for (int i = 0; i < 8; i++) {
      float angle = i * 3.14159 / 4;
      int distance = 10 + random(0, 10);
      int sparkX = burstX + distance * cos(angle);
      int sparkY = burstY + distance * sin(angle);
      
      // Red, white, or blue sparks
      uint16_t sparkColor;
      switch (i % 3) {
        case 0: sparkColor = RED; break;
        case 1: sparkColor = WHITE; break;
        case 2: sparkColor = BLUE; break;
      }
      
      tft.fillCircle(sparkX, sparkY, random(2, 5), sparkColor);
    }
    delay(50); // Brief delay between bursts
  }
}

// Confetti effect for Bluey balloon pop
void drawConfettiEffect(int x, int y) {
  // Colorful confetti pieces falling
  for (int i = 0; i < 15; i++) {
    int confettiX = x + random(-20, 20);
    int confettiY = y + random(-20, 20);
    
    // Random confetti colors
    uint16_t confettiColor;
    switch (random(6)) {
      case 0: confettiColor = RED; break;
      case 1: confettiColor = YELLOW; break;
      case 2: confettiColor = GREEN; break;
      case 3: confettiColor = BLUE; break;
      case 4: confettiColor = MAGENTA; break;
      case 5: confettiColor = CYAN; break;
    }
    
    // Draw small rectangles as confetti
    tft.fillRect(confettiX, confettiY, random(3, 7), random(3, 7), confettiColor);
  }
  
  // Show "POP!" text
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.setCursor(x-20, y-10);
  tft.print("POP!");
}

// NOLAN'S DRAGONS: Toothless the Night Fury
void drawToothless(int x, int y, bool withFlames) {
  // Toothless body - dark gray/black
  tft.fillCircle(x + currentChickenSize/2, y + currentChickenSize/2, currentChickenSize/2, DARK_GRAY);
  
  // Head
  tft.fillCircle(x + currentChickenSize/2, y + currentChickenSize/3, currentChickenSize/3, DARK_GRAY);
  
  // Ears/head fins (the pointy things on his head)
  tft.fillTriangle(x + 5, y + 5, x + 10, y - 3, x + 15, y + 5, DARK_GRAY);
  tft.fillTriangle(x + 25, y + 5, x + 30, y - 3, x + 35, y + 5, DARK_GRAY);
  
  // GREEN EYES with BLACK PUPILS like Nolan said!
  // Left eye
  tft.fillCircle(x + 12, y + 10, 4, GREEN);
  tft.fillCircle(x + 12, y + 10, 2, BLACK);  // Black pupil
  // Right eye  
  tft.fillCircle(x + 28, y + 10, 4, GREEN);
  tft.fillCircle(x + 28, y + 10, 2, BLACK);  // Black pupil
  
  // Wings (simplified)
  tft.fillTriangle(x - 5, y + 15, x - 15, y + 25, x + 5, y + 25, DARK_GRAY);
  tft.fillTriangle(x + 35, y + 15, x + 45, y + 25, x + 35, y + 25, DARK_GRAY);
  
  // Tail
  tft.fillRect(x + 15, y + 35, 10, 5, DARK_GRAY);
  tft.fillTriangle(x + 20, y + 38, x + 15, y + 45, x + 25, y + 45, DARK_GRAY);
  
  // If with plasma effect
  if (withFlames) {
    // Blue plasma glow around Toothless
    tft.drawCircle(x + currentChickenSize/2, y + currentChickenSize/2, currentChickenSize/2 + 5, PLASMA_BLUE);
    tft.drawCircle(x + currentChickenSize/2, y + currentChickenSize/2, currentChickenSize/2 + 6, PLASMA_BLUE);
  }
}

// Light Fury - the white dragon
void drawLightFury(int x, int y, bool withFlames) {
  // Light Fury body - white/light blue
  tft.fillCircle(x + currentChickenSize/2, y + currentChickenSize/2, currentChickenSize/2, WHITE);
  
  // Head with slight blue tint
  tft.fillCircle(x + currentChickenSize/2, y + currentChickenSize/3, currentChickenSize/3, LIGHT_BLUE);
  
  // Smoother head fins than Toothless
  tft.fillTriangle(x + 8, y + 5, x + 12, y - 2, x + 16, y + 5, LIGHT_BLUE);
  tft.fillTriangle(x + 24, y + 5, x + 28, y - 2, x + 32, y + 5, LIGHT_BLUE);
  
  // Blue eyes
  tft.fillCircle(x + 12, y + 10, 4, BLUE);
  tft.fillCircle(x + 12, y + 10, 2, BLACK);  // Black pupil
  tft.fillCircle(x + 28, y + 10, 4, BLUE);
  tft.fillCircle(x + 28, y + 10, 2, BLACK);  // Black pupil
  
  // Wings
  tft.fillTriangle(x - 5, y + 15, x - 15, y + 25, x + 5, y + 25, LIGHT_BLUE);
  tft.fillTriangle(x + 35, y + 15, x + 45, y + 25, x + 35, y + 25, LIGHT_BLUE);
  
  // Tail
  tft.fillRect(x + 15, y + 35, 10, 5, LIGHT_BLUE);
  tft.fillTriangle(x + 20, y + 38, x + 15, y + 45, x + 25, y + 45, WHITE);
  
  // If with plasma effect
  if (withFlames) {
    // Light plasma glow
    tft.drawCircle(x + currentChickenSize/2, y + currentChickenSize/2, currentChickenSize/2 + 5, CYAN);
    tft.drawCircle(x + currentChickenSize/2, y + currentChickenSize/2, currentChickenSize/2 + 6, CYAN);
  }
}

// Fish for dragons to collect!
void drawFish(int x, int y) {
  // Fish body - silver/gray
  tft.fillCircle(x + specialChickenSize/2, y + specialChickenSize/2, specialChickenSize/2 - 2, CYAN);
  
  // Fish tail
  tft.fillTriangle(x + specialChickenSize - 5, y + specialChickenSize/2,
                   x + specialChickenSize + 5, y + specialChickenSize/2 - 8,
                   x + specialChickenSize + 5, y + specialChickenSize/2 + 8, CYAN);
  
  // Eye
  tft.fillCircle(x + 8, y + specialChickenSize/2, 2, WHITE);
  tft.fillCircle(x + 8, y + specialChickenSize/2, 1, BLACK);
  
  // Scales effect
  tft.drawCircle(x + 15, y + specialChickenSize/2, 3, WHITE);
  tft.drawCircle(x + 20, y + specialChickenSize/2 - 3, 3, WHITE);
  tft.drawCircle(x + 20, y + specialChickenSize/2 + 3, 3, WHITE);
  
  // Sparkles around fish
  tft.fillCircle(x-2, y-2, 2, BLUE);
  tft.fillCircle(x+specialChickenSize+2, y-2, 2, BLUE);
}

// NOLAN'S SUPER COOL PLASMA BLAST!
void drawPlasmaBlast(int x, int y) {
  // Center of the blast
  int centerX = x + currentChickenSize/2;
  int centerY = y + currentChickenSize/2;
  
  // Multiple plasma rings shooting outward
  for (int ring = 0; ring < 3; ring++) {
    int radius = 10 + (ring * 15);
    
    // Draw plasma ring
    tft.drawCircle(centerX, centerY, radius, PLASMA_BLUE);
    tft.drawCircle(centerX, centerY, radius + 1, CYAN);
    tft.drawCircle(centerX, centerY, radius + 2, PLASMA_BLUE);
    
    // Plasma particles
    for (int i = 0; i < 8; i++) {
      float angle = i * 3.14159 / 4;
      int particleX = centerX + radius * cos(angle);
      int particleY = centerY + radius * sin(angle);
      
      tft.fillCircle(particleX, particleY, 3, PLASMA_BLUE);
      tft.fillCircle(particleX, particleY, 2, CYAN);
    }
  }
  
  // Central bright core
  tft.fillCircle(centerX, centerY, 5, WHITE);
  tft.fillCircle(centerX, centerY, 7, CYAN);
  tft.fillCircle(centerX, centerY, 9, PLASMA_BLUE);
  
  // Show blast text
  tft.setTextColor(PLASMA_BLUE);
  tft.setTextSize(2);
  tft.setCursor(x-10, y-20);
  tft.print("BLAST!");
}

// Unified function to draw the appropriate special character
void drawSpecialCharacter(int x, int y) {
  if (gameVariant == BONK_CHICKEN) {
    // Original blue chicken
    tft.fillRect(x, y, specialChickenSize, specialChickenSize, BLUE);
    tft.fillRect(x+specialChickenSize*2/3, y-3, 6, 8, MAGENTA);
    tft.fillRect(x+specialChickenSize/6, y+specialChickenSize/6, 4, 4, WHITE);
    tft.fillRect(x+specialChickenSize*2/3, y+specialChickenSize-4, 12, 4, PURPLE);
    tft.fillRect(x-4, y+specialChickenSize-4, 12, 4, PURPLE);
    tft.drawRect(x-1, y-1, specialChickenSize+2, specialChickenSize+2, CYAN);
    tft.fillCircle(x-2, y-2, 2, WHITE);
    tft.fillCircle(x+specialChickenSize+2, y-2, 2, WHITE);
    tft.fillCircle(x-2, y+specialChickenSize+2, 2, WHITE);
    tft.fillCircle(x+specialChickenSize+2, y+specialChickenSize+2, 2, WHITE);
  } else if (gameVariant == BONK_BUNNY) {
    // Easter egg
    drawEgg(x, y);
  } else if (gameVariant == BONK_PATRIOTS) {
    // Patriotic star
    drawStar(x, y);
  } else if (gameVariant == BONK_BLUEY) {
    // Red balloon
    drawBalloon(x, y);
  } else if (gameVariant == BONK_DRAGONS) {
    // Fish for dragons to collect!
    drawFish(x, y);
  }
}

// Generic gameplay handler function
void handleGameplay(GameState currentGameState) {
  unsigned long currentTime = millis();
  unsigned long gameElapsedTime = currentTime - stateStartTime;
  
  if (gameElapsedTime < 100) {
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("Time: ");
    tft.print(timeLeft);
    
    updateScoreDisplay();
    
    tft.setCursor(10, 300);
    tft.setTextSize(1);
    
    // Show player number in two-player mode
    if (twoPlayerMode) {
      tft.setTextColor(currentGameState == GAMEPLAY_P1 ? GREEN : RED);
      tft.print(currentGameState == GAMEPLAY_P1 ? "PLAYER 1 " : "PLAYER 2 ");
    }
    
    if (currentPlayerDifficulty == EASY) {
      tft.setTextColor(GREEN);
      tft.print("EASY MODE");
    } else {
      tft.setTextColor(RED);
      tft.print("SHRED GNAR MODE");
    }
  }
}

// The original gameplay handler code repurposed to work with our new structure
void handleOriginalGameplayLogic(unsigned long currentTime) {
  // Update timer
  if (currentTime - lastTimerUpdate >= 1000) {
    lastTimerUpdate = currentTime;
    timeLeft--;
    tft.fillRect(10, 0, 100, 30, BLACK);
    tft.setCursor(10, 10);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("Time: ");
    tft.print(timeLeft);
  }
  
  // Handle feedback effects
  if (showFeedback) {
    if (gameVariant == BONK_DRAGONS && currentTime - feedbackTime < 500) {
      // PLASMA BLAST EFFECT for dragons!
      if (currentTime - feedbackTime < 100) {
        drawPlasmaBlast(chickenX, chickenY);
      }
    } else if (currentTime - feedbackTime < 300) {
      // Original feedback for other variants
      if ((currentTime - feedbackTime) % 100 < 50) {
        tft.fillCircle(tft.width()/2, tft.height()/2, 20, GREEN);
      } else {
        tft.fillCircle(tft.width()/2, tft.height()/2, 20, BLACK);
      }
    } else {
      tft.fillCircle(tft.width()/2, tft.height()/2, 20, BLACK);
      showFeedback = false;
    }
  }
  
  // Handle frenzy mode
  if (frenzyMode) {
    if (currentTime - frenzyStartTime > frenzyDuration) {
      frenzyMode = false;
      bonkStreak = 0;
      currentChickenDuration = max(initialChickenDuration - (score * speedIncreasePerBonk), minChickenDuration);
    }
  }
  
  // Handle any text that needs to be shown temporarily
  if (showFrenzyText) {
    // For frenzy mode announcement
    if (currentTime - frenzyTextTime < 1000) {
      // Keep the text visible for 1 second
      tft.setTextColor(RED);
      tft.setTextSize(3);
      tft.setCursor(150, 50);
      if (gameVariant == BONK_PATRIOTS) {
        tft.print("FREEDOM TIME!");
      } else if (gameVariant == BONK_BLUEY) {
        tft.print("DANCE MODE!");
      } else if (gameVariant == BONK_DRAGONS) {
        tft.print("DRAGON FURY!");  // Super cool dragon mode!
      } else {
        tft.print("TIME TO SHRED!");
      }
    } 
    else if (currentTime - frenzyTextTime < 1100) {
      // Clear the text after 1 second
      tft.fillRect(150, 50, 240, 30, BLACK);
      showFrenzyText = false;
    }
    
    // For special character text clearing
    if (currentTime - frenzyTextTime >= 1000 && lastSpecialChickenX > 0 && lastSpecialChickenY > 0) {
      // Clear the text area
      tft.fillRect(lastSpecialChickenX, lastSpecialChickenY, 100, 40, BLACK);
      lastSpecialChickenX = 0;
      lastSpecialChickenY = 0;
    }
  }
  
  // Handle regular character spawning
  if (!chickenVisible && (currentTime - lastChickenTime >= (unsigned long)currentChickenDuration)) {
    if (currentPlayerDifficulty == SHRED_GNAR) {
      currentChickenSize = random(30, 60);
    }
    chickenX = random(currentChickenSize + 20, tft.width() - currentChickenSize - 20);
    chickenY = random(40 + currentChickenSize, tft.height() - currentChickenSize - 20);
    
    // Draw the appropriate character based on game variant
    if (gameVariant == BONK_CHICKEN) {
      drawChicken(chickenX, chickenY, showFlameEffect);
    } else if (gameVariant == BONK_BUNNY) {
      drawBunny(chickenX, chickenY, showFlameEffect);
    } else if (gameVariant == BONK_PATRIOTS) {
      drawFlag(chickenX, chickenY, showFlameEffect);
    } else if (gameVariant == BONK_BLUEY) {
      // Randomly choose between Bluey and Bingo
      if (random(2) == 0) {
        drawBluey(chickenX, chickenY, showFlameEffect);
      } else {
        drawBingo(chickenX, chickenY, showFlameEffect);
      }
    } else if (gameVariant == BONK_DRAGONS) {
      // Randomly choose between Toothless and Light Fury!
      if (random(2) == 0) {
        drawToothless(chickenX, chickenY, showFlameEffect);
      } else {
        drawLightFury(chickenX, chickenY, showFlameEffect);
      }
    }
    
    chickenVisible = true;
    lastChickenTime = currentTime;
    
    // Determine if special character should appear
    if (!specialChickenVisible && shouldShowSpecialCharacter()) {
      showSpecialChicken = true;
    }
    if (frenzyMode && !specialChickenVisible && random(100) < 20) {
      showSpecialChicken = true;
    }
  }
  else if (chickenVisible && (currentTime - lastChickenTime >= (unsigned long)currentChickenDuration)) {
    lastChickenX = chickenX;
    lastChickenY = chickenY;
    chickenDisappearTime = currentTime;
    clearCharacter(chickenX, chickenY, currentChickenSize);
    chickenVisible = false;
    lastChickenTime = currentTime;
  }
  
  // Handle special character spawning
  if (showSpecialChicken && !specialChickenVisible && 
      (currentTime - lastSpecialChickenTime >= (unsigned long)currentChickenDuration)) {
    if (currentPlayerDifficulty == SHRED_GNAR) {
      specialChickenSize = random(20, 40);
    }
    
    // Find a position that doesn't overlap with the main character
    do {
      specialChickenX = random(specialChickenSize + 20, tft.width() - specialChickenSize - 20);
      specialChickenY = random(40 + specialChickenSize, tft.height() - specialChickenSize - 20);
    } while (chickenVisible && 
             abs(specialChickenX - chickenX) < 80 && 
             abs(specialChickenY - chickenY) < 80);
    
    // Draw the appropriate special character
    drawSpecialCharacter(specialChickenX, specialChickenY);
    
    specialChickenVisible = true;
    lastSpecialChickenTime = currentTime;
    showSpecialChicken = false;
  }
  else if (specialChickenVisible && (currentTime - lastSpecialChickenTime >= (unsigned long)currentChickenDuration)) {
    lastSpecialChickenX = specialChickenX;
    lastSpecialChickenY = specialChickenY;
    specialChickenDisappearTime = currentTime;
    clearCharacter(specialChickenX, specialChickenY, specialChickenSize);
    specialChickenVisible = false;
    lastSpecialChickenTime = currentTime;
  }
  
  // Add movement to special character in SHRED_GNAR mode
  if (currentPlayerDifficulty == SHRED_GNAR && specialChickenVisible) {
    if (currentTime % 300 < 50) {
      clearCharacter(specialChickenX, specialChickenY, specialChickenSize);
      specialChickenX += random(-10, 11);
      specialChickenY += random(-10, 11);
      specialChickenX = constrain(specialChickenX, specialChickenSize + 20, tft.width() - specialChickenSize - 20);
      specialChickenY = constrain(specialChickenY, 40 + specialChickenSize, tft.height() - specialChickenSize - 20);
      drawSpecialCharacter(specialChickenX, specialChickenY);
    }
  }
}

// Connect gameplay handlers to original logic
void handleGameplayP1() {
  unsigned long currentTime = millis();
  
  // Call the generic game display function
  handleGameplay(GAMEPLAY_P1);
  
  // Run the actual gameplay logic
  handleOriginalGameplayLogic(currentTime);
  
  // Check if game is over
  if (timeLeft <= 0) {
    savePlayerOneResults();
    
    if (twoPlayerMode) {
      currentState = MODE_SELECT_P2;
    } else {
      currentState = RESULT_SCREEN;
    }
    
    stateStartTime = currentTime;
    return;
  }
}

void handleGameplayP2() {
  unsigned long currentTime = millis();
  
  // Call the generic game display function
  handleGameplay(GAMEPLAY_P2);
  
  // Run the actual gameplay logic
  handleOriginalGameplayLogic(currentTime);
  
  // Check if game is over
  if (timeLeft <= 0) {
    currentState = RESULT_SCREEN;
    stateStartTime = currentTime;
    return;
  }
}

void handleResultScreen() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - stateStartTime;
  
  if (elapsedTime < 100) {
    tft.fillScreen(BLACK);
    tft.setTextSize(3);
    
    // Determine win thresholds based on difficulty and create total scores
    int p1WinThreshold = (p1Difficulty == EASY) ? 20 : 45;
    int p2WinThreshold = (p2Difficulty == EASY) ? 20 : 45;
    int p1TotalScore = p1Score + (p1SpecialScore * 3);
    int p2TotalScore = p2Score + (p2SpecialScore * 3);
    bool p1Win = p1TotalScore >= p1WinThreshold;
    bool p2Win = p2TotalScore >= p2WinThreshold;
    
    // For single player mode
    if (!twoPlayerMode) {
      // Regular single player result screen
      if (p1Win) {
        tft.setTextColor(GREEN);
        tft.setCursor(50, 80);
        if (gameVariant == BONK_PATRIOTS) {
          tft.println("FREEDOM");
          tft.setCursor(90, 120);
          tft.println("ACHIEVED!");
        } else {
          tft.println("YOU SHREDDED");
          tft.setCursor(120, 120);
          tft.println("GNAR!");
        }
        tft.fillCircle(240, 200, 30, YELLOW);
        tft.fillCircle(225, 190, 5, BLACK);
        tft.fillCircle(255, 190, 5, BLACK);
        for (int i = 0; i < 21; i++) {
          int x = 230 + i;
          int y = 205 + sin((i - 10) * 0.1) * 6;
          tft.fillCircle(x, y, 2, BLACK);
        }
      } else {
        tft.setTextColor(RED);
        tft.setCursor(70, 80);
        if (gameVariant == BONK_PATRIOTS) {
          tft.println("FREEDOM");
          tft.setCursor(70, 120);
          tft.println("PENDING...");
        } else {
          tft.println("GNAR");
          tft.setCursor(70, 120);
          tft.println("UN-SHREDDED");
        }
        tft.fillCircle(240, 200, 30, RED);
        tft.fillCircle(225, 190, 5, BLACK);
        tft.fillCircle(255, 190, 5, BLACK);
        for (int i = 0; i < 21; i++) {
          int x = 230 + i;
          int y = 205 + sin((i - 10) * 0.1) * 6;
          tft.fillCircle(x, y, 2, BLACK);
        }
      }
      
      // Display score information
      String scoreLabel = (gameVariant == BONK_CHICKEN) ? "BONKS: " : 
                         (gameVariant == BONK_BUNNY) ? "HOPS: " : 
                         (gameVariant == BONK_PATRIOTS) ? "FLAGS: " : "TAPS: ";
      String specialLabel = (gameVariant == BONK_CHICKEN) ? "SPECIAL: " : 
                           (gameVariant == BONK_BUNNY) ? "EGGS: " : 
                           (gameVariant == BONK_PATRIOTS) ? "STARS: " : "BALLOONS: ";
      
      tft.setTextColor(WHITE);
      tft.setCursor(120, 240);
      tft.print(scoreLabel);
      tft.print(p1Score);
      tft.setTextColor(CYAN);
      tft.setCursor(120, 270);
      tft.print(specialLabel);
      tft.print(p1SpecialScore);
      
      tft.setTextColor(GREEN);
      tft.setCursor(120, 300);
      tft.print("Total: ");
      tft.print(p1TotalScore);
      
      // Game mode information
      tft.setTextSize(1);
      tft.setCursor(10, 300);
      if (p1Difficulty == EASY) {
        tft.setTextColor(GREEN);
        tft.print("EASY MODE");
      } else {
        tft.setTextColor(RED);
        tft.print("SHRED GNAR MODE");
      }
    } 
    // For two player mode
    else {
      tft.setTextColor(WHITE);
      tft.setCursor(110, 30);
      tft.println("FINAL RESULTS");
      
      // Player 1 results
      tft.setTextColor(GREEN);
      tft.setTextSize(2);
      tft.setCursor(50, 70);
      tft.print("Player 1: ");
      
      if (p1Win) {
        tft.setTextColor(GREEN);
        tft.print("WIN");
        tft.fillCircle(380, 70, 15, GREEN);
      } else {
        tft.setTextColor(RED);
        tft.print("LOSE");
        tft.fillCircle(380, 70, 15, RED);
      }
      
      // Display P1 scores
      String scoreLabel = (gameVariant == BONK_CHICKEN) ? "BONKS: " : 
                         (gameVariant == BONK_BUNNY) ? "HOPS: " : "FLAGS: ";
      String specialLabel = (gameVariant == BONK_CHICKEN) ? "SPECIAL: " : 
                           (gameVariant == BONK_BUNNY) ? "EGGS: " : "STARS: ";
      
      tft.setTextColor(WHITE);
      tft.setCursor(70, 100);
      tft.print(scoreLabel);
      tft.print(p1Score);
      
      tft.setCursor(70, 120);
      tft.print(specialLabel);
      tft.print(p1SpecialScore);
      
      tft.setTextColor(YELLOW);
      tft.setCursor(70, 140);
      tft.print("TOTAL: ");
      tft.print(p1TotalScore);
      
      // Player 2 results
      tft.setTextColor(RED);
      tft.setCursor(50, 180);
      tft.print("Player 2: ");
      
      if (p2Win) {
        tft.setTextColor(GREEN);
        tft.print("WIN");
        tft.fillCircle(380, 180, 15, GREEN);
      } else {
        tft.setTextColor(RED);
        tft.print("LOSE");
        tft.fillCircle(380, 180, 15, RED);
      }
      
      // Display P2 scores
      tft.setTextColor(WHITE);
      tft.setCursor(70, 210);
      tft.print(scoreLabel);
      tft.print(p2Score);
      
      tft.setCursor(70, 230);
      tft.print(specialLabel);
      tft.print(p2SpecialScore);
      
      tft.setTextColor(YELLOW);
      tft.setCursor(70, 250);
      tft.print("TOTAL: ");
      tft.print(p2TotalScore);
      
      // Final winner declaration
      tft.setTextSize(3);
      tft.setCursor(120, 280);
      
      if (p1TotalScore > p2TotalScore) {
        tft.setTextColor(GREEN);
        tft.print("P1 WINS!");
      } else if (p2TotalScore > p1TotalScore) {
        tft.setTextColor(RED);
        tft.print("P2 WINS!");
      } else {
        tft.setTextColor(YELLOW);
        tft.print("TIE GAME!");
      }
    }
  }
  
  // After 5 seconds, return to game select
  if (elapsedTime >= 5000) {
    currentState = GAME_SELECT;
    stateStartTime = currentTime;
    loadScreenPhase = 0;
  }
}

void drawChicken(int x, int y, bool withFlames) {
  tft.fillRect(x, y, currentChickenSize, currentChickenSize, YELLOW);
  tft.fillRect(x+currentChickenSize*2/3, y-5, 8, 10, RED);
  tft.fillRect(x+currentChickenSize/6, y+currentChickenSize/6, 5, 5, BLACK);
  tft.fillRect(x+currentChickenSize*2/3, y+currentChickenSize-5, 15, 5, ORANGE);
  tft.fillRect(x-5, y+currentChickenSize-5, 15, 5, ORANGE);
  tft.drawRect(x-1, y-1, currentChickenSize+2, currentChickenSize+2, RED);
  if (withFlames) {
    drawFlameEffect(x, y, flameEffectIntensity);
  }
}

void drawSpecialChicken(int x, int y) {
  tft.fillRect(x, y, specialChickenSize, specialChickenSize, BLUE);
  tft.fillRect(x+specialChickenSize*2/3, y-3, 6, 8, MAGENTA);
  tft.fillRect(x+specialChickenSize/6, y+specialChickenSize/6, 4, 4, WHITE);
  tft.fillRect(x+specialChickenSize*2/3, y+specialChickenSize-4, 12, 4, PURPLE);
  tft.fillRect(x-4, y+specialChickenSize-4, 12, 4, PURPLE);
  tft.drawRect(x-1, y-1, specialChickenSize+2, specialChickenSize+2, CYAN);
  tft.fillCircle(x-2, y-2, 2, WHITE);
  tft.fillCircle(x+specialChickenSize+2, y-2, 2, WHITE);
  tft.fillCircle(x-2, y+specialChickenSize+2, 2, WHITE);
  tft.fillCircle(x+specialChickenSize+2, y+specialChickenSize+2, 2, WHITE);
}

void drawFlameEffect(int x, int y, int intensity) {
  if (intensity <= 0) return;
  uint16_t flameColors[] = {YELLOW, 0x029F, 0x035F, 0x04FF, RED};
  uint16_t flameColor = flameColors[min(intensity-1, 4)];
  for (int i = 0; i < intensity * 3; i++) {
    int flameX = x + random(-5, currentChickenSize + 5);
    int flameY = y + random(-5, currentChickenSize + 5);
    if ((flameX > x && flameX < x + currentChickenSize && 
         flameY > y && flameY < y + currentChickenSize)) {
      continue;
    }
    int flameSize = random(2, 4 + intensity);
    tft.fillCircle(flameX, flameY, flameSize, flameColor);
  }
}
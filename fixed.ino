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

// Screen dimensions
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 320

// Game states
enum GameState {
  LOAD_SCREEN,
  GAME_SELECT,       // Choose game variant (Chicken/Bunny)
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
  BONK_BUNNY
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
void drawEgg(int x, int y);
void clearCharacter(int x, int y, int size);
bool isTapInCharacter(int tapX, int tapY, int checkX, int checkY, int size);
void checkTouchEvents();
void updateScoreDisplay();
void resetGameVariables();
void preparePlayerOneGame();
void preparePlayerTwoGame();
void savePlayerOneResults();
void drawFlameEffect(int x, int y, int intensity);
void drawSpecialCharacter(int x, int y);
void clearSpecialCharacter(int x, int y);
bool shouldShowSpecialCharacter();

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
  } else {
    tft.print("HOPS: ");
  }
  tft.print(score);
  
  tft.setCursor(240, 30);
  tft.setTextColor(CYAN);
  
  if (gameVariant == BONK_CHICKEN) {
    tft.print("SPECIAL: ");
  } else {
    tft.print("EGGS: ");
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
  
  Serial.print(gameVariant == BONK_CHICKEN ? "BONK! " : "HOP! ");
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
            // Handle game variant selection (BONK Chicken or BONK Bunny)
            if (x > 50 && x < 230 && y > 120 && y < 220) {
              Serial.println("BONK Chicken selected");
              gameVariant = BONK_CHICKEN;
              currentState = PLAYER_SELECT;
              stateStartTime = currentTime;
              tft.fillScreen(BLACK);
            }
            else if (x > 250 && x < 430 && y > 120 && y < 220) {
              Serial.println("BONK Bunny selected");
              gameVariant = BONK_BUNNY;
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
                // Show explosion effects
                for (int i = 0; i < 5; i++) {
                  int explodeX = specialChickenX + random(-20, 20);
                  int explodeY = specialChickenY + random(-20, 20);
                  uint16_t explodeColor = random(3) == 0 ? RED : (random(2) == 0 ? ORANGE : YELLOW);
                  tft.fillCircle(explodeX, explodeY, random(5, 15), explodeColor);
                  delay(20);
                }
                
                // Store the position info for proper clearing later
                int textX = specialChickenX - 20;
                int textY = specialChickenY - 20;
                int textWidth = 100;  // Increased width to ensure full text is cleared
                int textHeight = 40;
                
                // Show "GNAR!" text
                tft.setTextColor(YELLOW);
                tft.setTextSize(4);
                tft.setCursor(textX, textY);
                tft.print("GNAR!");
                
                // Set up a timer to clear this text after a brief delay
                showFrenzyText = true;
                frenzyTextTime = currentTime;
                
                // Store the area to clear in global variables
                lastSpecialChickenX = textX;
                lastSpecialChickenY = textY;
                specialChickenSize = max(textWidth, textHeight); // Reuse this variable to store clearing size
                
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

// New function to handle game variant selection
void handleGameSelection() {
  unsigned long currentTime = millis();
  
  // Draw the screen once when entering this state or refresh periodically
  if ((currentTime - stateStartTime) < 100 || 
      ((currentTime - stateStartTime) % 5000 >= 0 && (currentTime - stateStartTime) % 5000 < 100)) {
    
    Serial.println("Drawing GAME SELECT screen");
    tft.fillScreen(BLACK);
    
    // Title
    tft.setTextColor(WHITE);
    tft.setTextSize(4);
    tft.setCursor(100, 40);
    tft.println("SELECT GAME");
    
    // BONK Chicken option
    tft.drawRect(50, 120, 180, 100, YELLOW);
    tft.drawRect(51, 121, 178, 98, YELLOW);
    tft.fillRect(52, 122, 176, 96, RED);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.setCursor(75, 140);
    tft.println("BONK");
    tft.setCursor(60, 180);
    tft.println("CHICKEN");
    
    // BONK Bunny option
    tft.drawRect(250, 120, 180, 100, WHITE);
    tft.drawRect(251, 121, 178, 98, WHITE);
    tft.fillRect(252, 122, 176, 96, PURPLE);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.setCursor(275, 140);
    tft.println("BONK");
    tft.setCursor(275, 180);
    tft.println("BUNNY");
    
    // Descriptions
    tft.setTextSize(1);
    tft.setTextColor(YELLOW);
    tft.setCursor(75, 230);
    tft.println("Classic Chicken Action!");
    
    tft.setTextColor(CYAN);
    tft.setCursor(270, 230);
    tft.println("Easter Edition with Eggs!");
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
    tft.print(gameVariant == BONK_CHICKEN ? "CHICKEN" : "BUNNY");
    
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
    tft.print(gameVariant == BONK_CHICKEN ? "BONKS: " : "HOPS: ");
    tft.print(p1Score);
    
    tft.setCursor(70, 60);
    tft.print(gameVariant == BONK_CHICKEN ? "SPECIAL: " : "EGGS: ");
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
  } else {
    // Easter egg
    drawEgg(x, y);
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

// Forward declarations for specific player gameplay functions

// Forward declarations of player gameplay functions

================================================================================

   ######   ######  ###    ## ##   ##     ##    ##  #####
   ##   ## ##    ## ####   ## ##  ##      ##    ## ##   ##
   ######  ##    ## ## ##  ## #####       ##    ##  #####
   ##   ## ##    ## ##  ## ## ##  ##       ##  ##  ##   ##
   ######   ######  ##   #### ##   ##       ####    #####

                        *** FLOPPY EDITION ***
                         Version 9.0 - 2025

              "The Only Game That Fits on a 3.5-Inch Disk"

================================================================================

================================================================================
 SECTION 1: WHAT IS THIS FLOPPY?
================================================================================

This 1.44 MB floppy disk contains:

  * BONK.HEX        - Compiled Teensy 4.1 firmware (< 100 KB)
  * AUTOEXEC.BAT    - Auto-flash script for DOS/Windows
  * README.TXT      - This file
  * HIGHSCORES.000  - Your legendary scores
  * BONK.WAV        - 8-bit startup beep (10 KB)

Total: ~135 KB (1.3 MB FREE for expansion!)


================================================================================
 SECTION 2: HARDWARE YOU NEED
================================================================================

+--------------------------------------------------------------------------+
| PART #1: Teensy 4.1 Microcontroller                                     |
| ------------------------------------------------------------------------ |
| * ARM Cortex-M7 @ 600 MHz                                                |
| * 8 MB Flash, 1 MB RAM                                                   |
| * Source: https://www.pjrc.com/store/teensy41.html                       |
| * Price: $28.00                                                          |
+--------------------------------------------------------------------------+

+--------------------------------------------------------------------------+
| PART #2: ILI9488 3.5" TFT Display (480x320)                             |
| ------------------------------------------------------------------------ |
| * 3.5-inch color LCD                                                     |
| * SPI interface                                                          |
| * Includes XPT2046 resistive touch controller                           |
| * AliExpress: Search "ILI9488 3.5 inch TFT SPI"                         |
| * Price: $10-12                                                          |
+--------------------------------------------------------------------------+

+--------------------------------------------------------------------------+
| PART #3: USB Cable (Micro-USB or USB-C depending on Teensy)             |
| ------------------------------------------------------------------------ |
| * For programming and power                                              |
| * Price: $2.00                                                           |
+--------------------------------------------------------------------------+

+--------------------------------------------------------------------------+
| PART #4: Optional - Small Speaker/Piezo Buzzer                          |
| ------------------------------------------------------------------------ |
| * 8 Ohm speaker or piezo buzzer                                          |
| * Connect to Pin 5 for beeps                                             |
| * Price: $1-3                                                            |
+--------------------------------------------------------------------------+

TOTAL COST: ~$40-45 USD


================================================================================
 SECTION 3: WIRING DIAGRAM
================================================================================

+----------------------------------------------------------------------------+
|                    TEENSY 4.1 PINOUT FOR BONK v9                           |
+----------------------------------------------------------------------------+

    +-----------------------------+
    |      TEENSY 4.1 (TOP)       |
    +-----------------------------+
    |  GND ---------------------- GND (TFT)
    |  3.3V --------------------- VCC (TFT)
    |  Pin 11 (MOSI) ------------ SDA/MOSI (TFT)
    |  Pin 12 (MISO) ------------ MISO (TFT)
    |  Pin 13 (SCK) ------------- SCK (TFT)
    |  Pin 20 ------------------- CS (TFT Chip Select)
    |  Pin 21 ------------------- DC (TFT Data/Command)
    |  Pin 22 ------------------- RST (TFT Reset)
    |  Pin 23 ------------------- T_CS (Touch Chip Select)
    |  Pin 24 ------------------- T_IRQ (Touch Interrupt)
    |  Pin 5  ------------------- AUDIO OUT (Speaker +)
    |  GND ---------------------- Speaker -
    +-----------------------------+

+----------------------------------------------------------------------------+
|                    ILI9488 TFT MODULE PINOUT                               |
+----------------------------------------------------------------------------+

    ILI9488 TFT Pin         ->    Teensy 4.1 Pin
    ---------------------------------------------
    VCC                     ->    3.3V
    GND                     ->    GND
    CS  (Display)           ->    Pin 20
    RESET                   ->    Pin 22
    DC  (Data/Command)      ->    Pin 21
    SDI/MOSI                ->    Pin 11
    SCK                     ->    Pin 13
    LED (Backlight)         ->    3.3V (always on)
    SDO/MISO                ->    Pin 12

    T_CLK (Touch)           ->    Pin 13 (shared with SCK)
    T_CS  (Touch)           ->    Pin 23
    T_DIN (Touch)           ->    Pin 11 (shared with MOSI)
    T_DO  (Touch)           ->    Pin 12 (shared with MISO)
    T_IRQ (Touch)           ->    Pin 24

+----------------------------------------------------------------------------+
|                         VISUAL WIRING DIAGRAM                              |
+----------------------------------------------------------------------------+

         +-------------+                     +------------------+
         |  TEENSY 4.1 |                     |  ILI9488 TFT     |
         |             |                     |  (3.5" Display)  |
         |         3.3V|---------------------|VCC               |
         |          GND|---------------------|GND               |
         |      Pin 11 |---------------------|MOSI              |
         |      Pin 12 |---------------------|MISO              |
         |      Pin 13 |---------------------|SCK               |
         |      Pin 20 |---------------------|CS                |
         |      Pin 21 |---------------------|DC                |
         |      Pin 22 |---------------------|RST               |
         |      Pin 23 |---------------------|T_CS              |
         |      Pin 24 |---------------------|T_IRQ             |
         |       Pin 5 |------+              +------------------+
         +-------------+      |
                              | +----------+
                              +-| SPEAKER  |
                                |   8 Ohm  |
                          GND---+          |
                                +----------+


================================================================================
 SECTION 4: HOW TO BONK IN 3 EASY STEPS
================================================================================

+--------------------------------------------------------------------------+
| STEP 1: INSERT FLOPPY AND RUN AUTOEXEC.BAT                              |
| ------------------------------------------------------------------------ |
|                                                                          |
|   1. Insert this floppy into a PC with a floppy drive                   |
|   2. Open DOS prompt or Windows 95/98/XP command line                   |
|   3. Type: A:                                                            |
|   4. Type: AUTOEXEC.BAT                                                  |
|   5. Follow on-screen instructions                                       |
|                                                                          |
|   The script will:                                                       |
|   * Detect your Teensy 4.1 in bootloader mode                           |
|   * Flash BONK.HEX to the Teensy                                         |
|   * Verify successful flash                                              |
|   * Display wiring instructions                                          |
|                                                                          |
+--------------------------------------------------------------------------+

+--------------------------------------------------------------------------+
| STEP 2: WIRE UP YOUR HARDWARE                                           |
| ------------------------------------------------------------------------ |
|                                                                          |
|   1. Unplug Teensy from USB                                              |
|   2. Connect TFT display pins as shown in SECTION 3                     |
|   3. Connect touch controller pins                                       |
|   4. Optional: Connect speaker to Pin 5 and GND                          |
|   5. Double-check all connections                                        |
|                                                                          |
|   TIP: Use a breadboard or solder headers for clean connections         |
|                                                                          |
+--------------------------------------------------------------------------+

+--------------------------------------------------------------------------+
| STEP 3: POWER ON AND BONK!                                              |
| ------------------------------------------------------------------------ |
|                                                                          |
|   1. Connect Teensy to USB power (or 5V power supply)                   |
|   2. Screen should display BONK v9 splash screen                         |
|   3. Hear the startup beep! (if speaker connected)                       |
|   4. Touch screen to navigate menus                                      |
|   5. Choose your game: Chicken, Bunny, or Dragon                         |
|   6. Select 1P or 2P mode                                                |
|   7. Pick difficulty: EASY or HARD                                       |
|   8. TAP AWAY! 45-second rounds of pure bonking chaos!                   |
|                                                                          |
+--------------------------------------------------------------------------+


================================================================================
 SECTION 5: GAME FEATURES
================================================================================

  > 3 GAME VARIANTS:
    * CHICKEN MODE - Classic whack-a-chicken + flame bonuses
    * BUNNY MODE - Easter-themed with egg collectibles
    * DRAGON MODE - How to Train Your Dragon with fish!

  > 2 DIFFICULTY LEVELS:
    * EASY - Slower targets, perfect for beginners
    * HARD - Progressive speed-up, frenzy mode at 5+ streak!

  > 1-PLAYER OR 2-PLAYER MODE:
    * Compete for high scores
    * Each player picks their own difficulty
    * Winner displayed at end!

  > BONUS ITEMS:
    * Flames (Chicken mode)
    * Eggs (Bunny mode)
    * Fish (Dragon mode)

  > VISUAL EFFECTS:
    * Frenzy mode flames (HARD difficulty)
    * Plasma effects (Dragon mode)
    * Score animations

  > AUDIO:
    * PWM beeps for bonks
    * Special item sounds
    * Startup jingle


================================================================================
 SECTION 6: SAVING HIGH SCORES
================================================================================

Want to save your legendary scores back to the floppy?

METHOD 1: Manual Save (DOS/Windows)
  1. After your game, note your score
  2. Insert BONK floppy back into PC
  3. Type: ECHO 1337 >> HIGHSCORES.000
  4. Replace 1337 with your actual score

METHOD 2: View All High Scores
  1. Type: TYPE HIGHSCORES.000
  2. See all saved scores from past games

METHOD 3: Clear High Scores
  1. Type: DEL HIGHSCORES.000
  2. Type: ECHO 0 > HIGHSCORES.000

Example high score file:
  --------------------------------------
  1337 - VONHOLTEN - 10/28/2025 - HARD
  892  - PLAYER2   - 10/28/2025 - EASY
  1999 - L33TG4M3R - 10/29/2025 - HARD
  --------------------------------------


================================================================================
 SECTION 7: TROUBLESHOOTING
================================================================================

PROBLEM: Screen is blank
  SOLUTION: Check 3.3V and GND connections. Verify Pin 20 (CS) is connected.

PROBLEM: Touch not working
  SOLUTION: Check Pin 23 (T_CS) and Pin 24 (T_IRQ). Try recalibrating touch.

PROBLEM: Teensy won't flash
  SOLUTION: Press RESET button on Teensy. Ensure teensy_loader_cli.exe exists.

PROBLEM: No sound
  SOLUTION: Check Pin 5 connection. Verify speaker polarity. Sound is optional!

PROBLEM: Floppy disk read error
  SOLUTION: Clean floppy heads. Try another disk. Copy files to USB stick.

PROBLEM: Wrong touch coordinates
  SOLUTION: Adjust map() values in readTouch() function. See source code.


================================================================================
 SECTION 8: TECHNICAL SPECS
================================================================================

  Microcontroller:   Teensy 4.1 (ARM Cortex-M7 @ 600 MHz)
  Display:           ILI9488 3.5" TFT (480x320 RGB565)
  Touch:             XPT2046 Resistive Touch Controller
  Storage:           Floppy 1.44 MB (BONK.HEX in flash memory)
  Code Size:         ~85 KB compiled
  RAM Usage:         ~40 KB
  Frame Rate:        ~60 FPS
  Input Latency:     ~10ms
  Audio:             PWM on Pin 5 (440 Hz - 2000 Hz)
  Power:             5V USB (300 mA typical)


================================================================================
 SECTION 9: EASTER EGGS & SECRETS
================================================================================

  * Type "DIR" in DOS to see hidden ASCII dragon art (maybe...)
  * Get 10+ bonk streak in HARD mode to trigger FRENZY MODE
  * Dragons have green eyes (Night Fury reference!)
  * Plasma blast effect appears in Dragon frenzy mode
  * Fill the floppy disk completely to see "FLOPPY BONKED!" error


================================================================================
 SECTION 10: CREDITS & LICENSE
================================================================================

  Created by: VonHoltenCodes (Trenton Von Holten)
  GitHub:     https://github.com/VonHoltenCodes/BONK
  Version:    9.0 FLOPPY EDITION
  Year:       2025
  Inspired:   1995 DOS Games, Whack-a-Mole, Retro Computing

  Special Thanks:
    * PJRC for Teensy 4.1 and libraries
    * Adafruit for ILI9488 drivers
    * DreamWorks Animation (How to Train Your Dragon)
    * The floppy disk enthusiasts keeping the dream alive

  License:
    Educational and recreational use permitted.
    Commercial use prohibited without permission.
    Fan-art variants (Bluey, Dragons) used with respect to trademark holders.

  Want to contribute?
    Fork the repo, add features, submit PRs!


================================================================================

                  "BUILT IN 1995, RUNNING IN 2025"

                    ### THANK YOU FOR BONKING! ###

                 Now go forth and BONK with glory!

                          [___]
                          |   |    < 3.5" Floppy
                          |___|    < 1.44 MB of Pure Joy

================================================================================

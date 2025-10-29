================================================================================
  ######   ######  ###    ## ##   ##     ##    ##  #####
  ##   ## ##    ## ####   ## ##  ##      ##    ## ##   ##
  ######  ##    ## ## ##  ## #####       ##    ##  #####
  ##   ## ##    ## ##  ## ## ##  ##       ##  ##  ##   ##
  ######   ######  ##   #### ##   ##       ####    #####

                     *** PURE DOS EDITION v9.5 ***
                          "BONK Natively!"
================================================================================

WHAT IS THIS?
-------------
BONK.EXE is a native DOS whack-a-mole game that runs on ANY PC from 1990-2025!

No Teensy needed - this runs directly on your DOS PC!

SYSTEM REQUIREMENTS:
--------------------
MINIMUM:
  * 386SX CPU or better
  * VGA graphics card
  * MS-DOS 3.3 or later
  * 512 KB RAM
  * MS-DOS Mouse driver (MOUSE.COM)

RECOMMENDED (RetroNobilis):
  * Pentium III or Celeron 800MHz
  * Voodoo3 graphics
  * Sound Blaster Live! or compatible
  * Windows 98 SE (DOS mode)
  * 256 MB RAM

INSTALLATION:
-------------
1. Copy BONK.EXE to your DOS machine
2. Load mouse driver: MOUSE.COM
3. Run: BONK.EXE

For Sound Blaster support:
  * Ensure SET BLASTER environment variable is set
  * Example: SET BLASTER=A220 I5 D1 T4

FILES INCLUDED:
---------------
BONK.EXE       - Main game executable (~80 KB)
MOUSE.COM      - MS-DOS mouse driver (if needed)
README_DOS.TXT - This file

CONTROLS:
---------
MOUSE:    Move cursor and click targets
KEYBOARD: ESC to exit, 1/2/3 to select game mode

GAME MODES:
-----------
1. CHICKEN MODE - Classic bonk-the-chicken
2. BUNNY MODE   - Easter bunny bonking
3. DRAGON MODE  - How to Train Your Dragon!

Each mode has EASY and HARD difficulty.

FEATURES:
---------
✓ VGA Mode 13h graphics (320x200, 256 colors)
✓ Mouse-driven gameplay
✓ Sound Blaster audio with digital samples
✓ PC Speaker support (fallback)
✓ Geometric sprite rendering (no bitmaps needed!)
✓ 45-second timed rounds
✓ Progressive difficulty
✓ Frenzy mode in HARD difficulty
✓ Bonus item collection
✓ High score tracking
✓ Fits on a single 1.44 MB floppy!

AUDIO FEATURES:
---------------
Sound Blaster Detected:
  * Digital PCM click sounds
  * Bonk sound effects (different per game mode!)
  * Special collection "sparkle" effect
  * Epic frenzy activation sound
  * Victory fanfare
  * Sad game-over trombone
  * Procedural MOD music in menu

PC Speaker Fallback:
  * Beep-based sound effects
  * Works on ANY PC!

TROUBLESHOOTING:
----------------
"Mouse driver not found!"
  → Load MOUSE.COM before running game
  → Run: MOUSE.COM then BONK.EXE

"No sound"
  → Check Sound Blaster settings
  → Run: SET BLASTER=A220 I5 D1 T4
  → Game will work with PC Speaker

"Runs too fast!"
  → Normal on modern CPUs! Enjoy the speed!
  → Game has frame limiting

"Black screen"
  → Press ESC to exit
  → Check VGA compatibility

PERFORMANCE:
------------
Tested on:
  * 386SX/25:       15-20 FPS (playable!)
  * 486DX/66:       40-50 FPS (smooth!)
  * Pentium 200:    60+ FPS (perfect!)
  * Celeron 800:    60+ FPS (capped, no tearing!)

BUILDING FROM SOURCE:
---------------------
Requirements:
  * DJGPP compiler (recommended)
  * Or Turbo C++ 3.0

Compile:
  make

or manually:
  gcc -Wall -O2 -march=i386 BONK_DOS.C SOUND.C -o BONK.EXE

BOOTABLE FLOPPY:
----------------
Want to boot BONK from a floppy?

1. Format floppy with DOS: FORMAT A: /S
2. Copy BONK.EXE to floppy
3. Copy MOUSE.COM to floppy
4. Create AUTOEXEC.BAT:
   @ECHO OFF
   MOUSE.COM
   BONK.EXE
5. Boot from floppy!

TECHNICAL DETAILS:
------------------
* Written in pure C (no assembly required!)
* Direct VGA framebuffer access (0xA0000)
* Bresenham line/circle algorithms
* Sound Blaster DSP programming
* BIOS timer tick for timing (18.2 Hz)
* MS-DOS mouse interrupt (int 0x33)
* Procedurally generated sound samples
* No external dependencies
* Total executable size: ~80 KB

CREDITS:
--------
Original BONK concept: VonHoltenCodes
DOS Port: VonHoltenCodes (2025)
Tested on: RetroNobilis (Celeron 800, Voodoo3, SB Live!)

Inspired by:
  * 1990s DOS gaming
  * Whack-a-Mole arcade games
  * How to Train Your Dragon

CHANGELOG:
----------
v9.5 (2025-10-29):
  * Initial DOS native port
  * Sound Blaster audio system
  * Three game variants
  * Frenzy mode
  * High score tracking

LICENSE:
--------
Educational and recreational use permitted.
Commercial use prohibited without permission.

================================================================================
          "BUILT IN 1995, RUNNING IN 2025... FOR REAL THIS TIME!"
================================================================================

For updates and source code:
https://github.com/VonHoltenCodes/BONK

Press any key to exit this README...

@ECHO OFF
CLS
ECHO.
ECHO  ================================================================
ECHO.
ECHO    ######   ######  ###    ## ##   ##     ##    ##  #####
ECHO    ##   ## ##    ## ####   ## ##  ##      ##    ## ##   ##
ECHO    ######  ##    ## ## ##  ## #####       ##    ##  #####
ECHO    ##   ## ##    ## ##  ## ## ##  ##       ##  ##  ##   ##
ECHO    ######   ######  ##   #### ##   ##       ####    #####
ECHO.
ECHO                    *** FLOPPY EDITION ***
ECHO.
ECHO  ================================================================
ECHO.
ECHO  BONK v9 - Flash Instructions
ECHO.
ECHO  ================================================================
ECHO.
ECHO  This floppy contains BONK.HEX (97 KB) ready to flash!
ECHO.
ECHO  FLASHING OPTIONS:
ECHO.
ECHO  OPTION 1 - Teensy Loader (Recommended):
ECHO  ----------------------------------------
ECHO  1. Download Teensy Loader from: www.pjrc.com/teensy/loader.html
ECHO  2. Run Teensy Loader application
ECHO  3. Click: File -^> Open HEX File
ECHO  4. Select BONK.HEX from this floppy
ECHO  5. Plug in Teensy 4.1 via USB
ECHO  6. Click the Program button (or it auto-programs!)
ECHO  7. Done!
ECHO.
ECHO  OPTION 2 - Arduino IDE:
ECHO  -----------------------
ECHO  1. Copy BONK.HEX to your hard drive (C:\BONK.HEX)
ECHO  2. Open Arduino IDE with Teensyduino installed
ECHO  3. Tools -^> Board -^> Teensy 4.1
ECHO  4. Sketch -^> Upload Using Programmer
ECHO  5. Select BONK.HEX when prompted
ECHO.
ECHO  ================================================================
ECHO.
ECHO  AFTER FLASHING:
ECHO.
ECHO  1. Unplug Teensy from USB
ECHO  2. Wire TFT display (see README.TXT for pinout)
ECHO  3. Power on Teensy
ECHO  4. Touch screen and BONK AWAY!
ECHO.
ECHO  ================================================================
ECHO.
ECHO  Files on this floppy:
ECHO.
DIR /W
ECHO.
ECHO  ================================================================
ECHO.
ECHO  See README.TXT for complete wiring instructions!
ECHO.
ECHO  * BUILT IN 1995, RUNNING IN 2025 *
ECHO.
PAUSE

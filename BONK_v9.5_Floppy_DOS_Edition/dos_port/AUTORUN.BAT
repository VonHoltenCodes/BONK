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
ECHO                    *** PURE DOS EDITION v9.5 ***
ECHO.
ECHO  ================================================================
ECHO.
ECHO  Welcome to BONK - Native DOS Gaming!
ECHO.
ECHO  Loading mouse driver...
ECHO.

REM Load mouse driver if it exists
IF EXIST MOUSE.COM GOTO MOUSE_OK
IF EXIST C:\WINDOWS\COMMAND\MOUSE.COM C:\WINDOWS\COMMAND\MOUSE.COM
IF EXIST C:\DOS\MOUSE.COM C:\DOS\MOUSE.COM
IF EXIST C:\MOUSE\MOUSE.COM C:\MOUSE\MOUSE.COM
ECHO Mouse driver loaded.
:MOUSE_OK

ECHO.
ECHO  Starting BONK.EXE...
ECHO.
ECHO  * Mouse controls
ECHO  * VGA graphics
ECHO  * Sound Blaster audio
ECHO  * Press ESC to exit
ECHO.
PAUSE

REM Run the game
BONK.EXE

ECHO.
ECHO  Thanks for playing BONK!
ECHO.
ECHO  * BUILT IN 1995, RUNNING IN 2025 *
ECHO.

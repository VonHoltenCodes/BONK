/*
 * sprites.h - Sprite definitions for BONK v9 FLOPPY EDITION
 *
 * BONK uses ENHANCED GEOMETRIC SPRITES - no bitmaps needed!
 * All characters are drawn using TFT drawing primitives (circles, triangles, etc.)
 * This approach saves massive amounts of memory while providing detailed graphics.
 *
 * Sprites ported from REV8.0 Dragons with enhanced details:
 * - Toothless the Night Fury with green eyes, wings, tail fins, and plasma blast
 * - Detailed chicken with 3-part comb, wattle, wings, and expressive eyes
 * - Cute bunny with pink inner ears, whiskers, fluffy tail, and paws
 * - Enhanced special items: flickering flames, spotted eggs, scaled fish
 */

#ifndef SPRITES_H
#define SPRITES_H

#include <Arduino.h>

// All sprites are drawn procedurally - no bitmap data needed!

// Version info
#define BONK_VERSION "9.0"
#define BONK_EDITION "FLOPPY"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

// ASCII Art for boot screen
const char BONK_LOGO[] PROGMEM = R"(
   ██████   ██████  ███    ██ ██   ██
   ██   ██ ██    ██ ████   ██ ██  ██
   ██████  ██    ██ ██ ██  ██ █████
   ██   ██ ██    ██ ██  ██ ██ ██  ██
   ██████   ██████  ██   ████ ██   ██
)";

const char FLOPPY_ART[] PROGMEM = R"(
    ╔═══════════╗
    ║  3.5"     ║
    ║  [_____]  ║
    ║  |     |  ║
    ║  |_____|  ║
    ║  1.44 MB  ║
    ╚═══════════╝
)";

#endif // SPRITES_H

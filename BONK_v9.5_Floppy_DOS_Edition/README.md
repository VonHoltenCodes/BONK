# BONK v9.5 - FLOPPY & DOS EDITION 🎮💾

## The Dual-Platform Retro Gaming Experience

BONK v9.5 brings the whack-a-mole madness to **TWO platforms**:
1. **Teensy 4.1** with ILI9488 touchscreen (Embedded)
2. **Native MS-DOS** on x86 PCs (Real Hardware!)

Both versions fit on a **single 1.44 MB floppy disk** with room to spare!

---

## 📂 Project Structure

```
BONK_v9.5_Floppy_DOS_Edition/
├── teensy_source/          # Teensy 4.1 firmware (Arduino/C++)
│   ├── BONK_v9_Floppy.ino  # Main Teensy sketch
│   └── sprites.h           # Sprite definitions
├── dos_port/               # Native DOS version (Pure C)
│   ├── BONK_DOS.C          # Main game code
│   ├── SOUND.C             # PC Speaker audio system
│   ├── MAKEFILE            # Build configuration
│   ├── AUTORUN.BAT         # Auto-launcher
│   ├── README_DOS.TXT      # DOS user manual
│   └── COMPILE.TXT         # Compilation guide
├── floppy_files/           # Files for 1.44MB floppy
│   ├── BONK.HEX            # Teensy firmware
│   ├── AUTOEXEC.BAT        # Windows 98 instructions
│   └── README.TXT          # Floppy documentation
└── documentation/          # Project docs
    ├── PROJECT_COMPLETE_SUMMARY.txt
    └── DOS_PORT_SUMMARY.md
```

---

## 🎯 Two Versions, Same Gameplay

### Teensy Edition (Embedded)
- **Hardware**: Teensy 4.1 + ILI9488 3.5" TFT touchscreen
- **Graphics**: 480x320 resolution, 60 FPS
- **Controls**: Resistive touchscreen
- **Audio**: PWM beeps on Pin 5
- **Size**: 97 KB HEX file
- **Cost**: ~$40 in parts

### DOS Edition (Native x86)
- **Hardware**: Any PC from 1990-2025!
- **Graphics**: VGA Mode 13h (320x200, 256 colors)
- **Controls**: MS-DOS mouse driver
- **Audio**: PC Speaker (no sound card needed!)
- **Size**: 158 KB EXE
- **Cost**: FREE (if you have a retro PC!)

---

## 🎮 Game Features

### Three Game Variants
1. **Chicken Mode** - Classic bonk-the-chicken
2. **Bunny Mode** - Easter bunny bonking
3. **Dragon Mode** - How to Train Your Dragon!

### Two Difficulty Levels
- **EASY**: Slower targets, longer display time
- **HARD**: Fast targets, Frenzy mode unlocks at 5+ streak!

### Special Features
- Frenzy Mode (HARD difficulty, 5+ bonk streak)
- Special collectibles (flames, eggs, fish)
- Progressive difficulty
- Score tracking
- 45-second timed rounds
- **Infinite replay** - play again without restarting!

---

## 🔊 Audio System (DOS Edition)

All sounds are **procedurally generated** using the PC Speaker:

- **Welcome Jingle** - C-E-G-C melody on title screen
- **Character Select** - Different beep per character
- **Bonk Sounds** - 1000 Hz confirmation
- **Special Collect** - Rising tones (1500→2000 Hz)
- **Game Over** - Descending sad trombone (800→600→400 Hz)
- **Menu Effects** - Selection confirmations

No WAV files needed - all audio synthesized in real-time!

---

## 🛠️ Compiling the DOS Version

### Option 1: DJGPP (Modern, Cross-Platform)
```bash
cd dos_port
make
```

Or manually:
```bash
i586-pc-msdosdjgpp-gcc -Wall -O2 -march=i386 -s BONK_DOS.C SOUND.C -o BONK.EXE -lm
```

### Option 2: Turbo C++ 3.0 (Period-Correct!)
```
tcc -mc -O -eBONK.EXE BONK_DOS.C SOUND.C
```

### Option 3: Open Watcom (Free!)
```
wcl386 -l=dos4g -ox BONK_DOS.C SOUND.C -fe=BONK.EXE
```

---

## 💿 Running on Real Hardware

### DOS Edition Requirements
**Minimum:**
- 386SX CPU or better
- VGA graphics card
- MS-DOS 3.3 or later
- 512 KB RAM
- MS-DOS mouse driver (MOUSE.COM)

**Recommended (RetroNobilis tested):**
- Celeron 800 MHz or Pentium III
- Voodoo3 graphics (not used, but cool!)
- Sound Blaster Live! (not required - PC Speaker works!)
- Windows 98 SE (DOS mode)
- 256 MB RAM

### Performance Estimates
- **Celeron 800**: 60 FPS (capped, perfect!)
- **Pentium 200**: 60+ FPS
- **486DX/66**: 40-50 FPS (smooth)
- **386SX/25**: 15-20 FPS (playable!)

---

## 🎨 Graphics

Both versions use **geometric sprites** - no bitmaps!

Characters are drawn using:
- Circles (fillCircle)
- Triangles (fillTriangle)
- Lines (drawLine)
- Rectangles (fillRect)

### Enhanced Sprites from REV8.0 Dragons:

**Toothless (Dragon):**
- Green eyes with white shine
- Detailed wings with veining
- Tail fins (red prosthetic!)
- Blue plasma blast effect

**Chicken:**
- 3-part red comb
- Wattle (chin dangles)
- Orange wings and feet
- Layered flame effects

**Bunny:**
- Long ears with pink inner detail
- Whiskers and triangle nose
- Fluffy tail and pink paws

---

## 📊 Technical Stats

### Teensy Edition
- **Source**: 974 lines Arduino C++
- **Compiled**: 97 KB HEX
- **RAM Usage**: ~40 KB (4% of 1 MB)
- **Flash Usage**: ~85 KB (1% of 8 MB)

### DOS Edition
- **Source**: 1,168 lines pure C
- **Compiled**: 158 KB EXE
- **Memory**: Runs in 512 KB!
- **Graphics**: Direct VGA framebuffer access

### Combined Project
- **Total Lines**: 2,142 lines of code
- **Both fit on ONE floppy**: ~226 KB / 1,440 KB (84% free!)

---

## 🚀 Quick Start

### DOS Version
1. Copy files to DOS machine
2. Run: `MOUSE.COM` (if needed)
3. Run: `BONK.EXE`
4. Enjoy!

### Teensy Version
1. Open `teensy_source/BONK_v9_Floppy.ino` in Arduino IDE
2. Select Board: Teensy 4.1
3. Upload firmware
4. Wire up ILI9488 display
5. Power on and play!

---

## 🎖️ Credits

**Created by Trent Von Holten (VonHoltenCodes) - 2025**

### Inspiration
- 1990s DOS gaming golden era
- Whack-a-Mole arcade games
- How to Train Your Dragon

### Special Thanks
- RetroNobilis (Celeron 800, Voodoo3, SB Live!) - Testing platform
- The entire retro computing community

---

## 📜 License

Educational and recreational use permitted.
Commercial use prohibited without permission.

---

## 🌟 Why This is AWESOME

1. **Runs on REAL DOS hardware** from 1990+
2. **No emulation needed** (though works in DOSBox!)
3. **Fits on ONE FLOPPY** with room to spare
4. **Full PC Speaker audio** - no sound card required
5. **Same gameplay** on embedded and x86
6. **Source code included** - learn and modify!
7. **Period-correct technology** - truly "Built in 1995, Running in 2025"

---

## 🔗 More Information

For detailed documentation, see:
- `documentation/PROJECT_COMPLETE_SUMMARY.txt` - Full project overview
- `documentation/DOS_PORT_SUMMARY.md` - DOS version details
- `dos_port/COMPILE.TXT` - Compilation instructions
- `dos_port/README_DOS.TXT` - User manual

---

**BONK v9.5 - Where Retro Gaming Meets Modern Development!** 🎮✨

*"Built in 1995, Running in 2025... FOR REAL!"*

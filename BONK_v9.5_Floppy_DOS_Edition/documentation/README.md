# BONK v9 — FLOPPY EDITION 💾🐉

```
   ██████   ██████  ███    ██ ██   ██     ██    ██  █████
   ██   ██ ██    ██ ████   ██ ██  ██      ██    ██ ██   ██
   ██████  ██    ██ ██ ██  ██ █████       ██    ██  █████
   ██   ██ ██    ██ ██  ██ ██ ██  ██       ██  ██  ██   ██
   ██████   ██████  ██   ████ ██   ██       ████    █████

            *** The Only Game That Fits on a 3.5" Floppy ***
```

## What Is This?

**BONK v9 FLOPPY EDITION** is a fully functional **Whack-a-Mole game** that:
- ✅ Fits entirely on a **1.44 MB 3.5" floppy disk**
- ✅ Auto-flashes to a **Teensy 4.1** microcontroller
- ✅ Runs on a **3.5" ILI9488 touchscreen**
- ✅ Costs **< $40** to build
- ✅ Boots from **DOS/Windows 95** like it's **1995**

This is retro-computing art meets modern embedded systems.

---

## 🎮 Game Features

### 3 Game Variants
- **🐔 BONK Chicken** — Classic whack-a-chicken with flame bonuses
- **🐰 BONK Bunny** — Easter-themed with collectible eggs
- **🐉 BONK Dragons** — *How to Train Your Dragon* with flying fish!

### 2 Difficulty Modes
- **EASY** — Slow targets, great for beginners
- **HARD** — Progressive speed-up, 5+ bonks = **FRENZY MODE** with flames!

### Multiplayer Support
- **1-Player** — Beat your own high score
- **2-Player** — Compete head-to-head (each player picks difficulty!)

### Special Features
- 45-second timed rounds
- Bonus collectibles (flames, eggs, fish)
- Visual effects (frenzy flames, plasma blasts)
- PWM audio beeps
- High score saving to floppy disk

---

## 📦 What's On The Floppy?

```
A:\
├── BONK.HEX         97 KB   (Compiled Teensy 4.1 firmware)
├── AUTOEXEC.BAT     3.2 KB  (Auto-flash DOS script)
├── README.TXT       16 KB   (DOS-compatible ASCII manual)
├── HIGHSCORES.000   ~40 B   (Your legendary scores)
└── BONK.WAV         9.8 KB  (8-bit startup beep)

TOTAL: ~126 KB / 1.44 MB (91% free!)
```

---

## 🛠️ Hardware You Need

| Part | Where to Buy | Price |
|------|--------------|-------|
| **Teensy 4.1** | [PJRC.com](https://www.pjrc.com/store/teensy41.html) | $28 |
| **ILI9488 3.5" TFT + Touch** | AliExpress ("ILI9488 SPI") | $12 |
| **USB Cable** | Any store | $2 |
| **Speaker (optional)** | AliExpress | $1 |
| **TOTAL** | | **~$40** |

---

## 🚀 Quick Start (3 Steps)

### Step 1: Flash The Floppy
```bash
# Insert floppy into PC (or use USB stick)
A:\> AUTOEXEC.BAT

# Script auto-detects Teensy and flashes BONK.HEX
# (Press RESET button on Teensy if needed)
```

### Step 2: Wire The Hardware
Connect your Teensy 4.1 to the ILI9488 display:

```
TEENSY 4.1        ILI9488 TFT
──────────────────────────────
3.3V       →      VCC
GND        →      GND
Pin 11     →      MOSI
Pin 12     →      MISO
Pin 13     →      SCK
Pin 20     →      CS
Pin 21     →      DC
Pin 22     →      RST
Pin 23     →      T_CS (Touch)
Pin 24     →      T_IRQ (Touch)
Pin 5      →      Speaker + (optional)
```

Full wiring diagram in `floppy/README.TXT`

### Step 3: BONK!
1. Power on Teensy via USB
2. Hear startup beep 🎵
3. Touch screen to navigate menus
4. Choose game, players, difficulty
5. **BONK AWAY FOR 45 SECONDS OF GLORY!**

---

## 📁 Repository Structure

```
BONK-Floppy-v9/
├── source/
│   ├── BONK_v9_Floppy.ino    # Main Arduino code
│   └── sprites.h              # Sprite header
│
├── floppy/                    # Ready to copy to floppy disk!
│   ├── BONK.HEX               # Compiled Teensy firmware
│   ├── AUTOEXEC.BAT           # Auto-flash script
│   ├── README.TXT             # DOS-compatible manual
│   ├── HIGHSCORES.000         # Score storage
│   └── BONK.WAV               # Startup sound
│
├── tools/
│   ├── compress_sprites.py    # RLE compression utility
│   ├── generate_bonk_wav.py   # WAV generator
│   └── generate_dummy_hex.py  # Test HEX generator
│
├── README.md                  # This file
├── COMPILATION_GUIDE.md       # How to compile from source
└── LICENSE                    # Educational use license
```

---

## 🔧 Compiling From Source

Want to modify the game? Compile your own `BONK.HEX`:

### Requirements
- Arduino IDE 2.3+
- Teensyduino
- ILI9488_t3 library (included with Teensyduino)

### Steps
```bash
# 1. Open source/BONK_v9_Floppy.ino in Arduino IDE
# 2. Set board: Tools → Board → Teensy 4.1
# 3. Set CPU: 600 MHz
# 4. Click Verify
# 5. Sketch → Export Compiled Binary
# 6. Copy .hex file to floppy/ directory
```

See **COMPILATION_GUIDE.md** for full details.

---

## 🎯 Performance Specs

| Metric | Value |
|--------|-------|
| **Code Size** | ~85 KB (compiled) |
| **RAM Usage** | ~40 KB |
| **Frame Rate** | ~60 FPS |
| **Touch Latency** | ~10 ms |
| **Audio Range** | 440 Hz - 2000 Hz (PWM) |
| **Power Draw** | 300 mA @ 5V |

---

## 🎨 Enhanced Geometric Sprites

BONK Floppy Edition uses **procedural geometric sprites** instead of bitmaps - saving massive amounts of memory while providing detailed, professional-looking graphics!

**Ported from BONK REV8.0 Dragons:**
- **Toothless the Night Fury**: Green eyes with shine, detailed wings with veining, tail fins (red prosthetic!), side fins, and blue plasma blast effect
- **Detailed Chicken**: 3-part red comb, wattle, expressive eyes with white highlights, orange wings, feet, and layered flame effects
- **Cute Bunny**: Long ears with pink inner ear detail, whiskers, triangle nose, eye shine, fluffy tail, pink paws, and sparkle effects
- **Special Items**:
  - Flames: Multi-layered with flickering tips (red/orange/yellow/white)
  - Easter Eggs: White with colorful spots (magenta/cyan/green/yellow)
  - Fish: Cyan body with scales, detailed tail, eye with shine, magical sparkles

All drawn with TFT primitives (fillCircle, fillTriangle, drawLine) - **zero bitmap data!**

---

## 💾 Creating A Bootable Floppy

### Windows 95/98/XP
```bash
FORMAT A:
COPY floppy\* A:\
```

### Modern Windows (USB Floppy)
1. Use [WinImage](http://www.winimage.com/)
2. Create 1.44 MB floppy image
3. Copy `floppy/*` to image
4. Write to physical floppy

### Linux
```bash
sudo fdformat /dev/fd0
sudo mkfs.msdos /dev/fd0
sudo mount /dev/fd0 /mnt/floppy
sudo cp floppy/* /mnt/floppy/
sudo umount /mnt/floppy
```

### No Floppy Drive?
**Copy files to USB stick!** Works exactly the same.

---

## 🐉 Easter Eggs

- Get **10+ bonk streak** in HARD mode → **FRENZY MODE** with flames
- Dragons have **green eyes** (Night Fury reference!)
- **Plasma blast** effect appears in dragon frenzy mode
- Fill floppy completely → See **"FLOPPY BONKED!"** error
- Type `DIR` in DOS... maybe there's ASCII art hidden? 👀

---

## 🎨 Screenshots

*Coming soon — take photos of your build and submit a PR!*

---

## 🤝 Contributing

Want to add features?

1. Fork this repo
2. Create your feature branch (`git checkout -b feature/LaserChickens`)
3. Commit your changes
4. Push and submit a Pull Request

Ideas:
- More game variants (Space Invaders chickens?)
- Network multiplayer (ESP32 WiFi?)
- Save scores to SD card
- High score leaderboard on TFT
- Boss battle mode

---

## 📜 License

**Educational and Recreational Use**

- ✅ Personal projects
- ✅ Classroom demos
- ✅ Maker fairs
- ✅ Retro computing museums
- ❌ Commercial use without permission

Fan-art variants (Dragons, Bluey) used with respect to trademark holders:
- *How to Train Your Dragon* © DreamWorks Animation
- Character designs are parodies for educational purposes

---

## 🙏 Credits

**Created by:** [VonHoltenCodes](https://github.com/VonHoltenCodes) (Trenton Von Holten)

**Inspired by:**
- 1990s DOS games
- Whack-a-Mole arcade classics
- Retro computing enthusiasts
- The floppy disk collectors keeping history alive

**Special Thanks:**
- **PJRC** for Teensy 4.1 and Teensyduino
- **Adafruit** for ILI9488 drivers
- **DreamWorks** for inspiring dragon designs
- **You** for keeping retro computing alive!

---

## 🌟 Show Your Build!

Built your own BONK machine? Share it!

- Tweet photos with `#BONKv9` and `#FloppyEdition`
- Post to [r/retrobattlestations](https://reddit.com/r/retrobattlestations)
- Submit photos via GitHub Issues → we'll add a gallery!

---

## 📞 Support

- 🐛 **Bugs?** Open an [issue](https://github.com/VonHoltenCodes/BONK/issues)
- 💬 **Questions?** Check `floppy/README.TXT` troubleshooting section
- 🛠️ **Wiring help?** See pinout diagram in README.TXT

---

## 🚀 What's Next?

Future plans for BONK v10:
- [ ] SD card high score persistent storage
- [ ] WiFi leaderboards (ESP32 variant)
- [ ] More game modes (Boss battles, survival mode)
- [ ] Custom sprite uploader via floppy
- [ ] VGA output mode (for CRT monitors!)
- [ ] Mechanical score counter (solenoid-driven!)

---

```
╔══════════════════════════════════════════════════════════╗
║                                                          ║
║            "BUILT IN 1995, RUNNING IN 2025"              ║
║                                                          ║
║         Thank you for keeping floppy disks alive!        ║
║                                                          ║
║                    [___]                                 ║
║                    |   |    < 3.5" of Pure Joy           ║
║                    |___|                                 ║
║                                                          ║
╚══════════════════════════════════════════════════════════╝
```

---

**⭐ Star this repo if you love retro computing!**

**🍴 Fork it to add your own game variants!**

**💾 BONK AWAY!**

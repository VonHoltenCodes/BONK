# CYBERPUNK THEME GUIDE

```
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
█ ▄▄▀█▄█▄▄▀█▄▄▀█▄▄▀█▄▄▀█▄▄ █▄▄ █▄▄ █▄▄▀
█ ▀▀ ████▀▀▄█▀▀▄█▀▀▄█▄▄▀█▄▄ █▄▄ █▄▄ █▀▀▄
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
```

**"Welcome to the Grid, Runner..."**

## Overview

This maze game uses a cyberpunk theme inspired by:
- **Cyberpunk 2077** - Neon aesthetic, corporate dystopia
- **Blade Runner** - Tech noir atmosphere
- **Tron** - Grid-based digital world
- **Neuromancer** - Cyberspace navigation
- **Ghost in the Shell** - Corporate espionage

## Color Palette

### Neon Colors (VGA Mode 13h)

| Color | VGA Index | Usage | Cyberpunk Element |
|-------|-----------|-------|-------------------|
| **Neon Cyan** | 11 (LCYAN) | Primary accent | Grid lines, panels |
| **Neon Magenta** | 13 (LMAGENTA) | Secondary accent | Holograms, alerts |
| **Neon Green** | 10 (LGREEN) | Circuit glow | Circuit boards |
| **Neon Pink** | 12 (LRED) | Danger/warnings | Security systems |
| **Cyber Purple** | 5 (MAGENTA) | Data streams | Information flow |
| **Neon Blue** | 9 (LBLUE) | Highlights | Active elements |
| **Void Black** | 0 (BLACK) | Background | Deep cyberspace |
| **Dark Blue** | 1 (BLUE) | Fog/ceiling | Atmospheric haze |

## Wall Types

### 1. Neon Grid Panels (Cyan)
- **ID:** CELL_WALL_NEON_GRID (1)
- **Color:** NEON_CYAN (bright cyan)
- **Description:** Glowing cyan grid panels, like Tron light walls
- **Lore:** Standard corporate partition walls with embedded holographic displays

### 2. Circuit Boards (Green)
- **ID:** CELL_WALL_CIRCUIT (2)
- **Color:** NEON_GREEN (bright green)
- **Description:** Exposed circuit boards with glowing traces
- **Lore:** Server infrastructure walls, humming with electrical activity

### 3. Holographic Barriers (Magenta)
- **ID:** CELL_WALL_HOLOGRAM (3)
- **Color:** NEON_MAGENTA (bright magenta)
- **Description:** Shimmering magenta force fields
- **Lore:** High-security holographic barriers, semi-transparent but impenetrable

### 4. Data Streams (Purple)
- **ID:** CELL_WALL_DATA (4)
- **Color:** CYBER_PURPLE (purple/violet)
- **Description:** Flowing streams of digital data
- **Lore:** Raw data conduits, visualized information highways

## Environment

### Ceiling
- **Color:** COLOR_BLUE (dark blue)
- **Description:** Deep blue cyberspace void
- **Atmosphere:** Creates sense of infinite digital space above

### Floor
- **Color:** VOID_BLACK (black)
- **Description:** Dark grid floor
- **Atmosphere:** Tron-style grid disappearing into darkness

### Fog/Distance
Implements cyberpunk "haze" effect:
- **0-5 units:** Full neon brightness
- **5-10 units:** Dimmed neon (baseColor - 3)
- **10-15 units:** Dark blue atmospheric haze
- **15+ units:** Fade to void black

## Reference Points (Cyberpunk Themed)

### System Points
| ID | Original | Cyberpunk Name | Description |
|----|----------|----------------|-------------|
| 10 | Player Spawn | Neural Jack-In Point | Where runner connects to grid |
| 20 | Exit | Data Extraction Node | Mission completion point |

### Security (Enemies)
| ID | Original | Cyberpunk Name | Description |
|----|----------|----------------|-------------|
| 30 | Enemy 1 | Security Bot Alpha | AI patrol bot, upper sector |
| 31 | Enemy 2 | Security Bot Beta | AI patrol bot, lower sector |
| 32 | Enemy 3 | Security Bot Gamma | Additional patrol unit |

### Items
| ID | Original | Cyberpunk Name | Description |
|----|----------|----------------|-------------|
| 40 | Health | Med-Pack Nanobots | Healing nanomachine injector |
| 41 | Ammo | Energy Cell Depot | Weapon power supply |
| 42 | Key | Access Keycard | Corporate security clearance |

### Interactive
| ID | Original | Cyberpunk Name | Description |
|----|----------|----------------|-------------|
| 50 | Trigger | Security Terminal | Hackable control console |
| 51 | Switch | Override Switch | Emergency system bypass |
| 60 | Door | Corporate Firewall | Digital security barrier |

## Terminology

### Old Term → Cyberpunk Term

| Generic | Cyberpunk | Usage |
|---------|-----------|-------|
| Maze | The Grid / Corporate Cyberspace | "Navigate the grid" |
| Player | Runner / Netrunner | "Welcome, runner" |
| Level | Sector | "Sector 7-G" |
| Spawn | Jack-In | "Neural jack-in point" |
| Exit | Extraction | "Data extraction node" |
| Enemy | Security Bot | "Corporate security AI" |
| Wall | Barrier / Firewall | "Neon barriers" |
| Empty space | Cyberspace | "Open cyberspace" |

## Interface Messages

### Startup
```
========================================
  CYBERPUNK MAZE RUNNER
  >> NEURAL INTERFACE ACTIVE <<
========================================

[ SYSTEM ] Initializing neural uplink...
[ GRID  ] Corporate grid loaded: 24x24 sectors
[ SPAWN ] Jack-in coordinates: (1.5, 1.5)
```

### Reference Points Display
```
========================================
 GRID REFERENCE POINTS - SECTOR 7-G
========================================

 [10] <01,01> Neural Jack-In Point [ONLINE]
 [20] <12,22> Data Extraction Node [ONLINE]
 [30] <19,09> Security Bot Alpha [ONLINE]
```

### Controls
```
========================================
 RUNNER CONTROLS:
========================================
  [UP]    Navigate forward
  [DOWN]  Navigate backward
  [LEFT]  Rotate left
  [RIGHT] Rotate right
  [ESC]   Disconnect from grid
```

### Shutdown
```
========================================
  >> CONNECTION TERMINATED <<
  Neural interface disconnected
  Thank you for running the grid

  VonHoltenCodes 2025
  Cyberpunk Maze Runner v1.0
========================================
```

## Visual Style

### Aesthetic Goals
1. **High Contrast** - Bright neons against dark void
2. **Tech Noir** - Dark, moody, with bursts of color
3. **Grid-Based** - Geometric, structured, digital
4. **Glowing Elements** - Everything emits light
5. **Atmospheric Haze** - Blue fog in distance

### Color Theory
- **Cyan + Magenta** - Classic cyberpunk combo (complementary)
- **Green** - Computer/tech association
- **Purple** - Digital mystique, data
- **Black** - Deep space, mystery
- **Blue haze** - Atmospheric depth

## Future Enhancements

### Potential Additions
1. **Scanline effect** - Horizontal lines for CRT feel
2. **Flicker** - Slight brightness variation on neons
3. **Grid floor** - Tron-style floor grid lines
4. **Particles** - Floating data bits
5. **HUD** - Cyberpunk-styled interface overlay
6. **Glitch effects** - Digital corruption on damage
7. **Neon text** - Glowing status messages
8. **Radar** - Circular grid-based mini-map

### Sound Design (Future)
- **Ambient** - Low electronic hum
- **Footsteps** - Digital echo
- **Walls** - Electrical buzz near circuits
- **Alerts** - Synthesized warning tones
- **Music** - Synthwave / outrun style

## Lore

### Setting: Corporate Sector 7-G
*Year 2089. Neo-Tokyo Corporate Grid.*

You are a freelance netrunner hired to infiltrate Sector 7-G of the MegaCorp mainframe. Your mission: reach the data extraction node and download classified corporate secrets.

Security bots patrol the neon-lit corridors. Firewalls block your path. You'll need keycards, energy cells, and quick reflexes to survive the grid.

**"In cyberspace, nobody hears you crash..."**

---

*This theme guide documents the complete cyberpunk aesthetic of the Von Holten Maze Game.*

**VonHoltenCodes 2025**

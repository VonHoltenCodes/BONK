# Von Holten Maze Game: CYBERPUNK EDITION

```
▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
█ ▄▄▀█▄█▄▄▀█▄▄▀█▄▄▀█▄▄▀█▄▄ █▄▄ █▄▄ █▄▄▀
█ ▀▀ ████▀▀▄█▀▀▄█▀▀▄█▄▄▀█▄▄ █▄▄ █▄▄ █▀▀▄
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
```

**"Welcome to the Grid, Runner..."**

A neon-lit DOS-style 3D maze game using raycasting. Navigate corporate cyberspace, dodge security bots, and extract the data.

## Overview

Inspired by Cyberpunk 2077, Blade Runner, and Tron, this project builds on BONK v9.5 DOS Edition to create a first-person cyberpunk maze explorer with arrow key controls (no mouse).

## Features

- **Raycasting 3D renderer** - Doom/Wolf3D style pseudo-3D
- **Neon cyberpunk aesthetic** - Cyan, magenta, green color scheme
- **Arrow key navigation** - No mouse required
- **4 wall types** - Neon grids, circuits, holograms, data streams
- **Reference point system** - Corporate grid with security bots, terminals, firewalls
- **VGA Mode 13h graphics** - Classic 320x200, 256 colors
- **Pure DOS/C** - Runs on real hardware (386+)
- **Cyberpunk theme** - Corporate espionage in neon-lit cyberspace

## Cyberpunk Maze Structure

The corporate grid uses a 2D system where each cell represents:
- `0` - Empty cyberspace (walkable)
- `1` - **Neon grid panels** (cyan - CELL_WALL_NEON_GRID)
- `2` - **Circuit boards** (green - CELL_WALL_CIRCUIT)
- `3` - **Holographic barriers** (magenta - CELL_WALL_HOLOGRAM)
- `4` - **Data streams** (purple - CELL_WALL_DATA)
- `10` - Neural jack-in point
- `20` - Data extraction node
- `30-39` - Security bot spawns
- `40-49` - Items (nanobots, energy cells, keycards)
- `50-59` - Security terminals
- `60-69` - Corporate firewalls

Example:
```c
// Cyberpunk grid layout
int maze[10][10] = {
  {1,1,1,1,1,1,1,1,1,1},
  {1,10,0,0,2,0,0,0,30,1},  // 10 = jack-in, 30 = security bot
  {1,0,3,0,2,0,3,3,0,1},     // 3 = hologram, 2 = circuit
  {1,0,2,0,0,0,2,0,0,1},
  {1,0,2,4,4,0,2,0,4,1},     // 4 = data stream
  {1,0,0,0,0,0,2,0,20,1},    // 20 = extraction point
  {1,1,1,1,1,1,1,1,1,1}
};
```

## Development Plan

### Phase 1: Core Engine (Current)
- [x] Research raycasting techniques
- [x] Create repository structure
- [x] Design maze data structure with reference points
- [ ] Implement basic raycasting renderer
- [ ] Add arrow key controls
- [ ] Test on DOSBox

### Phase 2: Gameplay
- [ ] Add player movement and collision
- [ ] Implement reference point system
- [ ] Add textures/colors for walls
- [ ] Create level loader

### Phase 3: Polish
- [ ] PC Speaker audio
- [ ] Multiple levels
- [ ] HUD display
- [ ] Win condition

## Technical Details

**Graphics:** VGA Mode 13h (320x200)
**Input:** Keyboard (arrow keys, ESC)
**Audio:** PC Speaker
**Compiler:** DJGPP / Turbo C++
**Target:** MS-DOS 3.3+, 386+ CPU

## Building

```bash
# Using DJGPP
gcc -Wall -O2 -march=i386 maze.c -o MAZE.EXE -lm -s

# Using Turbo C++
tcc -mc -O -eMAZE.EXE maze.c
```

## Inspiration

- **Cyberpunk 2077** - Neon aesthetic, corporate dystopia
- **Blade Runner** - Tech noir atmosphere
- **Tron** - Grid-based cyberspace
- **BONK v9.5 DOS Edition** (VonHoltenCodes) - Technical foundation
- **Wolfenstein 3D / Doom** (id Software) - Raycasting engine
- **Descent** (Parallax Software) - 3D navigation

## Author

Trent Von Holten (VonHoltenCodes) - 2025

## License

Educational and recreational use permitted.

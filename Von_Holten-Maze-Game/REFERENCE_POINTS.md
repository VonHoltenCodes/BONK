# Reference Points System

## Overview

The maze uses a reference point system where cells with values 10 and above represent special locations that can be used for gameplay elements.

## Reference Point ID Ranges

| Range | Purpose | Description |
|-------|---------|-------------|
| 0-9 | Walls/Empty | Basic maze structure |
| 10-19 | Player/System | Player spawn, system triggers |
| 20-29 | Exits/Goals | Level exits, objectives |
| 30-39 | Enemies | Enemy spawn points |
| 40-49 | Items | Pickups and collectibles |
| 50-59 | Triggers | Switches, buttons, interactive |
| 60-69 | Doors | Doors, gates, barriers |
| 70-79 | Reserved | Future: Teleporters |
| 80-89 | Reserved | Future: Moving platforms |
| 90-99 | Reserved | Future: Traps, hazards |

## Current Level 1 Reference Points

### System (10-19)
- **ID 10** - Player Spawn Point
  - Location: (1, 1)
  - Starting position and direction

### Goals (20-29)
- **ID 20** - Level Exit
  - Location: (12, 22)
  - Completing the level

### Enemies (30-39)
- **ID 30** - Enemy Spawn 1
  - Location: (19, 9)
  - Upper right area guard

- **ID 31** - Enemy Spawn 2
  - Location: (10, 17)
  - Lower area guard

### Items (40-49)
- **ID 40** - Health Pickup
  - Location: (11, 5)
  - Restores player health

- **ID 41** - Ammo Pickup
  - Location: (18, 19)
  - Weapon ammunition

- **ID 42** - Key Item
  - Location: (21, 15)
  - Required to unlock door

### Triggers (50-59)
- **ID 50** - Door Trigger/Switch
  - Location: (13, 13)
  - Opens the door at (9, 13)

### Doors (60-69)
- **ID 60** - Locked Door
  - Location: (9, 13)
  - Requires key or trigger

## Usage in Code

```c
/* Check if a cell is a reference point */
if (isReferencePoint(cellValue)) {
    ReferencePoint* ref = getReferencePoint(x, y);
    if (ref != NULL) {
        printf("Found: %s at (%d, %d)\n", ref->name, x, y);
    }
}

/* Check specific reference types */
int cell = getMazeCell(maze, x, y);
if (cell == REF_PLAYER_SPAWN) {
    /* Handle player spawn */
}
if (cell >= REF_ENEMY_1 && cell <= REF_ENEMY_3) {
    /* Handle enemy spawn */
}
```

## Adding New Reference Points

To add a new reference point:

1. **Define in maze.h:**
   ```c
   #define REF_NEW_FEATURE 55
   ```

2. **Add to maze_data.c layout:**
   ```c
   /* In mazeLayout array */
   { 1, 0, 55, 0, 1 }  /* Add at desired position */
   ```

3. **Document in referencePoints array:**
   ```c
   { REF_NEW_FEATURE, x, y, 1, "New Feature" }
   ```

4. **Update MAZE_MAP.txt** with visual indicator

5. **Update this document** with the new ID

## Best Practices

- Keep IDs within their designated ranges
- Always document new reference points
- Use descriptive names in the referencePoints array
- Set `active` flag to 0 for disabled features
- Reserve consecutive IDs for related features (e.g., 30-39 for all enemies)

## Future Expansion Ideas

### Teleporters (70-79)
```c
#define REF_TELEPORT_A_IN   70
#define REF_TELEPORT_A_OUT  71
#define REF_TELEPORT_B_IN   72
#define REF_TELEPORT_B_OUT  73
```

### Moving Platforms (80-89)
```c
#define REF_PLATFORM_START  80
#define REF_PLATFORM_END    81
```

### Traps (90-99)
```c
#define REF_SPIKE_TRAP      90
#define REF_FIRE_TRAP       91
#define REF_ARROW_TRAP      92
```

## Coordinate System

```
     X →
  Y  +--+--+--+
  ↓  |  |  |  |
     +--+--+--+
     |  |  |  |
     +--+--+--+
```

- X increases to the right (columns)
- Y increases downward (rows)
- Origin (0,0) is top-left corner
- All reference points use this coordinate system

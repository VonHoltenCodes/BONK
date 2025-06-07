# Creative Coding Sessions Guide for BONK Variants

## Overview
This guide helps parents and educators create custom BONK game variants with children. Learn how to turn your child's favorite characters into a playable game while teaching programming concepts!

## Pre-Session Preparation

### 1. Environment Setup (Do Before Kids Join)
- **Install all dependencies**: Libraries, board support, drivers
- **Test compile**: Ensure code builds without errors
- **Verify hardware**: Connect devices and test basic functionality
- **Create workspace**: Clean project folder with example code ready

### 2. Project Analysis
- **Understand code structure**: Identify where changes will be made
- **Find customizable elements**: Graphics, text, sounds, colors, game rules
- **Document key variables**: Note which parts kids can safely modify
- **Prepare examples**: Have reference images/ideas ready

## Session Structure

### Phase 1: Introduction (5 minutes)
- Show the working game/project
- Explain what we'll be creating today
- Let them play with the original to understand it

### Phase 2: Planning (10 minutes)
- Brainstorm theme ideas together
- Sketch out new characters/elements on paper
- Choose colors and names
- Make a simple checklist of changes

### Phase 3: Coding Together (30-45 minutes)
- Start with visual changes (colors, text)
- Move to simple logic changes (scores, timing)
- Add new features if time permits
- Test frequently - every small change

### Phase 4: Testing & Polishing (10 minutes)
- Play test the new version
- Fix any bugs together
- Add final touches
- Celebrate the creation!

## Technical Context Template

When returning to a project, save this information:

```markdown
## Project: [Project Name]
## Hardware: [Board type, Display, Sensors]
## Libraries Required:
- Library 1 (version/source)
- Library 2 (version/source)

## Key Files:
- Main code file: [filename]
- Customizable sections: [line numbers/functions]

## Current Status:
- [X] Environment setup complete
- [ ] Test compile successful
- [ ] Hardware connected

## Creative Elements to Modify:
- Characters/sprites: [location in code]
- Colors: [color definitions location]
- Text/messages: [string locations]
- Game parameters: [timing, scores, etc.]
```

## Tips for Success

### Do's:
- Keep original code as backup
- Use version control (git) if possible
- Comment changes with child's name
- Take screenshots of their creation
- Let them type when possible
- Celebrate small victories

### Don'ts:
- Don't skip the planning phase
- Don't make changes too complex
- Don't fix their "mistakes" if harmless
- Don't rush through testing

## Common Customizations by Age

### Ages 7-9:
- Colors and visual themes
- Character names and messages
- Simple number changes (lives, points)
- Sound effects selection

### Ages 10-12:
- New game modes
- Modified scoring systems
- Additional visual effects
- Simple AI behavior changes

### Ages 13+:
- New features and mechanics
- Code structure improvements
- Performance optimizations
- Cross-game integrations

## Troubleshooting Quick Fixes

1. **Won't compile**: Check semicolons and brackets
2. **Display issues**: Verify color values are valid
3. **Touch not working**: Check coordinate boundaries
4. **Game too hard/easy**: Adjust timing variables
5. **Crashes**: Revert to last working version

## Post-Session

- Save their version with their name
- Document what was created
- Take photos/videos of them with their creation
- Plan what to explore next time
- Back up everything!

## BONK-Specific Customization Points

### Easy Changes for Any Age:
1. **Character Drawing Functions**
   - `drawChicken()`, `drawBunny()`, `drawFlag()` - Main characters
   - Create your own like `drawDragon()`, `drawUnicorn()`, etc.

2. **Colors** (Lines 22-33)
   - All color definitions in one place
   - Add new colors like `#define PINK 0xF81F`

3. **Text Labels**
   - "BONKS" → "CATCHES", "ZAPS", "SAVES"
   - "SPECIAL" → "BONUS", "SUPER", "MEGA"

4. **Special Items**
   - Change from eggs/stars to gems/coins/hearts

5. **Sound Effects Text**
   - "GNAR!" → "BOOM!", "ZAP!", "YEAH!"
   - Frenzy mode text customization

### Example Quick Variant Ideas:
- 🦄 **Unicorn Magic**: Tap unicorns, collect rainbows
- 🚀 **Space Adventure**: Tap rockets, collect planets  
- 🐉 **Dragon Quest**: Tap dragons, collect treasure
- 🌮 **Taco Time**: Tap tacos, collect hot sauce
- 🎮 **Minecraft**: Tap creepers, collect diamonds

## Example Context Save

```
PROJECT: BONK Bluey Variant
DATE: 2025-06-07
CHILD: Daisy, Age 9

COMPLETED:
- Created REV 7.0 with Bluey/Bingo characters
- Added red balloons with confetti effect
- Changed scoring to "TAPS"
- Updated all menus for 4 variants

NEXT TIME:
- Add floating balloon animation
- Include Bandit as ultra-rare character
- Add Keepy Uppy mini-game
- Sound effects with Teensy Audio

REMEMBER:
- Fan art disclaimers important
- 2x2 menu grid for 4+ variants
- Test on actual hardware
```
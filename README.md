# Terminal Galaga

<img width="1444" height="765" alt="image" src="https://github.com/user-attachments/assets/98ddc735-9ba4-491a-a137-960938b4ae60" />

A fast-paced, feature-rich Galaga clone built entirely for the terminal using raw ANSI escape codes in C.

Built using Grok4 with love. 

## Features

- **Classic Galaga Gameplay** - Enemy formations, dive attacks, and capture mechanics
- **10 Unique Powerups** - From dual shots to ally drones
- **Combo System** - Chain kills for massive score multipliers (up to 4x!)
- **Special Weapon** - Charge up devastating spread attacks
- **Bonus Stages** - Test your skills for extra points
- **Health System** - Multiple hits before losing a life
- **God Mode** - For testing and fun
- **Beautiful Terminal Graphics** - Full color support with smooth 30 FPS gameplay
- **Cross-Terminal Compatible** - Works on any modern terminal emulator

## Requirements

- GCC or compatible C compiler
- CMake 3.10+
- POSIX-compliant system (Linux, macOS, WSL)
- Terminal with 256-color support
- Minimum 80x24 terminal size

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./build/galaga
```

## Controls

### Basic Controls
- **WASD** or **Arrow Keys** - Move your ship
- **Space** - Shoot
- **Q** or **ESC** - Quit game

### Special Actions
- **G** - Toggle God Mode (invincibility)
- **B** - Use Bomb (if available - clears screen)
- **X** - Fire Special Weapon (when fully charged)

## Gameplay Mechanics

### Lives & Health
- Start with **3 lives**
- Each life has **3 health points**
- Lose health when hit by enemies or bullets
- Brief invincibility after taking damage

### Combo System
Build combos by destroying enemies in quick succession:
- **3+ kills** → 2x score multiplier
- **5+ kills** → 4x score multiplier
- Combo timer resets with each kill
- Miss for 2 seconds and combo breaks

### Special Weapon
- Charges automatically over time
- When full, press **X** to unleash
- Fires 5 super-piercing mega lasers in a spread pattern
- Deals massive damage to enemy formations

### Wave Progression
- Enemies get faster and more aggressive each wave
- Every 3rd wave is a bonus stage
- Increasing difficulty keeps the challenge fresh

## Powerups

Destroy enemies to drop powerups. Collect them to gain temporary abilities!

| Icon | Name | Effect | Color |
|------|------|--------|-------|
| **D** | Dual Shot | Fire two bullets at once | Cyan |
| **S** | Shield | Protect from one hit | Yellow |
| **F** | Speed Boost | Move faster | Orange |
| **M** | Mega Laser | Bullets pierce through enemies | Purple |
| **B** | Bomb | Gain 3 screen-clearing bombs | Red |
| **H** | Homing | Bullets track enemies | Green |
| **L** | Lightning | Damage chains between enemies | White |
| **R** | Reflect Shield | Bounce enemy bullets back | Blue |
| **T** | Time Slow | Slow down enemy movement | Gray |
| **A** | Ally Drone | Companion ship fights with you | Magenta |

### Powerup Stacking
- Multiple powerups can be active simultaneously!
- Same powerup extends duration
- Combine effects for devastating combos

## Enemy Types

- **🐝 Bee** (Green) - Basic enemy, 100 points
- **🦋 Butterfly** (Yellow) - Faster, aggressive dives, 150 points
- **👑 Boss** (Red) - Can capture your ship, 300 points

### Enemy Behaviors
- **Formation Movement** - Enemies move in synchronized patterns
- **Dive Attacks** - Individual enemies break formation to attack
- **Capture Beam** - Boss enemies can capture your ship (shoot them to escape!)
- **Return Flight** - Diving enemies return to formation

## Scoring

- Bee: 100 points
- Butterfly: 150 points
- Boss: 300 points
- Bonus Stage Enemies: 500 points
- **Score multipliers from combos!**

## Tips & Strategy

1. **Build Combos** - Focus fire on tight enemy groups for big multipliers
2. **Save Bombs** - Use them when surrounded or in emergency
3. **Charge Specials** - Perfect for clearing tough formations
4. **Stack Powerups** - Multiple active powerups = unstoppable
5. **Watch Dive Patterns** - Learn to predict enemy attacks
6. **Prioritize Bosses** - They're worth the most points and drop better powerups
7. **Stay Mobile** - Keep moving to avoid enemy bullet patterns

## Technical Details

- **Language**: C (GNU C99)
- **Rendering**: Raw ANSI escape codes with double buffering
- **Frame Rate**: 30 FPS with fixed timestep
- **Input**: Non-blocking keyboard input with key decay timers
- **Colors**: 256-color ANSI palette

### Architecture
- Modular design with separate systems:
  - Terminal rendering and buffer management
  - Input handling with simultaneous key support
  - Entity management (player, enemies, bullets, powerups)
  - Collision detection (AABB)
  - Enemy AI (formations, dives, capture logic)
  - Game state management
  - Bonus stage system

## Development

Built using modern C best practices:
- Elimination of magic numbers
- Descriptive constants and enums
- Clean separation of concerns
- POSIX compliance for portability

## Known Issues

- Requires terminal with proper ANSI support
- Best experience on 80x24 terminal or larger
- Some terminal emulators may have slight rendering differences

## License

This is a personal project created for learning and entertainment.

## Credits

Inspired by the classic Namco arcade game Galaga (1981).

---

**Enjoy the game! Press SPACE to start!** 🚀


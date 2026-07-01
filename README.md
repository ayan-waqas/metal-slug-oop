# Metal Slug (SFML / C++)

A 2D run-and-gun game inspired by *Metal Slug*, built in C++ with SFML. Originally an Object-Oriented Programming course project, expanded well beyond scope — the codebase emphasizes clean OOP design (polymorphism over type-switching, encapsulated state machines, composition-based systems) across roughly 140 classes, and now includes procedural world generation and a self-play AI mode trained with NEAT.

## Features

**Playable Characters**
- Marco, Tarma, Eri, and Germi — each with unique stats, weapon behavior, and a signature powerup (e.g. Marco's mirrored-shot ability)
- Per-character animation managers (idle, walk, knife, shoot, death, spawn)
- Three-stage damage state machine (Normal → Injury → Critical → Dead) with a visual red-hue warning overlay
- Zombie/Mummy transformation states triggered by special enemy attacks

**Fusion Companion**
- A Mahoraga-inspired AI companion, summonable mid-run with `V`
- Arrives beside the active player and assists in combat

**Enemies**
- Rebel Soldier, Shielded Soldier (frontal-block shield), Bazooka Soldier, Grenade Soldier
- Mummy Warrior and Zombie (melee, with crumble/resurrect and blast-death states)
- Martian (two-phase pod → grounded transformation)
- Paratrooper (spawns a random ground enemy on landing)
- Shared `EnemyAIState` hierarchy driving patrol, alert, and attack behavior per enemy type

**Bosses**
- Iron Nokana, HairBuster, and Sea Satan — each with idle/move/retreat state machines and minion-spawning batches
- Ultimate Boss — a composite final boss that cycles between Ground, Aerial, and Aquatic `BossMode` strategies, each with its own weapons, movement, and minion batches

**Vehicles**
- Player vehicles: Metal Slug, Slug Flyer, Slug Mariner, and the form-switching Amphibious Slug (auto-swaps Ground/Aerial/Aquatic form based on terrain — tank on ground, aircraft in air, submarine in water)
- Enemy vehicles: Flying Tara, Bradley M-15, Enemy Sub
- Enter/exit mechanics, per-form weapon loadouts via the `VehicleMode` strategy pattern

**Weapons & Combat**
- Pistol, Heavy Machine Gun, Knife, Rocket Launcher, Flame Shot, Laser Gun, Hand Grenade, Fire Bomb Grenade
- Damage Zone system (`OneShotZone`, `TimedZone`, `ExplosionZone`, `FireZone`) decouples hit detection from weapon logic
- Explosion, fire pool, and muzzle-flash effects

**Collectibles & Interactables**
- Food (Turkey, Fruit) for saturation, dropped weapon pickups, prisoners that release supply crates
- Supply boxes with randomized weapon/grenade rolls

**Grudgeful Enemies**
- Enemies left behind as the camera scrolls past them don't simply despawn — they're marked "grudgeful" and held in reserve
- On a delay, grudgeful enemies reposition toward the player's current location and rejoin combat, discouraging players from outrunning encounters instead of clearing them
- Paired with a Cowardice system that tracks ignored enemies and can trigger a coordinated counterattack if too many pile up unaddressed

**Forks of Fate**
- A rolling state buffer snapshots player, enemy, boss, vehicle, and world state several times per second
- Snapshots are written to disk on a short interval and support a time-reversal replay — the game can rewind recent seconds of play by reading the buffer back and restoring each recorded `CurrentState`
- Used as the backbone for the death-review and "second chance" flow, letting a run be reconstructed frame-by-frame from the most recent checkpoint

**Developer Mode**
- Toggled in-game with `O` + `P`
- Grants all players full immortality and unlocks the complete weapon set (HMG, Rocket Launcher, Flame Shot, Laser Gun) in addition to the starting Knife/Pistol
- Applies live to every active player and persists across level transitions in Survival mode
- Intended for testing weapon balance, animations, and level layout without dying mid-session

**Game Modes**
- **Survival** — Hand-authored levels (1, 2, 3) culminating in a boss fight
- **Campaign** — Infinite procedurally generated world using layered Perlin noise with swappable terrain profiles (Amplified, Normal, Flat); scrolling camera, tile-based collision, chunk streaming
- **Self-Play AI** — A fully working NEAT (NeuroEvolution of Augmenting Topologies) implementation trains an agent to play the game autonomously

**UI**
- Animated main menu with a nested state machine (Play → Survival / Campaign / Self-Play)
- In-game HUD (HP, weapon/ammo, grenades, lives)
- Pause menu

## Architecture

The project follows strict OOP conventions:
- **No type-based dispatch** — behavior is resolved through virtual functions and polymorphism, never `int type` switches or `dynamic_cast`
- **Pure virtual `getType()`-style hooks** overridden per concrete class where identity matters
- **Strategy pattern** for AI (`EnemyAIState`), vehicle forms (`VehicleMode`), boss modes (`BossMode`), and player transformation states (`TransformationState`)
- **Single public entry point** — every entity exposes one `behaviour()` method per frame; internal helpers (`movement()`, `attack()`, `updateAnimations()`, `setAndDraw()`) are `protected`
- **Encapsulation** — no direct field access across class boundaries; getters/setters and friend-free design throughout

### Key Files

| File | Contents |
|---|---|
| `project.h` | All class declarations and shared globals |
| `Entities.cpp` | Base `Entity`, `DamagableEntity`, projectile, and damage zone systems |
| `Players.cpp` | Player soldier base class, inventory, and all four playable characters |
| `Enemies.cpp` | Enemy base class, AI states, and all enemy types |
| `Bosses.cpp` | Boss base class and all boss encounters, including the multi-mode Ultimate Boss |
| `Vehicles.cpp` | Player and enemy vehicle systems, including form-switching logic |
| `Weapons.cpp` | Weapon hierarchy and projectile behavior |
| `Collectables.cpp` | Food, supply boxes, prisoners, and dropped weapons |
| `Levels.cpp` | Map generation (hand-authored and procedural), scrolling, and noise generation |
| `Engines.cpp` | Game loop, state manager, main menu, and animation system |
| `Managers.cpp` | `EntityManager` and `LevelManager` — spawning, lifecycle, and per-frame orchestration |

## Getting Started

### Requirements
- [SFML](https://www.sfml-dev.org/) 2.6.1 (Graphics, Audio, Window, System modules)
- Either Visual Studio (2019+) **or** CMake + a Linux toolchain (GCC/Clang)

### Build — Windows (Visual Studio)
1. Clone the repository
2. Open the solution in Visual Studio
3. Link `sfml-graphics`, `sfml-window`, `sfml-system`, and **`sfml-audio`** (commonly forgotten — missing this causes a runtime crash)
4. Ensure `openal32.dll` is present alongside the executable
5. Build and run

### Build — Linux (CMake / CLion)
1. Install SFML 2.6.1 (Arch: `pacman -S sfml`; note SFML 3.x breaks API compatibility with this project, pin to 2.6.1)
2. Clone the repository
3. ```bash
   mkdir build && cd build
   cmake ..
   make
   ./MetalSlug
   ```
4. Or open the project directly in CLion — it will pick up the existing `CMakeLists.txt`

### Controls

| Key | Action |
|---|---|
| `A` / `D` | Move left / right |
| `W` | Jump |
| `S` | Crouch |
| Mouse | Aim |
| Left Click | Fire current weapon |
| Right Click | Melee (knife) |
| `R` | Reload |
| `F` | Throw hand grenade |
| `E` | Throw fire bomb |
| `Q` | Character powerup |
| `1`–`4` | Switch weapon slot |
| `X` | Enter / exit vehicle |
| `P` | Interact (prisoners, supply boxes) |
| `Z` | Switch active player |
| `V` | Summon Fusion Companion |
| `O` + `P` | Toggle developer mode |
| `Esc` | Quit |

## Assets & Attribution

- Music sourced from [Downloads.khinsider.com](https://downloads.khinsider.com) (Metal Slug OSTs)
- Sound effects from [Pixabay](https://pixabay.com) and [Voicy.network](https://voicy.network)
- Font: PressStart2P (retro pixel UI font)
- Sprite sheets packed and exported with TexturePacker

## Status

Actively in development. See open items in the codebase for current work-in-progress systems (vehicle animations, remaining boss animation sets, and weapon/collectible bug fixes).

## Contributors

Built collaboratively via a Pull → Work → Commit → Push GitHub workflow.

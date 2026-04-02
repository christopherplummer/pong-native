# Pong Native

`pong-native` is a small cross-platform starter project for a native desktop Pong game. The gameplay logic lives in a shared x86_64 NASM assembly core, while each platform folder owns window creation, rendering, timing, and input scaffolding.

The main completed target is Linux. Linux uses SDL2 for windowing, input, and 2D rendering because SDL2 is a practical game-oriented library with a simple event/render loop and wide Linux support. Windows and macOS are included as lightweight scaffolds for future expansion.

## Project Layout

```text
pong-native/
  README.md
  common/
    pong.asm
    pong.h
  linux/
    detect_linux_ui.sh
    main_sdl.c
    build.sh
    run.sh
  windows/
    main.c
  macos/
    main.m
  Makefile
```

## Why The Core Logic Is In Assembly

The assembly core exists to isolate game-state rules from platform code:

- The same `PongState` layout can be shared across Linux, Windows, and macOS frontends.
- Rendering and input remain easy to replace without rewriting game rules.
- The exported functions provide a small, C-compatible ABI that is simple to call from platform code.

This project prioritizes clarity and determinism over clever assembly tricks. The code in `common/pong.asm` uses integer math, fixed movement steps, and explicit field offsets so the interface is easy to understand.

## Why Platform-Specific Scaffolding Exists

Native desktop applications still need OS-facing code for:

- creating windows
- polling input
- pacing frames
- drawing shapes or textures
- integrating with each platform's runtime and toolchain

That logic is intentionally kept outside the assembly core. The Linux frontend demonstrates the pattern by calling the assembly functions each frame and rendering the resulting state with SDL2.

## Linux Framework Choice

Linux does not choose a game framework based only on distro name or desktop shell. A machine may run GNOME, KDE, Xfce, or another desktop while still having completely different development libraries installed.

For this project, the detection logic checks the actual machine environment and installed tooling:

- distro information from `/etc/os-release` when available
- session details from `XDG_CURRENT_DESKTOP`, `DESKTOP_SESSION`, and `XDG_SESSION_TYPE`
- SDL2 availability through `pkg-config --exists sdl2`
- optional GTK and Qt presence for informational output

If SDL2 development files are installed, the Linux target selects SDL2 as the preferred game framework. If SDL2 is missing, the script fails clearly and explains why. This reflects a practical rule: game-oriented libraries matter more than the current desktop shell.

## Current Platform Status

- Linux: playable and intended as the primary target
- Windows: scaffold only
- macOS: scaffold only

## Implemented Game Features

- Pong gameplay with a left paddle, right paddle, and bouncing ball
- player 1 score tracking
- player 2 score tracking
- keyboard control for player 1
- simple AI paddle movement for player 2
- deterministic integer-based movement and collision logic
- round reset after each score while preserving total score
- match end when either player reaches 10
- game-over overlay with winner text, final score, and restart button

## Controls

Linux SDL2 version:

- `W`: move player 1 paddle up
- `S`: move player 1 paddle down
- `Up Arrow`: move player 1 paddle up
- `Down Arrow`: move player 1 paddle down
- `Escape`: quit
- `R`: restart after a match has ended
- window close button: quit
- left mouse click on `Restart`: begin a new game after the match ends

## Scoring Behavior

When the ball leaves the left side of the screen, player 2 scores and the ball resets to the center.

When the ball leaves the right side of the screen, player 1 scores and the ball resets to the center.

Scores remain intact across round resets.

When either player reaches 10 points, the match ends immediately. The Linux UI shows the winner, the final score, and a restart button so a new match can begin without restarting the program.

## AI Behavior

The AI is intentionally simple and beatable. Each update, player 2 compares its paddle center to the ball position and moves one small step toward it when the difference is large enough. It does not predict future ball movement, so it can be outplayed.

## Core API

`common/pong.h` defines `struct PongState` and the exported assembly functions:

```c
void pong_init(struct PongState* state);
void pong_reset_round(struct PongState* state);
void pong_update_player1(struct PongState* state, int direction);
void pong_update_ai_player2(struct PongState* state);
void pong_step(struct PongState* state);
```

Expected behavior:

- `pong_init` sets default dimensions, paddle placement, ball position, velocities, and scores
- `pong_reset_round` centers paddles and ball while keeping scores
- `pong_update_player1` moves player 1 using `-1`, `0`, or `1`
- `pong_update_ai_player2` moves the AI paddle toward the ball
- `pong_step` advances the ball, handles wall and paddle collisions, and updates scores

The SDL frontend reads input, translates it into the simple `direction` value for player 1, calls the assembly functions, and draws the resulting state.

## Required Tools And Dependencies

Linux build requirements:

- `nasm`
- `gcc`
- `make`
- `pkg-config` or `pkgconf`
- SDL2 development packages

## Example Linux Dependency Installation

Debian / Ubuntu:

```bash
sudo apt update
sudo apt install nasm gcc make pkg-config libsdl2-dev
```

Fedora:

```bash
sudo dnf install nasm gcc make pkgconf-pkg-config SDL2-devel
```

Arch Linux:

```bash
sudo pacman -S nasm gcc make pkgconf sdl2
```

openSUSE:

```bash
sudo zypper install nasm gcc make pkg-config SDL2-devel
```

## Build And Run On Linux

From the repository root:

```bash
cd pong-native
make linux
make run-linux
```

Or directly:

```bash
cd pong-native/linux
./build.sh
./run.sh
```

The build script:

1. runs `detect_linux_ui.sh`
2. confirms SDL2 development files are present
3. assembles `common/pong.asm` with NASM
4. compiles and links `linux/main_sdl.c` against SDL2
5. writes the runnable binary to `linux/pong_native`

## Linux Detection Script

`linux/detect_linux_ui.sh` reports:

- distro name when available
- current desktop and session environment variables
- whether SDL2 is available via `pkg-config`
- whether GTK3, GTK4, Qt5, or Qt6 appear to be installed

Its purpose is diagnostic and practical. It does not treat GNOME, KDE, or another desktop as the deciding factor for a game framework. Instead, it checks which development/runtime libraries are actually available and recommends SDL2 when present.

## How The Assembly Core Interacts With SDL

The SDL frontend owns:

- window creation
- frame pacing
- input polling
- score drawing
- paddle/ball rendering

The assembly core owns:

- state initialization
- paddle movement updates
- AI movement
- ball movement
- collision handling
- score changes
- round resets

This separation keeps the platform layer small and makes the core logic reusable.

## Windows And macOS Future Work

The repository includes scaffold entry points for future expansion:

- `windows/main.c` shows where a Win32 or SDL2-based frontend would call the shared assembly logic
- `macos/main.m` shows where an AppKit-based or SDL2-based frontend would do the same

Likely future directions:

- Windows with Win32 + software rendering
- Windows with SDL2
- macOS with AppKit + Metal or OpenGL
- macOS with SDL2

## Troubleshooting

### `pkg-config` cannot find SDL2

Install the SDL2 development package for your distro, then verify:

```bash
pkg-config --exists sdl2 && echo "SDL2 found"
```

### `nasm: command not found`

Install NASM using your distro package manager.

### Linker warnings about executable stack

The assembly source declares a non-executable stack section for ELF builds. Rebuild after ensuring NASM assembled the current source.

### The window opens but input feels too fast or too slow

The game uses a simple fixed-rate loop with deterministic integer movement. Adjust the constants in `linux/main_sdl.c` or the movement constants in `common/pong.asm` if you want a different feel.

## Notes

- Linux is the most complete and runnable target.
- Windows and macOS files are intentionally scaffold-only.
- The assembly code is the actual gameplay core used by the Linux build, not a placeholder.

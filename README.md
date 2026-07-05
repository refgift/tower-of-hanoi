# grok code fast 1
<img src=tohrun.png />

# grok build 0.1

<img src=tohwithgrokbuild.png />

# Tower of Hanoi - C with Curses TUI

Build a complete Tower of Hanoi solver in C that runs in the terminal using the curses library for a text-based user interface (TUI).

### Requirements:
- Implement the classic Tower of Hanoi puzzle with 3 pegs.
- Allow the user to interact with the puzzle through keyboard input in the terminal.
- Use the curses (ncurses) library to render the towers, disks, and any status information.
- The program should be able to solve the puzzle automatically (show the step-by-step moves) and/or let the user solve it manually.
- It must compile and run on Linux/macOS using a standard Makefile.
- Include clear instructions in the code or README on how to build and run it (e.g. `make` and `./toh`).

### Technical Details:
- Language: C
- UI: curses / ncurses
- Build system: Makefile
- The program should be self-contained in a single source file if possible (`toh.c`).

### Goals:
- Create a clean, functional, and visually clear terminal interface.
- This project is intended as a benchmark test for LLM coding capabilities.
- Keep the code reasonably well-structured and commented.

## Build & Run

```sh
make
./toh          # default 5 disks
./toh 8
make run       # build + run
```

`make clean`

**macOS notes**: `brew install ncurses` then try `make LDFLAGS="-L/opt/homebrew/opt/ncurses/lib -lncurses" CFLAGS="-I/opt/homebrew/opt/ncurses/include -O2 -std=c99 -Wall -D_POSIX_C_SOURCE=200809L"`.

## Usage & Controls

- Keys: `1/2/3` (two-key moves: source then dest), `a` (auto-solve from full tower), `u` (undo), `r` (reset), `q` or `ESC` (quit).
- Benchmark mode: run from clean state to solved; elapsed wall time shown on solve.
- Auto solve animates step-by-step; press `q` during solve to abort.
- Manual: you can reassemble a full tower on any peg then press `a`.

## Timing / Benchmark

Elapsed time uses `CLOCK_MONOTONIC`. Reported time includes animation delays (visible demo runs). For pure compute benchmark use small N or modify delay logic.

## Disks

Max 12 (4095 optimal moves). Default 5. CLI arg overrides.



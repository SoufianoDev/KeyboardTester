# KeyboardTester

A simple terminal tool written in C to test if your keyboard keys are working. Press any key and it shows you what was detected, plays a click sound, and highlights anything that might not be responding as expected. Built to run on Linux, macOS, and Windows with no external dependencies beyond the standard OS audio tools.

---

## Features

- Real-time keystroke detection with character display and control-key identification
- Audio feedback on every keypress using platform-native sound APIs
- Animated intro sequence with a blocking welcome sound
- ANSI color-coded ASCII art banner (violet/crimson accent palette)
- Cross-platform support: Linux, macOS, and Windows
- Minimal dependencies - requires only the C standard library and OS system calls
- Graceful session termination via the `ESC` key

---

## Project Structure

```
KeyboardTester/
├── src/
│   └── main.c              # Full application source
├── res/
│   └── audio/
│       ├── yokoso.wav      # Intro sound (played at startup, blocking)
│       └── key_is_working.wav  # Key-click sound effect (async)
└── out/
    └── KeyboardTester      # Compiled binary (Linux/macOS)
```

---

## Requirements

| Platform | Compiler | Audio Dependency |
|----------|----------|------------------|
| Linux    | GCC / Clang | `aplay` (ALSA utils) |
| macOS    | GCC / Clang / Xcode | `afplay` (built-in) |
| Windows  | MSVC / MinGW | `winmm.lib` (built-in) |

> On Linux, ensure the `alsa-utils` package is installed: `sudo apt install alsa-utils`

---

## Installation

Clone the repository:

```bash
git clone https://github.com/SoufianoDev/KeyboardTester.git
cd KeyboardTester
```

---

## Building

### Linux / macOS

```bash
gcc src/main.c -o out/KeyboardTester
```

### Windows (MSVC)

```cmd
cl src/main.c /Fe:out\KeyboardTester.exe
```

### Windows (MinGW)

```cmd
gcc src/main.c -o out/KeyboardTester.exe -lwinmm
```

---

## Usage

Run the compiled binary from the **project root directory** so that relative paths to audio files resolve correctly.

```bash
# Linux / macOS
./out/KeyboardTester

# Windows
out\KeyboardTester.exe
```

Once launched, the application will:

1. Display a colored ASCII art banner
2. Play the intro welcome sound
3. Print a typed animation welcoming the user to keyboard diagnostic mode
4. Begin capturing all keystrokes and printing their detected character or control-key label
5. Play a short audio click on each keypress
6. Exit cleanly when the `ESC` key is pressed

---

## Architecture

The application centers around a single `KeyboardTester` struct that acts as a self-contained object, holding state (`isRunning`) and a table of function pointers:

| Method | Description |
|--------|-------------|
| `init` | Enables ANSI output (Windows) or raw terminal mode (POSIX), prints the banner |
| `playIntroSound` | Plays `yokoso.wav` synchronously, then runs the typed welcome animation |
| `setOnClickListener` | Registers a callback invoked on each key event |
| `run` | The main event loop; reads characters until `ESC` is pressed |
| `playSound` | Plays `key_is_working.wav` asynchronously on each keypress |
| `cleanup` | Prints the goodbye message; raw mode is restored automatically via `atexit` |

Platform-specific branching (`#ifdef _WIN32`, `#elif __APPLE__`, `#else`) is used throughout to select the correct terminal and audio APIs without any external build system.

---

## Audio Files

| File | Purpose | Playback Mode |
|------|---------|---------------|
| `res/audio/yokoso.wav` | Japanese-language intro welcome sound | Synchronous (blocks until complete) |
| `res/audio/key_is_working.wav` | Short click/blip played on every keypress | Asynchronous (non-blocking) |

> The binary must be run from the project root so that `res/audio/` paths resolve correctly.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

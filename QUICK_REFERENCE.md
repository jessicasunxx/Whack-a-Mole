# Quick Reference Card

## Project Files Overview

```
Whack-a-Mole/
├── platformio.ini          ← PlatformIO configuration (libraries, compiler settings)
├── src/
│   └── main.cpp            ← Main Arduino sketch (850+ lines, fully commented)
├── SETUP_GUIDE.md          ← Complete hardware & software setup guide
├── CALIBRATION.md          ← Touch pad calibration & troubleshooting
├── CUSTOMIZATION.md        ← How to modify the game
└── README.md               ← Original project file
```

---

## Essential Commands (VS Code + PlatformIO)

### Build & Upload
| Task | Shortcut | Menu |
|------|----------|------|
| Build | `Ctrl+Alt+B` | PlatformIO: Build |
| Upload | `Ctrl+Alt+U` | PlatformIO: Upload |
| Clean | `Ctrl+Alt+Shift+B` | PlatformIO: Clean |
| Monitor | `Ctrl+Alt+S` | PlatformIO: Monitor |

### Workflow
```
1. Edit code in src/main.cpp
2. Save file (Ctrl+S)
3. Build (Ctrl+Alt+B)
4. Fix any errors shown
5. Connect ESP32 via USB
6. Upload (Ctrl+Alt+U)
7. Verify success in output
```

---

## Pin Quick Reference

```cpp
// Touch Pads (Capacitive Sensors)
#define GPIO_4   (T0)    // Pad 1
#define GPIO_13  (T2)    // Pad 2
#define GPIO_12  (T5)    // Pad 3
#define GPIO_15  (T4)    // Pad 4

// LEDs
#define GPIO_25  // LED 1
#define GPIO_26  // LED 2
#define GPIO_27  // LED 3
#define GPIO_32  // LED 4

// Display (Built-in, no action needed)
// TTGO T-Display 1.14"
// Pre-configured in platformio.ini
```

---

## Game Logic Quick View

### Game States
```
START_SCREEN ──tap pad──> PLAYING ──30 seconds elapsed──> GAME_OVER ──tap pad──> START_SCREEN
```

### Scoring
```
HIT!  = Tapped correct pad within time limit = +1 point
MISS! = Tapped wrong pad OR time expired    = 0 points
```

### Difficulty Progression
```
Time 0-10s:   Mole duration = 1500ms  (EASY)
Time 10-20s:  Mole duration = 1100ms  (MEDIUM)
Time 20-30s:  Mole duration = 800ms   (HARD)
```

---

## Touch Sensitivity Adjustment

### Single Line Change
**File:** `src/main.cpp`  
**Line:** ~25

```cpp
#define TOUCH_THRESHOLD 40   // 20-30 = very sensitive, 60-100 = very stable
```

### Quick Adjustment Guide
| Behavior | Action | New Value |
|----------|--------|-----------|
| Too sensitive (false positives) | INCREASE | 60, 80, 100 |
| Too hard to trigger | DECREASE | 30, 20, 10 |
| Unstable/varies | INCREASE | 80-100 |
| Responsive but stable | KEEP | 40-50 |

---

## Serial Monitor Output Reference

### Startup (Normal)
```
=== Whack-a-Mole Game Starting ===

Calibrating touch pads...
Calibration complete!
Touch pad baseline values:
  Pad 0: 87
  Pad 1: 92
  Pad 2: 88
  Pad 3: 95
```

### During Gameplay
```
Game started!
New mole: 2, duration: 1500 ms
HIT! Score: 1
New mole: 0, duration: 1500 ms
MISS!
New mole: 3, duration: 1200 ms
HIT! Score: 2
```

### What It Means
- **Baseline values 50-250:** Normal
- **Baseline values < 30:** Weak signal, check connections
- **Baseline values > 500:** Strong signal, may be noisy
- **Calibration complete!:** Ready to play
- **HIT! Score: X:** Correct pad tapped
- **MISS!:** Wrong pad or timeout

---

## Troubleshooting Flowchart

```
START
  │
  ├─ Code won't compile?
  │  └─ Check error message in "PROBLEMS" tab
  │     (Usually: missing semicolon, typo in pin name)
  │
  ├─ Device won't upload?
  │  └─ ESP32 connected via USB?
  │     ├─ NO: Connect it
  │     └─ YES: Try different USB port or cable
  │
  ├─ Display shows garbage?
  │  └─ Try tft.setRotation(0,1,2,3) to rotate
  │
  ├─ Touch pads don't work?
  │  └─ Check Serial Monitor for baseline values
  │     ├─ Shows reasonable values (50-250)?
  │     │  └─ Adjust TOUCH_THRESHOLD
  │     └─ Shows 0 or very high?
  │        └─ Check wire connections
  │
  ├─ LEDs don't light up?
  │  └─ Check that resistor is connected
  │     └─ Try different GPIO pin
  │
  └─ Game feels wrong?
     └─ Read CALIBRATION.md
        └─ Adjust one parameter at a time
```

---

## Most Common Adjustments

### 1. Increase Game Speed (Make It Harder)
**File:** `src/main.cpp`  
**Function:** `getGameDifficulty()` (~line 305)  
**Change:**
```cpp
// From:
return 1500, 1100, 800;
// To:
return 1200, 900, 600;
```

### 2. Make Game Longer
**File:** `src/main.cpp`  
**Function:** `updateGame()` (~line 190)  
**Change:**
```cpp
// From:
if (elapsedTime >= 30000) {   // 30 seconds
// To:
if (elapsedTime >= 60000) {   // 60 seconds
```

### 3. Adjust Touch Sensitivity (Fix Touchpads)
**File:** `src/main.cpp`  
**Line:** ~25  
**Change:**
```cpp
#define TOUCH_THRESHOLD 40  // Try 60 if too sensitive, try 20 if not sensitive enough
```

### 4. Change Display Colors
**File:** `src/main.cpp`  
**Function:** `drawGameScreen()` (~line 370)  
**Change:**
```cpp
// From:
tft.setTextColor(TFT_GREEN, TFT_BLACK);
// To any of:
TFT_RED, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_ORANGE, TFT_WHITE, TFT_MAGENTA
```

---

## Hardware Check Checklist

Before troubleshooting, confirm:

```
Copper Tape Pads:
□ Cut to ~2cm x 2cm
□ Firmly adhered to surface
□ Wire with good contact (no corrosion)
□ Wire connected to correct GPIO (4, 13, 12, 15)
□ All GND connections together

LEDs:
□ Longest leg (anode) to GPIO through resistor
□ Shortest leg (cathode) to GND
□ Resistor value 220-330Ω
□ LEDs face correct direction

Display:
□ Built-in to TTGO T-Display (no setup needed)
□ Should show image on startup

Power:
□ ESP32 connected via USB
□ LED on ESP32 indicates power
□ No burning smell (indicates short circuit)
```

---

## File Locations in Code

### Where to find what:

| What | File | Location |
|------|------|----------|
| Pin definitions | src/main.cpp | Lines 3-18 |
| Game state enum | src/main.cpp | Lines 23-28 |
| Global variables | src/main.cpp | Lines 30-56 |
| Touch calibration | src/main.cpp | Lines 103-123 |
| Game update logic | src/main.cpp | Lines 203-248 |
| Display rendering | src/main.cpp | Lines 343-395 |
| Difficulty setting | src/main.cpp | Lines 305-323 |
| LED control | src/main.cpp | Lines 324-341 |

---

## Before Asking for Help

1. **Check:** Is the device powered? (LED on ESP32 should be on)
2. **Read:** Check Serial Monitor for error messages
3. **Try:** Re-upload the code (press Ctrl+Alt+U)
4. **Review:** Check if any hardware connections are loose
5. **Search:** Read SETUP_GUIDE.md and CALIBRATION.md

---

## Resources

- **PlatformIO Docs:** https://docs.platformio.org/
- **TFT_eSPI Library:** https://github.com/Bodmer/TFT_eSPI
- **ESP32 Pinout:** https://randomnerdtutorials.com esp32-pinout-reference-gpios/
- **TTGO T-Display:** Search GitHub for "TTGO T-Display ESP32" examples

---

## Keyboard Shortcuts Cheat Sheet

```
Ctrl+S          Save current file
Ctrl+K Ctrl+0   Fold all code sections
Ctrl+K Ctrl+J   Unfold all code sections
Ctrl+F          Find text in file
Ctrl+H          Find and replace
Ctrl+G          Go to line number
Ctrl+/          Toggle comment on selected lines

PlatformIO Specific:
Ctrl+Alt+B      Build project
Ctrl+Alt+U      Upload to device
Ctrl+Alt+S      Serial monitor
Ctrl+Alt+D      Open debugging
```

---

**Happy coding! Reference this guide while developing and customizing your game.** 🎮

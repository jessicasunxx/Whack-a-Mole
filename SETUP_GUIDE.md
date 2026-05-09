# Whack-a-Mole Game - Complete Setup Guide

## Project Overview

An interactive Whack-a-Mole game built with an ESP32 TTGO T-Display microcontroller, capacitive touch sensors, LEDs, and a TFT display. This is a fully functional embedded systems project that demonstrates real-time game logic, sensor input handling, and UI design.

---

## Hardware Requirements

### Main Components
- **ESP32 TTGO T-Display** - Microcontroller with built-in 1.14" TFT display
- **4 Copper Tape Touch Pads** - DIY capacitive sensors (~2cm x 2cm each)
- **4 LEDs** - One for each mole target
- **4 Current Limiting Resistors** - 220-330Ω (for LED protection)
- **Breadboard & Jumper Wires** - For connections
- **USB Cable** - For programming and power

### Pin Configuration

```
CAPACITIVE TOUCH PADS:          LEDs:                DISPLAY (Built-in):
─────────────────────          ────                 ───────────────────
├─ Pad 1: GPIO 4               ├─ LED 1: GPIO 25    ├─ CS: GPIO 5
├─ Pad 2: GPIO 13              ├─ LED 2: GPIO 26    ├─ DC: GPIO 16
├─ Pad 3: GPIO 12              ├─ LED 3: GPIO 27    ├─ RST: GPIO 23
└─ Pad 4: GPIO 15              └─ LED 4: GPIO 32    ├─ MOSI: GPIO 19
                                                     ├─ CLK: GPIO 18
                                                     ├─ MISO: GPIO 38
                                                     └─ Backlight: GPIO 4
```

---

## Software Setup

### Step 1: Install PlatformIO

1. Open **VS Code**
2. Go to *Extensions* (Ctrl+Shift+X on Windows/Linux, Cmd+Shift+X on Mac)
3. Search for **"PlatformIO IDE"** by PlatformIO
4. Click **Install** (this also installs the core tools)
5. Reload VS Code when prompted

### Step 2: Open the Project

1. In VS Code, click **File → Open Folder**
2. Navigate to the `Whack-a-Mole` project folder
3. PlatformIO should automatically detect it

### Step 3: Build and Upload

1. **Connect your ESP32 TTGO T-Display via USB**

2. **Build the project:**
   - Click the checkmark ✓ icon in the PlatformIO toolbar
   - Or press `Ctrl+Alt+B`

3. **Upload to the ESP32:**
   - Click the arrow → icon in the PlatformIO toolbar
   - Or press `Ctrl+Alt+U`
   - The upload should take 10-30 seconds

4. **Monitor the output (optional):**
   - Click the plug icon in the PlatformIO toolbar
   - Or press `Ctrl+Alt+S`
   - Set baud rate to **115200** if not automatic
   - Watch for calibration messages

---

## Hardware Wiring

### Creating Capacitive Touch Pads

1. **Cut copper tape into 4 squares** (~2cm x 2cm each)
2. **Stick them to a non-conductive surface** (plastic, cardboard, etc.)
3. **Label them:** Pad 1, Pad 2, Pad 3, Pad 4
4. **Connect each pad to its GPIO pin** with a jumper wire
5. **Connect all GND lines together** on the breadboard

The ESP32 has built-in capacitive touch sensing - no additional components needed!

### Wiring LEDs

For each LED:
```
         ┌─ GPIO pin (e.g., GPIO 25)
         |
       ╱─╲
      │   │ LED  } 
      │   │         } Connect between GPIO and GND
       ╲─╱       } with 220-330Ω resistor in series
         |
         ├─ 220-330Ω Resistor
         |
         └─ GND (0V)
```

**Important:** Connect the LED resistor to the cathode (short leg) side.

### Wiring Diagram Summary

- **Copper Tape Pads:** Connect directly to GPIO pins (4, 13, 12, 15)
- **LEDs:** Connect through resistors to GPIO pins and GND
- **Display:** Already built-in to TTGO T-Display
- **All GND pins:** Connect together on the breadboard

---

## Calibration & Configuration

### Touch Sensitivity Adjustment

The touch sensors are calibrated at startup. If they don't work well:

1. **Open `src/main.cpp`**
2. **Find this line (around line 25):**
   ```cpp
   #define TOUCH_THRESHOLD 40
   ```

3. **Adjust based on behavior:**
   - **Too sensitive** (false triggers): INCREASE to 60-80
   - **Not sensitive enough**: DECREASE to 20-30

4. **View baseline values to understand sensitivity:**
   - Open Serial Monitor (`Ctrl+Alt+S`)
   - Watch for "Calibration complete!" message
   - Note the baseline values printed for each pad
   - Typical values are 50-500

5. **How to interpret:**
   - The threshold is compared against the baseline
   - A larger baseline = stronger sensor signal possible
   - Adjust threshold based on how much the value drops when touched

### Display Orientation

If the display appears rotated or upside-down:
1. Open `src/main.cpp`
2. Find `tft.setRotation()` in the `setup()` function
3. Try values 0, 1, 2, or 3 until correct

Current setting: **1** (landscape mode)

---

## Game Instructions

### Start Screen
- Displays: "Whack-a-Mole" title
- Instructions: "Tap any pad to start"
- **Action:** Tap any copper pad to begin

### Active Game (30 seconds)
- An LED lights up (the "active mole")
- **Goal:** Tap the corresponding copper pad
- **HIT!** (shown in green) = +1 point
- **MISS!** (shown in red) = 0 points
- Game gets harder over time (LED blinks faster)

### Difficulty Levels
| Time | LED Duration | Difficulty |
|------|--------------|-----------|
| 0-10s | 1500 ms | EASY |
| 10-20s | 1100 ms | MEDIUM |
| 20-30s | 800 ms | HARD |

### Game Over Screen
- Displays final score
- Shows: "Tap any pad to restart"

---

## Troubleshooting

### "COM Port Not Found" Error
- Check USB cable connection
- Try a different USB port
- Ensure the cable supports data (not just charging)
- Try holding BOOT button while connecting to ESP32

### Device Connected But Won't Upload
- Click the **trash icon** in PlatformIO to clean build
- Try uploading again
- If still failing, try uploading at a lower speed (edit `platformio.ini`)

### Touch Pads Don't Work
1. Check that copper tape makes good electrical contact with jumper wires
2. Print baseline values in Serial Monitor (look for calibration output)
3. Adjust `TOUCH_THRESHOLD` value
4. Ensure no metal objects or hands are very close to pads when not playing
5. Try a larger copper tape patch if baseline values are very high

### Display Shows Garbage or Doesn't Update
- Verify display rotation: Try different `tft.setRotation()` values (0-3)
- Try resetting the ESP32 (press RST button)
- Check that USB power is stable
- Verify build flags in `platformio.ini` match TTGO T-Display specs

### Game is Too Easy or Too Hard
- **Too easy (scoring too much):** Increase `TOUCH_THRESHOLD` to require firmer touches
- **Too hard (can't score):** Decrease `TOUCH_THRESHOLD` for easier detection
- Check Serial Monitor during gameplay for touch detection feedback
- Ensure copper tape pads are approximately 2cm × 2cm

### LED Doesn't Turn On
- Verify GPIO pin connections: 25, 26, 27, 32
- Check that LED polarity is correct (long leg = anode/+)
- Test resistor values (should be 220-330Ω)
- Verify resistor is connected to LED cathode, not anode
- Check that all GND connections are solid

---

## Code Structure

### Main Files

**`src/main.cpp`** - Complete game implementation
- Pin definitions and setup
- Game state machine (START_SCREEN → PLAYING → GAME_OVER)
- Touch pad calibration and reading logic
- Game timing and scoring
- LED control functions
- TFT display rendering

**`platformio.ini`** - Project configuration
- Board and platform settings
- Library dependencies
- Build flags for display driver
- Serial monitor baud rate

### Key Concepts

**Non-Blocking Timing:**
- Uses `millis()` instead of `delay()`
- Allows the game loop to stay responsive
- Touch pads are checked frequently

**Game State Machine:**
```
START_SCREEN (waiting for tap)
    ↓ (any pad tapped)
PLAYING (30-second game)
    ├─ Random LED lights up (active mole)
    ├─ Player must tap correct pad
    ├─ LED duration decreases as game progresses
    └─ 30 seconds elapsed
        ↓
GAME_OVER (show score)
    ↓ (any pad tapped)
BACK TO START_SCREEN
```

**Touch Sensing Process:**
1. Baseline calibration at startup (reads when no touch)
2. During game, compare current reading to baseline
3. If reading drops by more than `TOUCH_THRESHOLD`: register as touch
4. Debounce by waiting for pad to be released

---

## Serial Monitor Output Explanation

When you connect to Serial Monitor with proper baud rate (115200), you'll see:

```
=== Whack-a-Mole Game Starting ===

Calibrating touch pads...
Calibration complete!
Touch pad baseline values:
  Pad 0: 87
  Pad 1: 92
  Pad 2: 88
  Pad 3: 95
Game started!
New mole: 2, duration: 1500 ms
HIT! Score: 1
New mole: 0, duration: 1500 ms
MISS!
New mole: 3, duration: 1200 ms
...
```

This helps debug touch sensitivity issues.

---

## Optimization Tips

### If Touch Response is Slow
1. Decrease `TOUCH_THRESHOLD` value
2. Ensure copper tape pads are large and well-connected
3. Check Serial Monitor for baseline values (should be 50-200 ideally)

### If False Touches Occur
1. Increase `TOUCH_THRESHOLD` value
2. Ensure pads aren't near other conductive objects
3. Add shielding or move pads further apart if needed

### If Display Updates Lag
- Current code updates at 100ms intervals - this is optimized
- Reduce `DRAW_UPDATE_INTERVAL` if you want faster updates (uses more power)

---

## Features of This Implementation

✓ **Responsive Game Loop** - Uses `millis()` for non-blocking timing  
✓ **Automatic Calibration** - Touch pads self-calibrate at startup  
✓ **Progressive Difficulty** - Gets harder every 10 seconds  
✓ **User Feedback** - Shows HIT/MISS feedback immediately  
✓ **Well-Commented Code** - Easy to understand and modify  
✓ **Stable Timing** - All LED switches synchronized  
✓ **Clear UI** - Shows score, time, difficulty level, and feedback  

---

## Future Enhancement Ideas

- **High Score Storage** - Save best scores in EEPROM
- **Sound Effects** - Add a buzzer for feedback
- **Different Game Modes** - Endless mode, survival mode, multiplayer
- **Difficulty Settings** - Choose starting difficulty
- **LED Intensity** - PWM brightness control
- **Combo Multiplier** - Bonus points for multiple hits
- **Vibration Feedback** - Motor feedback on hits

---

## Learning Outcomes

By completing this project, you'll learn:

1. **Microcontroller Programming** - Arduino on ESP32
2. **Sensor Input Handling** - Capacitive touch sensing
3. **Real-Time Systems** - Non-blocking timing with `millis()`
4. **State Machines** - Game state management
5. **Display Driver Control** - TFT graphics and text
6. **Hardware Integration** - LEDs, sensors, displays working together
7. **Embedded Systems Design** - Responsive, efficient code for microcontrollers

---

## Support & Resources

- **PlatformIO Docs:** https://docs.platformio.org/
- **TFT_eSPI Library:** https://github.com/Bodmer/TFT_eSPI
- **ESP32 Arduino Docs:** https://docs.espressif.com/projects/arduino-esp32/en/latest/
- **TTGO T-Display Info:** Search for "TTGO T-Display ESP32" for community examples

---

## Debugging Checklist

Before troubleshooting, verify:
- [ ] ESP32 is connected via USB
- [ ] No compilation errors (check PlatformIO problems)
- [ ] Upload completes successfully
- [ ] Serial Monitor shows "Calibration complete!"
- [ ] At least one LED can turn on manually
- [ ] Display shows start screen
- [ ] Touch pads have visible copper tape connections

If issues persist, try in order:
1. Restart VS Code and PlatformIO
2. Clean build (trash icon)
3. Rebuild and upload
4. Check Serial Monitor output
5. Verify hardware connections with multimeter
6. Adjust `TOUCH_THRESHOLD` and recalibrate

---

**Good luck with your embedded systems final project!** 🎮


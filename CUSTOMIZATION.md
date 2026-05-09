# Code Customization Guide

## Overview

This guide shows you how to modify the Whack-a-Mole game to customize gameplay, difficulty, and behavior. All modifications are made to `src/main.cpp`.

---

## Quick Modifications

### Change Game Duration

**Default: 30 seconds**

In the `updateGame()` function, find:
```cpp
// Line ~190
if (elapsedTime >= 30000) {  // 30000 milliseconds = 30 seconds
```

**To change:**
- 20 seconds: `20000`
- 60 seconds: `60000`
- 45 seconds: `45000`

### Change Difficulty Levels

**Default progression:**
- 0-10s: 1500ms per mole
- 10-20s: 1100ms per mole
- 20-30s: 800ms per mole

Find the `getGameDifficulty()` function around **line 305**:

```cpp
int getGameDifficulty() {
  unsigned long elapsedTime = millis() - gameStartTime;
  
  if (elapsedTime < 10000) {
    // First 10 seconds: 1500ms per mole
    return 1500;  // <-- Change this
  } else if (elapsedTime < 20000) {
    // Next 10 seconds: 1100ms per mole
    return 1100;  // <-- Or this
  } else {
    // Last 10 seconds: 800ms per mole
    return 800;   // <-- Or this
  }
}
```

**Example: Super Hard Mode (gets faster immediately)**
```cpp
int getGameDifficulty() {
  unsigned long elapsedTime = millis() - gameStartTime;
  
  if (elapsedTime < 5000) {
    return 1000;   // Very fast from the start
  } else if (elapsedTime < 15000) {
    return 600;
  } else {
    return 300;    // Insanely fast at the end
  }
}
```

### Change LED Colors (Feedback Text)

In the `drawGameScreen()` function around **line 370**:

```cpp
if (feedbackText == "HIT!") {
  tft.setTextColor(TFT_GREEN, TFT_BLACK);  // <-- Change this color
} else {
  tft.setTextColor(TFT_RED, TFT_BLACK);    // <-- Or this color
}
```

**Available colors in TFT_eSPI:**
```cpp
TFT_BLACK, TFT_NAVY, TFT_DARKGREEN, TFT_DARKCYAN,
TFT_MAROON, TFT_PURPLE, TFT_OLIVE, TFT_LIGHTGREY,
TFT_DARKGREY, TFT_BLUE, TFT_GREEN, TFT_CYAN,
TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_WHITE,
TFT_ORANGE, TFT_GREENYELLOW, TFT_PINK, TFT_BROWN
```

### Show/Hide Difficulty Text

In the `drawGameScreen()` function, find around **line 385**:

```cpp
// Draw difficulty indicator
tft.setTextDatum(BC_DATUM);
tft.setTextSize(1);
tft.setTextColor(TFT_WHITE, TFT_BLACK);
if (elapsedTime < 10000) {
  tft.drawString("Difficulty: EASY", 120, 130);
} else if (elapsedTime < 20000) {
  tft.drawString("Difficulty: MEDIUM", 120, 130);
} else {
  tft.drawString("Difficulty: HARD", 120, 130);
}
```

**To hide it:** Comment out all the lines or delete that section entirely.

### Change Initial Threshold

Find near the top of the file, around **line 26**:

```cpp
#define TOUCH_THRESHOLD 40  // <-- Change this value
```

See [CALIBRATION.md](CALIBRATION.md) for guidance on this value.

---

## Intermediate Customizations

### Add Sound Effects

To add a buzzer that beeps on HIT:

1. Define a buzzer pin at the top:
```cpp
#define BUZZER_PIN 33  // GPIO 33 (or any available GPIO)
```

2. Initialize in `setup()`:
```cpp
pinMode(BUZZER_PIN, OUTPUT);
```

3. Beep on HIT in `updateGame()`:
```cpp
if (activeMole != -1 && TOUCH_TO_MOLE[touchedPad] == activeMole) {
  score++;
  displayFeedback("HIT!");
  
  // Add beep!
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  
  // ... rest of code
}
```

### Add Score Multiplier

To get 2 points per hit after 20 seconds:

Find the HIT detection in `updateGame()` around **line 210**:

```cpp
if (activeMole != -1 && TOUCH_TO_MOLE[touchedPad] == activeMole) {
  // Original: score++;
  
  // Add multiplier based on time:
  unsigned long elapsedTime = millis() - gameStartTime;
  if (elapsedTime >= 20000) {
    score += 2;  // Double points in final 10 seconds
  } else {
    score += 1;
  }
  
  displayFeedback("HIT!");
  // ... rest of code
}
```

### Add Combo System

Let's add a combo counter that resets on MISS:

1. Add variable at the top with other globals:
```cpp
int comboCounter = 0;
```

2. In `updateGame()`, modify HIT and MISS handling:
```cpp
if (activeMole != -1 && TOUCH_TO_MOLE[touchedPad] == activeMole) {
  score++;
  comboCounter++;  // Increase combo
  
  String feedback = "HIT! x" + String(comboCounter);
  displayFeedback(feedback);  // Can now accept any string
  
  // ... rest of code
} else {
  // Wrong pad touched
  if (comboCounter > 0) {
    Serial.printf("Combo broken! Was: %d\n", comboCounter);
  }
  comboCounter = 0;  // Reset on MISS
  displayFeedback("MISS!");
}
```

3. Display combo on-screen in `drawGameScreen()`:
```cpp
// After drawing score
if (comboCounter > 1) {
  tft.setTextSize(2);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  tft.drawString("Combo: " + String(comboCounter), 235, 25);
}
```

---

## Advanced Customizations

### Per-Pad Sensitivity Thresholds

If one pad is too sensitive/insensitive:

1. Define individual thresholds:
```cpp
#define TOUCH_THRESHOLD_PAD_0 40
#define TOUCH_THRESHOLD_PAD_1 45
#define TOUCH_THRESHOLD_PAD_2 35
#define TOUCH_THRESHOLD_PAD_3 50
```

2. Modify `getTouchedPad()`:
```cpp
int getTouchedPad() {
  const int thresholds[4] = {
    TOUCH_THRESHOLD_PAD_0,
    TOUCH_THRESHOLD_PAD_1,
    TOUCH_THRESHOLD_PAD_2,
    TOUCH_THRESHOLD_PAD_3
  };
  
  for (int i = 0; i < 4; i++) {
    if (getTouchValue(i) < (touchBaseline[i] - thresholds[i])) {
      return i;
    }
  }
  return -1;
}
```

### Save High Score to EEPROM

1. Include EEPROM header at top:
```cpp
#include <EEPROM.h>
```

2. Define EEPROM location:
```cpp
#define EEPROM_ADDR_HIGHSCORE 0
```

3. Add function to save/load score:
```cpp
void saveHighScore(int newScore) {
  int highScore = getHighScore();
  if (newScore > highScore) {
    EEPROM.write(EEPROM_ADDR_HIGHSCORE, newScore);
    EEPROM.commit();
    Serial.printf("New high score: %d!\n", newScore);
  }
}

int getHighScore() {
  return EEPROM.read(EEPROM_ADDR_HIGHSCORE);
}
```

4. Call when game ends in `updateGame()`:
```cpp
if (elapsedTime >= 30000) {
  gameState = GAME_OVER;
  saveHighScore(score);  // <-- Add this line
  turnOffAllLEDs();
  return;
}
```

5. Display on game over screen in `drawGameOverScreen()`:
```cpp
void drawGameOverScreen() {
  tft.fillScreen(TFT_BLACK);
  
  // ... existing code ...
  
  // Add high score display
  int highScore = getHighScore();
  if (score == highScore && highScore > 0) {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("NEW HIGH SCORE!", 120, 110);
  } else if (highScore > 0) {
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("High Score: " + String(highScore), 120, 110);
  }
}
```

### Custom Game Modes

Add a mode selector at start:

1. Add variable:
```cpp
int gameMode = 0;  // 0=Normal, 1=Endless, 2=Zen
```

2. Modify game loop behavior based on mode:
```cpp
void updateGame() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - gameStartTime;
  
  // Check if game is over (depends on mode)
  bool gameOver = false;
  if (gameMode == 0) {
    gameOver = (elapsedTime >= 30000);  // Normal: 30 sec
  } else if (gameMode == 1) {
    gameOver = false;  // Endless: never ends
  } else if (gameMode == 2) {
    gameOver = (elapsedTime >= 60000);  // Zen: 60 sec
  }
  
  if (gameOver) {
    gameState = GAME_OVER;
    turnOffAllLEDs();
    return;
  }
  
  // ... rest of game logic ...
}
```

### PWM LED Brightness Control

For dimmer LEDs or breathing effect:

1. Include PWM support (ESP32 does this automatically)

2. Replace LED control functions:
```cpp
void turnOnLED(int mole, int brightness = 255) {
  if (mole >= 0 && mole < 4) {
    analogWrite(MOLE_TO_LED[mole], brightness);
  }
}

void turnOffLED(int mole) {
  if (mole >= 0 && mole < 4) {
    analogWrite(MOLE_TO_LED[mole], 0);
  }
}

void flashLED(int mole) {
  // Breathing effect
  for (int i = 0; i < 256; i += 10) {
    analogWrite(MOLE_TO_LED[mole], i);
    delay(5);
  }
  for (int i = 256; i > 0; i -= 10) {
    analogWrite(MOLE_TO_LED[mole], i);
    delay(5);
  }
}
```

3. Use it in `chooseMole()`:
```cpp
void chooseMole() {
  activeMole = random(0, 4);
  moleStartTime = millis();
  moleActiveDuration = getGameDifficulty();
  
  turnOnLED(activeMole, 200);  // 200/255 brightness
  // Or use LED breathing:
  // flashLED(activeMole);
}
```

---

## Display Customizations

### Change Screen Text Colors

Example: Change the start screen colors:

```cpp
void drawStartScreen() {
  tft.fillScreen(TFT_NAVY);  // Dark blue background instead of black
  
  // Title in bright cyan
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_CYAN, TFT_NAVY);  // NEW COLOR
  tft.drawString("Whack-a-Mole", 120, 30);
  
  // Instructions in yellow
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_NAVY);  // NEW COLOR
  tft.drawString("Tap any pad", 120, 70);
  tft.drawString("to start", 120, 95);
}
```

### Add Progress Bar

Show remaining time as a bar:

```cpp
void drawGameScreen() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - gameStartTime;
  int remainingTime = (30000 - elapsedTime) / 1000;
  
  // ... existing code ...
  
  // Draw progress bar
  int barWidth = (200 * elapsedTime) / 30000;  // 0 to 200 pixels
  tft.drawRect(20, 115, 200, 10, TFT_WHITE);  // Bar outline
  tft.fillRect(20, 115, barWidth, 10, TFT_GREEN);  // Bar fill
}
```

### Minimize Display Updates

For better performance, use dirty rectangles:

```cpp
void drawGameScreen() {
  // Instead of tft.fillScreen(TFT_BLACK);
  // Only clear areas that change:
  
  // Clear score area
  tft.fillRect(5, 5, 100, 20, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Score: " + String(score), 5, 5);
  
  // Clear time area
  tft.fillRect(150, 5, 85, 20, TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString(String(remainingTime) + "s", 235, 5);
  
  // Clear center
  tft.fillRect(0, 40, 240, 60, TFT_BLACK);
  // ... draw only changed elements
}
```

---

## Debugging Helps

### Add Debug Output

Add Serial printing to understand game flow:

```cpp
void updateGame() {
  // ... existing code ...
  
  int touchedPad = getTouchedPad();
  if (touchedPad != -1) {
    Serial.printf("Touch detected on pad: %d\n", touchedPad);
    
    if (activeMole != -1 && TOUCH_TO_MOLE[touchedPad] == activeMole) {
      Serial.printf("HIT on mole %d! Score now: %d\n", activeMole, score);
    }
  }
}
```

### Add FPS Counter

Know how fast your game loop runs:

```cpp
// Add at top with globals
unsigned long lastFPSTime = 0;
int frameCount = 0;
int currentFPS = 0;

// In loop()
frameCount++;
if (millis() - lastFPSTime >= 1000) {
  currentFPS = frameCount;
  frameCount = 0;
  lastFPSTime = millis();
  Serial.printf("FPS: %d\n", currentFPS);
}

// Display FPS on screen in drawGameScreen()
tft.setTextDatum(BL_DATUM);
tft.setTextSize(1);
tft.setTextColor(TFT_DARKGREY);
tft.drawString("FPS: " + String(currentFPS), 5, 135);
```

---

## Testing Your Changes

After modifying the code:

1. **Build:**
   - Press `Ctrl+Alt+B`
   - Watch for compilation errors

2. **Upload:**
   - Ensure ESP32 is connected
   - Press `Ctrl+Alt+U`
   - Watch for upload complete message

3. **Test:**
   - Open Serial Monitor
   - Watch calibration output
   - Verify changes work as expected

4. **Debug:**
   - Check Serial output for your debug prints
   - Watch LED/display behavior
   - Note any timing issues

---

## Common Mistakes

| Mistake | Problem | Prevention |
|---------|---------|-----------|
| Forgetting semicolons | Won't compile | Use compiler errors as guide |
| Wrong variable type | Logic errors | Use `Serial.println()` to verify |
| Off-by-one errors | Timing issues | Test boundaries (0s, 10s boundaries) |
| Not updating display | Visual glitches | Remember to call `tft.drawString()` |
| Blocking delays | Game lag | Always use `millis()` timing |

---

##Template for Adding a New Feature

```cpp
// 1. ADD VARIABLE (near top with other globals)
int myNewFeature = 0;

// 2. INITIALIZE in setup()
myNewFeature = 0;

// 3. UPDATE in loop() or updateGame()
if (someCondition) {
  myNewFeature++;
}

// 4. DISPLAY in drawGameScreen()
tft.drawString(String(myNewFeature), 120, 50);

// 5. TEST
// - Compile with Ctrl+Alt+B
// - Upload with Ctrl+Alt+U
// - Check Serial Monitor for errors
```

---

**Have fun customizing! Remember to test each change in isolation before adding the next one.**

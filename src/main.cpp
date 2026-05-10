#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================================
// PIN DEFINITIONS
// ============================================================================

// Touch Pad Pins (Capacitive Touch Sensors)
#define TOUCH_PAD_1 T0  // GPIO 4
#define TOUCH_PAD_2 T2  // GPIO 13
#define TOUCH_PAD_3 T5  // GPIO 12
#define TOUCH_PAD_4 T4  // GPIO 15

// LED Pins
#define LED_1 25
#define LED_2 26
#define LED_3 27
#define LED_4 32

// Touch threshold value
// CALIBRATION TIPS:
// - If sensors are too sensitive (triggering without taps): INCREASE this value (try 80-100)
// - If sensors are not sensitive enough: DECREASE this value (try 20-40)
// - Read the baseline values at startup and adjust based on your copper tape size
#define TOUCH_THRESHOLD 40

// ============================================================================
// GAME STATE ENUM
// ============================================================================
enum GameState {
  START_SCREEN,
  PLAYING,
  GAME_OVER
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
TFT_eSPI tft = TFT_eSPI();

GameState gameState = START_SCREEN;
int score = 0;
unsigned long gameStartTime = 0;
unsigned long moleStartTime = 0;
int activeMole = -1;  // -1 means no mole is active
int moleActiveDuration = 1500;  // milliseconds

// Touch pad baseline values (calibrated at startup)
uint16_t touchBaseline[4] = {0, 0, 0, 0};

// Mapping of touch pads to moles (0-3)
const int TOUCH_TO_MOLE[4] = {0, 1, 2, 3};  // touch pad 0,1,2,3 -> mole 0,1,2,3
const int MOLE_TO_LED[4] = {LED_1, LED_2, LED_3, LED_4};

// Timing variables
unsigned long lastDrawTime = 0;
const unsigned long DRAW_UPDATE_INTERVAL = 100;  // Update display every 100ms
bool showFeedback = false;
String feedbackText = "";
unsigned long feedbackStartTime = 0;
const unsigned long FEEDBACK_DISPLAY_TIME = 500;  // Show feedback for 500ms

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================
void calibrateTouchPads();
void drawStartScreen();
void drawGameScreen();
void drawGameOverScreen();
void updateGame();
void turnOnLED(int mole);
void turnOffLED(int mole);
void turnOffAllLEDs();
void chooseMole();
bool checkTouchPads();
int getTouchedPad();
uint32_t getTouchValue(int padIndex);
void displayFeedback(String text);
int getGameDifficulty();

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n\n=== Whack-a-Mole Game Starting ===\n");
  Serial.println("Display resolution: 240x135 (ST7735S driver)");
  Serial.println("Initializing display...");
  
  // Initialize LED pins
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  turnOffAllLEDs();
  
  // Initialize TFT display with extra delay for stability
  tft.init();
  delay(100);
  tft.setRotation(1);  // Landscape mode for TTGO T-Display
  delay(100);
  
  // Fill screen with black and show it's working
  tft.fillScreen(TFT_BLACK);
  delay(200);
  
  // Quick test - show all colors to verify display works
  Serial.println("Display test: Showing color bars...");
  tft.fillRect(0, 0, 48, 135, TFT_RED);
  tft.fillRect(48, 0, 48, 135, TFT_GREEN);
  tft.fillRect(96, 0, 48, 135, TFT_BLUE);
  tft.fillRect(144, 0, 96, 135, TFT_YELLOW);
  delay(1000);
  
  tft.fillScreen(TFT_BLACK);
  delay(500);
  
  Serial.println("Display initialized successfully!");
  
  delay(500);
  
  // Calibrate touch pads
  Serial.println("Calibrating touch pads...");
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Calibrating...", 120, 67);
  
  calibrateTouchPads();
  
  Serial.println("Calibration complete!");
  Serial.println("Touch pad baseline values:");
  for (int i = 0; i < 4; i++) {
    Serial.printf("  Pad %d: %d\n", i, touchBaseline[i]);
  }
  
  delay(1000);
  
  // Draw start screen
  drawStartScreen();
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  unsigned long currentMillis = millis();
  
  switch (gameState) {
    case START_SCREEN:
      // Wait for any touch to start the game
      if (checkTouchPads()) {
        Serial.println("Game started!");
        gameState = PLAYING;
        score = 0;
        gameStartTime = currentMillis;
        turnOffAllLEDs();
        chooseMole();
      }
      break;
      
    case PLAYING:
      updateGame();
      
      // Update display at regular intervals
      if (currentMillis - lastDrawTime >= DRAW_UPDATE_INTERVAL) {
        drawGameScreen();
        lastDrawTime = currentMillis;
      }
      break;
      
    case GAME_OVER:
      drawGameOverScreen();
      
      // Wait for any touch to restart
      if (checkTouchPads()) {
        Serial.println("Game restarted!");
        gameState = START_SCREEN;
        drawStartScreen();
      }
      break;
  }
  
  // Small delay to prevent overwhelming the CPU
  delayMicroseconds(100);
}

// ============================================================================
// CALIBRATION FUNCTION
// ============================================================================
void calibrateTouchPads() {
  // Take multiple readings to get a stable baseline
  // For each touch pad, we read 10 values and average them
  const int READ_COUNT = 10;
  
  for (int pad = 0; pad < 4; pad++) {
    uint32_t sum = 0;
    for (int i = 0; i < READ_COUNT; i++) {
      // Read touch value from the touch pad
      // touch_pad_read_raw_data() gives us the raw value
      uint32_t value = touchRead(4 + pad * 3 + (pad >= 2 ? 1 : 0));
      sum += value;
      delay(20);
    }
    touchBaseline[pad] = sum / READ_COUNT;
  }
  
  Serial.println("Baseline calibration complete!");
}

// ============================================================================
// TOUCH PAD READING FUNCTIONS
// ============================================================================

// Check if any touch pad has been touched
bool checkTouchPads() {
  for (int i = 0; i < 4; i++) {
    if (getTouchValue(i) < (touchBaseline[i] - TOUCH_THRESHOLD)) {
      return true;
    }
  }
  return false;
}

// Get which touch pad was touched (-1 if none)
int getTouchedPad() {
  for (int i = 0; i < 4; i++) {
    if (getTouchValue(i) < (touchBaseline[i] - TOUCH_THRESHOLD)) {
      return i;
    }
  }
  return -1;
}

// Helper function to read raw touch values from specific GPIO pins
uint32_t getTouchValue(int padIndex) {
  // Map pad index to touch channel
  // Pad 0 -> T0 (GPIO 4)
  // Pad 1 -> T2 (GPIO 13)
  // Pad 2 -> T5 (GPIO 12)
  // Pad 3 -> T4 (GPIO 15)
  
  const int touchChannels[4] = {0, 2, 5, 4};  // Touch channel numbers
  return touchRead(touchChannels[padIndex]);
}

// ============================================================================
// GAME LOGIC FUNCTIONS
// ============================================================================

void updateGame() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - gameStartTime;
  
  // Check if game is over (30 seconds)
  if (elapsedTime >= 30000) {
    gameState = GAME_OVER;
    turnOffAllLEDs();
    return;
  }
  
  // Check if a touch pad was pressed
  int touchedPad = getTouchedPad();
  if (touchedPad != -1) {
    if (activeMole != -1 && TOUCH_TO_MOLE[touchedPad] == activeMole) {
      // Correct pad touched!
      score++;
      displayFeedback("HIT!");
      Serial.printf("HIT! Score: %d\n", score);
      
      // Turn off the LED and choose a new mole
      turnOffLED(activeMole);
      chooseMole();
    } else {
      // Wrong pad touched
      displayFeedback("MISS!");
      Serial.println("MISS!");
    }
    
    // Debounce: wait for pad to be released
    while (getTouchedPad() != -1) {
      delay(50);
    }
  }
  
  // Check if current mole's time has expired
  if (activeMole != -1) {
    unsigned long moleDuration = currentMillis - moleStartTime;
    if (moleDuration >= moleActiveDuration) {
      // Time expired without being tapped
      displayFeedback("MISS!");
      Serial.println("MISS! Time expired");
      
      turnOffLED(activeMole);
      chooseMole();
    }
  }
}

// Choose a new random mole and turn on its LED
void chooseMole() {
  activeMole = random(0, 4);
  moleStartTime = millis();
  
  // Update difficulty based on elapsed time
  moleActiveDuration = getGameDifficulty();
  
  turnOnLED(activeMole);
  Serial.printf("New mole: %d, duration: %d ms\n", activeMole, moleActiveDuration);
}

// Get the mole active duration based on game difficulty (time-based)
int getGameDifficulty() {
  unsigned long elapsedTime = millis() - gameStartTime;
  
  if (elapsedTime < 10000) {
    // First 10 seconds: 1500ms per mole
    return 1500;
  } else if (elapsedTime < 20000) {
    // Next 10 seconds: 1100ms per mole
    return 1100;
  } else {
    // Last 10 seconds: 800ms per mole
    return 800;
  }
}

// ============================================================================
// LED CONTROL FUNCTIONS
// ============================================================================

void turnOnLED(int mole) {
  if (mole >= 0 && mole < 4) {
    digitalWrite(MOLE_TO_LED[mole], HIGH);
  }
}

void turnOffLED(int mole) {
  if (mole >= 0 && mole < 4) {
    digitalWrite(MOLE_TO_LED[mole], LOW);
  }
}

void turnOffAllLEDs() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(MOLE_TO_LED[i], LOW);
  }
}

// ============================================================================
// DISPLAY/DRAWING FUNCTIONS
// ============================================================================

void drawStartScreen() {
  tft.fillScreen(TFT_BLACK);
  
  // Draw title
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("Whack-a-Mole", 120, 30);
  
  // Draw instructions
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Tap any pad", 120, 70);
  tft.drawString("to start", 120, 95);
}

void drawGameScreen() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - gameStartTime;
  int remainingTime = (30000 - elapsedTime) / 1000;
  
  // Clear background
  tft.fillScreen(TFT_BLACK);
  
  // Draw score at top left
  tft.setTextDatum(TL_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Score: " + String(score), 5, 5);
  
  // Draw remaining time at top right
  tft.setTextDatum(TR_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString(String(remainingTime) + "s", 235, 5);
  
  // Draw active mole indicator
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  if (activeMole != -1) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Mole " + String(activeMole + 1) + " ACTIVE!", 120, 50);
  }
  
  // Draw feedback text if active
  if (showFeedback) {
    unsigned long feedbackElapsed = currentMillis - feedbackStartTime;
    if (feedbackElapsed < FEEDBACK_DISPLAY_TIME) {
      tft.setTextSize(3);
      if (feedbackText == "HIT!") {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
      } else {
        tft.setTextColor(TFT_RED, TFT_BLACK);
      }
      tft.drawString(feedbackText, 120, 90);
    } else {
      showFeedback = false;
    }
  }
  
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
}

void drawGameOverScreen() {
  tft.fillScreen(TFT_BLACK);
  
  // Draw game over title
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(3);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("GAME OVER", 120, 20);
  
  // Draw final score
  tft.setTextSize(3);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("Final Score:", 120, 50);
  
  tft.setTextSize(4);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString(String(score), 120, 85);
  
  // Draw restart instructions
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Tap any pad", 120, 115);
  tft.drawString("to restart", 120, 135);
}

// ============================================================================
// FEEDBACK DISPLAY FUNCTION
// ============================================================================

void displayFeedback(String text) {
  feedbackText = text;
  showFeedback = true;
  feedbackStartTime = millis();
}

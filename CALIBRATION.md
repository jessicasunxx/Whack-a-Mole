# Touch Pad Calibration Guide

## Quick Start Calibration

### Step 1: Initial Upload
1. Upload the code with default `TOUCH_THRESHOLD 40`
2. Open Serial Monitor at **115200 baud**
3. Press RST button on ESP32 or restart the board
4. Observe the calibration output

### Step 2: Read Baseline Values
You should see something like:
```
Calibration complete!
Touch pad baseline values:
  Pad 0: 87
  Pad 1: 92
  Pad 2: 88
  Pad 3: 95
```

These are your **baseline values** when NOT touching the pads.

### Step 3: Test Touch Detection

1. In the Serial Monitor, watch the game output
2. Play one test round and see what happens:
   - **Immediate HIT on tap?** → Sensitivity might be too high
   - **Have to really press hard to register?** → Sensitivity might be too low
   - **Works perfectly?** → No calibration needed!

### Step 4: Adjust if Needed

If the game doesn't feel right, edit `src/main.cpp`:

```cpp
// Line ~25
#define TOUCH_THRESHOLD 40   // <-- Change this number
```

**Adjustment Guidelines:**

| Problem | Solution | New Value to Try |
|---------|----------|-----------------|
| False triggers / too sensitive | INCREASE threshold | 60, 80, 100 |
| Need to tap very hard | DECREASE threshold | 30, 20, 10 |
| Works perfectly | No change needed | Keep at 40 |

### Step 5: Re-Test

After changing the value:
1. Save `src/main.cpp`
2. Build and upload again
3. Watch Serial Monitor for new baseline values
4. Test gameplay again
5. Repeat until satisfied

---

## Understanding Capacitive Touch Sensing

### How It Works

The ESP32 has built-in **capacitive touch sensing**. The process:

1. **Baseline Measurement:** When the ESP32 starts, it measures the electrical properties of each touch pad (when nothing is touching it)

2. **Touch Detection:** When you touch the copper tape, the capacitance changes (your finger adds capacitance)

3. **Comparison:** The current reading is compared to the baseline
   - If the difference exceeds the threshold: TOUCH detected
   - Otherwise: no touch

### Formula

```
Touch Detected? = (Baseline - Current Reading) > TOUCH_THRESHOLD
```

### Example

If baseline is **87** and threshold is **40**:
- Current reading **85**: 87 - 85 = 2 (No touch, since 2 < 40)
- Current reading **60**: 87 - 60 = 27 (No touch, since 27 < 40)
- Current reading **40**: 87 - 40 = 47 (YES! Touch detected, since 47 > 40)

---

## Advanced Calibration

### For Different Copper Tape Sizes

**Large Pads (3cm × 3cm or bigger):**
- Baseline will be much higher (200-500 range)
- Try INCREASING threshold to 60-100

**Small Pads (1cm × 1cm):**
- Baseline will be lower (30-70 range)
- Try DECREASING threshold to 15-25

### For Environmental Factors

**Highly metallic/conductive environment:**
- Increase threshold to reduce noise

**Plastic/wooden enclosure:**
- Threshold might need to be lower

**Humid environment:**
- Environmental capacitance is higher
- May need to adjust baseline measurements

### Multi-Point Calibration (Advanced)

If individual pads need different thresholds, modify the `getTouchValue()` function to use an array of thresholds:

```cpp
#define TOUCH_THRESHOLD_PAD_0 40
#define TOUCH_THRESHOLD_PAD_1 45
#define TOUCH_THRESHOLD_PAD_2 35
#define TOUCH_THRESHOLD_PAD_3 50
```

Then modify the touch detection accordingly.

---

## Debugging Touch Issues

### Issue: No Touch Detected at All

**Checklist:**
1. ✓ Copper tape is firmly stuck to surface
2. ✓ Jumper wire makes good contact with tape (no corrosion)
3. ✓ Wire is connected to correct GPIO (4, 13, 12, or 15)
4. ✓ Serial output shows reasonable baseline values (not 0)
5. ✓ Threshold is not set too high

**Fix:** Decrease `TOUCH_THRESHOLD` to 10-20

### Issue: Too Many False Positives

**Checklist:**
1. ✓ No metal objects within 10cm of pads
2. ✓ USB cable is not touching the pads
3. ✓ Hands are dry (moisture affects capacitance)
4. ✓ EMI from nearby devices (WiFi, cell phones)
5. ✓ Baseline values vary significantly between pads

**Fix:** Increase `TOUCH_THRESHOLD` to 80-100

### Issue: One Pad Doesn't Work

**Possible causes:**
1. Wire disconnected or loose
2. Copper tape damaged or dirty
3. Different pad geometry than others
4. Individual pad needs higher threshold

**Troubleshooting:**
- Check baseline value for that pad in Serial Monitor
- Compare to other pads (should be similar)
- If very different (±50), there's likely a connection issue
- Can add individual threshold for that pad

### Issue: Baseline Values Keep Changing

This is **normal**. Small variations are expected due to:
- Temperature changes
- Humidity changes
- Electrical noise
- USB power variations

If they vary **wildly** (±100 between reads):
- Check for loose connections
- move the copper tape farther from RF sources
- Increase `TOUCH_THRESHOLD` for stability

---

## Calibration Procedure for Perfect Response

### Detailed Step-by-Step

1. **Disconnect**
   - Unplug ESP32 from USB
   - Wait 30 seconds

2. **Connect**
   - Plug in USB cable
   - Device should auto-restart

3. **Monitor Output**
   - Open Serial (115200 baud)
   - Wait for "Calibration complete!"
   - Note baseline values:
     ```
     Pad 0: ___ (Target: 50-250)
     Pad 1: ___ (Target: 50-250)
     Pad 2: ___ (Target: 50-250)
     Pad 3: ___ (Target: 50-250)
     ```

4. **Analyze**
   - If all values 50-250: Good signal quality
   - If any value < 30: Weak sensor connection
   - If any value > 500: Might have EMI, try shielding

5. **Test Light Touch**
   - Start the game ("Tap any pad to start")
   - Tap pads very lightly
   - All four should register easily

6. **Test Medium Touch**
   - Play one full 30-second game
   - Use normal tapping pressure
   - Should feel responsive

7. **Evaluate**
   - If responsive → Done!
   - If too sensitive → Increase threshold
   - If too insensitive → Decrease threshold

---

## Common Baseline Values

### Typical Range by Pad Size

```
Copper Tape Size:     Baseline Range:    Suggested Threshold:
─────────────────     ───────────────    ────────────────────
1cm × 1cm            20-80               10-20
2cm × 2cm            80-150              30-50
3cm × 3cm            150-300             50-100
5cm × 5cm            300-600             100-150
```

### Environmental Baselines

```
Setting:              Typical Baseline:    Threshold Adjustment:
────────              ─────────────────    ──────────────────────
Open Air             80-150               Default (40)
Plastic Enclosure    60-120               -5 to -10
Metal Enclosure      150-400              +20 to +50
Near WiFi/Router     100-200              +10 to +20
```

---

## Performance Tuning

### For Maximum Responsiveness
- Decrease threshold to 20-30
- Result: Very sensitive, might get false triggers
- Use if: You want fastest response time

### For Maximum Stability
- Increase threshold to 100-150
- Result: Very stable, might need firm taps
- Use if: False triggers are a problem

### Balanced (Default)
- Threshold of 40-50
- Result: Good responsiveness with stability
- Use if: Everything works well

---

## Troubleshooting Reference

| Symptom | Likely Cause | Solution |
|---------|--------------|----------|
| One pad not working | Connection or sensor issue | Check wire contact, test baseline |
| All pads work but need hard press | Threshold too high | Decrease TOUCH_THRESHOLD |
| All pads trigger randomly | Threshold too low | Increase TOUCH_THRESHOLD |
| Different pads have different sensitivity | Baseline variation OK | Can create per-pad thresholds |
| Baseline values vary wildly | EMI or loose connections | Check connections, move away from WiFi |
| Works once then stops | Possible ESP32 lock-up | Try pressing RST button |

---

## Testing Checklist

Before adjusting calibration, verify:

- [ ] Test with dry hands
- [ ] Keep 30cm away from WiFi router
- [ ] Remove any bracelets, rings, watches
- [ ] Ensure USB cable doesn't touch the pads
- [ ] All copper tape is firmly adhered
- [ ] All connections are visible and clean
- [ ] No corrosion on copper tape or wires
- [ ] Game starts cleanly (no random HIT on startup)

---

## If You Can't Get It Working

### Nuclear Option: Reset Everything

1. Delete all changes to `src/main.cpp`
2. Set `TOUCH_THRESHOLD` back to **40**
3. Check all hardware connections physically with multimeter
4. Ensure no cold solder joints
5. Rebuild and upload fresh

### Contact Resources

- Check the main README for troubleshooting
- Search "ESP32 capacitive touch" tutorials
- Verify TTGO T-Display comes with proper drivers installed

---

**Remember:** Calibration is normal! Every set of copper tape sensors will vary slightly. Trust the Serial Monitor output as your guide.

# FirstRepoGalih

## Encoder Interrupt Implementation

This repository contains an implementation of a dual encoder interrupt system for ESP32.

### Files

- **encoder_interrupt.ino** - Working Arduino sketch for ESP32 with dual encoder support
- **ENCODER_README.md** - Detailed documentation in Indonesian explaining the implementation

### Answer to "Apakah ini bisa?"

**Ya, ini bisa!** The original code concept is valid, but several issues were fixed:

1. ✅ Separate ISR handling for both encoders
2. ✅ Proper critical section protection
3. ✅ CHANGE interrupts for 4x resolution
4. ✅ Delay added to prevent serial flooding
5. ✅ Simplified ISR logic

### Quick Start

1. Upload `encoder_interrupt.ino` to your ESP32
2. Connect encoders to pins:
   - Encoder 1: GPIO 36 (A) and GPIO 34 (B)
   - Encoder 2: GPIO 39 (A) and GPIO 35 (B)
3. **Important**: Add external pull-up resistors (10kΩ) to all encoder pins
4. Open Serial Monitor at 115200 baud

See [ENCODER_README.md](ENCODER_README.md) for detailed documentation.
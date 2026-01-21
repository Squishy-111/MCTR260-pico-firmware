# Raspberry Pi Pico W Firmware

Robot Controller firmware for Raspberry Pi Pico W (2022, RP2040).

## Requirements

- Raspberry Pi Pico W (2022 version with RP2040)
- PlatformIO with Arduino framework (earlephilhower core)
- RC Controller Flutter app

## Quick Start

1. Open this folder in VS Code with PlatformIO
2. Edit `src/project_config.h` for your robot (GPIO pins, device name)
3. Build and upload:
   ```bash
   pio run -t upload
   pio run -t monitor
   ```
4. Connect with Flutter app via BLE

## Project Structure

```
firmwares/pico/
├── platformio.ini          # Build configuration
├── src/
│   ├── project_config.h    # ← YOUR SETTINGS (GPIO, device name)
│   ├── main.cpp            # Entry point
│   ├── core/               # BLE, command parsing, motor management
│   ├── drivers/            # Motor driver implementations
│   └── profiles/           # Motion profiles (mecanum, etc.)
└── doc/                    # Documentation
```

## Supported Motor Drivers

| Driver | Type | Notes |
|--------|------|-------|
| DRV8871 | DC Motor | 2 PWM pins per motor |
| DRV8833 | DC Motor | Dual H-bridge, 2 motors per chip |
| L298N | DC Motor | Dual H-bridge with enable pins |
| TMC2209 | Stepper | Step/dir, UART optional |
| A4988 | Stepper | Step/dir, basic driver |
| DRV8825 | Stepper | Step/dir, up to 1/32 microstepping |

## Feature Comparison with ESP32

| Feature | ESP32 | Pico W |
|---------|-------|--------|
| BLE Control | ✅ | ✅ |
| DC Motors | ✅ | ✅ |
| Steppers | ✅ | ✅ |
| Mecanum Kinematics | ✅ | ✅ |
| Safety Watchdog | ✅ | ✅ |
| Encoders | ✅ | ⏳ (planned) |
| PID Control | ✅ | ⏳ (planned) |
| Autotuning | ✅ | ⏳ (planned) |

## BLE Pairing

1. Power on robot
2. Open Flutter app → Connection screen
3. Scan for devices → Select your robot name
4. Enter 6-digit PIN (default: 123456)
5. Connected!

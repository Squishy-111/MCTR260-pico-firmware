# MechaPico MCB Stepper Motor GPIO Mapping Reference

This document provides the complete GPIO pin mappings for the stepper motor connections on the MechaPico Motor Control Board (MCB).

## Architecture Overview

The MechaPico MCB uses **two MCP23017 16-bit I2C GPIO Expanders** to control the stepper motors. The Pico W does **NOT** drive the steppers directly via its GPIO pins; instead, it communicates with the MCP23017 chips over I2C.

```
┌─────────────────────────┐        I2C Bus        ┌──────────────────────────┐
│  Pimoroni Pico          │◄──────────────────────►│  MCP23017 (U6_1)         │
│  Inventor 2040 W        │   SDA, SCL             │  I2C Address: 0x20       │
│                         │                         │  (Stepper Control)       │
│  GPIO 4  = SDA          │                         └──────────────────────────┘
│  GPIO 5  = SCL          │
│  GPIO 22 = INT          │                         ┌──────────────────────────┐
│                         │◄──────────────────────►│  MCP23017 (U6_2)         │
└─────────────────────────┘                         │  I2C Address: 0x21       │
                                                    │  (DC Motor/LED Control)  │
                                                    └──────────────────────────┘
```

---

## Pico W I2C Pins (Pico Inventor Board)

The Pico Inventor 2040 W uses a level shifter for I2C communication between 3.3V (Pico) and 5V (MCP23017).

| Signal | Pico Inventor Connector | Pico W GPIO | Level Shifted |
|--------|-------------------------|-------------|---------------|
| SDA    | Pin 2                   | **GPIO 4**  | 5V on bus     |
| SCL    | Pin 3                   | **GPIO 5**  | 5V on bus     |
| INT    | Pin 4                   | **GPIO 22** | 5V on bus     |
| 3V3    | Pin 1                   | 3.3V        | -             |
| GND    | Pin 5                   | GND         | -             |

---

## MCP23017 GPIO Expander #1 (U6_1) - Stepper Motors

**I2C Address: `0x20`** (A0=GND, A1=GND, A2=GND)

### Port B (GPB0-GPB7) - Stepper Motor STEP/DIR

| MCP23017 Pin | Signal Name   | Function                |
|--------------|---------------|-------------------------|
| GPB0 (Pin 1) | STPR_M1_DIR   | Motor 1 Direction       |
| GPB1 (Pin 2) | STPR_M1_STEP  | Motor 1 Step Pulse      |
| GPB2 (Pin 3) | STPR_M2_DIR   | Motor 2 Direction       |
| GPB3 (Pin 4) | STPR_M2_STEP  | Motor 2 Step Pulse      |
| GPB4 (Pin 5) | STPR_M3_DIR   | Motor 3 Direction       |
| GPB5 (Pin 6) | STPR_M3_STEP  | Motor 3 Step Pulse      |
| GPB6 (Pin 7) | STPR_M4_DIR   | Motor 4 Direction       |
| GPB7 (Pin 8) | STPR_M4_STEP  | Motor 4 Step Pulse      |

### Port A (GPA0-GPA7) - Motor 5 + Shared Control

| MCP23017 Pin   | Signal Name     | Function                        |
|----------------|-----------------|----------------------------------|
| GPA0 (Pin 21)  | LED_BAR_1       | LED Bar Output 1                |
| GPA1 (Pin 22)  | STPR_M5_STEP    | Motor 5 Step Pulse              |
| GPA2 (Pin 23)  | STPR_M5_DIR     | Motor 5 Direction               |
| GPA3 (Pin 24)  | STPR_ALL_EN     | **Shared Enable (active LOW)**  |
| GPA4 (Pin 25)  | STPR_ALL_MS1    | Microstepping Config 1          |
| GPA5 (Pin 26)  | STPR_ALL_MS2    | Microstepping Config 2          |
| GPA6 (Pin 27)  | STPR_ALL_SPREAD | SpreadCycle Mode                |
| GPA7 (Pin 28)  | STPR_ALL_PDN    | Power Down (active LOW)         |

---

## MCP23017 GPIO Expander #2 (U6_2) - DC Motors & LEDs

**I2C Address: `0x21`** (A0=VCC, A1=GND, A2=GND)

### Port B (GPB0-GPB7) - General Purpose 5V I/O

| MCP23017 Pin | Signal Name  | Function               |
|--------------|--------------|------------------------|
| GPB0 (Pin 1) | 5V_IO_0      | General Purpose I/O 0  |
| GPB1 (Pin 2) | 5V_IO_1      | General Purpose I/O 1  |
| GPB2 (Pin 3) | 5V_IO_2      | General Purpose I/O 2  |
| GPB3 (Pin 4) | 5V_IO_3      | General Purpose I/O 3  |
| GPB4 (Pin 5) | 5V_IO_4      | General Purpose I/O 4  |
| GPB5 (Pin 6) | 5V_IO_5      | General Purpose I/O 5  |
| GPB6 (Pin 7) | 5V_IO_6      | General Purpose I/O 6  |
| GPB7 (Pin 8) | 5V_IO_7      | General Purpose I/O 7  |

### Port A (GPA0-GPA7) - DC Motor Control & LEDs

| MCP23017 Pin   | Signal Name  | Function               |
|----------------|--------------|------------------------|
| GPA0 (Pin 21)  | IN_MOT_4N    | DC Motor 4 Negative    |
| GPA1 (Pin 22)  | IN_MOT_4P    | DC Motor 4 Positive    |
| GPA2 (Pin 23)  | IN_MOT_3N    | DC Motor 3 Negative    |
| GPA3 (Pin 24)  | IN_MOT_3P    | DC Motor 3 Positive    |
| GPA4 (Pin 25)  | LED_BAR_2    | LED Bar Output 2       |
| GPA5 (Pin 26)  | LED_BAR_3    | LED Bar Output 3       |
| GPA6 (Pin 27)  | LED_BAR_4    | LED Bar Output 4       |
| GPA7 (Pin 28)  | LED_BAR_5    | LED Bar Output 5       |

---

## Stepper Motor Summary

| Motor | STEP Signal   | DIR Signal   | MCP23017 | STEP Bit | DIR Bit |
|-------|---------------|--------------|----------|----------|---------|
| M1    | STPR_M1_STEP  | STPR_M1_DIR  | U6_1     | GPB1     | GPB0    |
| M2    | STPR_M2_STEP  | STPR_M2_DIR  | U6_1     | GPB3     | GPB2    |
| M3    | STPR_M3_STEP  | STPR_M3_DIR  | U6_1     | GPB5     | GPB4    |
| M4    | STPR_M4_STEP  | STPR_M4_DIR  | U6_1     | GPB7     | GPB6    |
| M5    | STPR_M5_STEP  | STPR_M5_DIR  | U6_1     | GPA1     | GPA2    |

### Shared Stepper Control Signals (U6_1 Port A)

| Signal Name     | MCP23017 Pin | Description                              |
|-----------------|--------------|------------------------------------------|
| STPR_ALL_EN     | GPA3         | Enable all steppers (active LOW)         |
| STPR_ALL_MS1    | GPA4         | Microstepping config bit 1               |
| STPR_ALL_MS2    | GPA5         | Microstepping config bit 2               |
| STPR_ALL_SPREAD | GPA6         | SpreadCycle mode (TMC2209)               |
| STPR_ALL_PDN    | GPA7         | Power down (active LOW, TMC2209)         |

---

## Firmware Implementation Guide

### 1. Initialize I2C Bus

```cpp
// Pico W I2C Configuration
#define I2C_SDA_PIN    4
#define I2C_SCL_PIN    5
#define I2C_FREQUENCY  400000  // 400 kHz

i2c_init(i2c0, I2C_FREQUENCY);
gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
gpio_pull_up(I2C_SDA_PIN);
gpio_pull_up(I2C_SCL_PIN);
```

### 2. MCP23017 I2C Addresses

```cpp
#define MCP23017_STEPPER_ADDR  0x20  // U6_1 - Stepper motors
#define MCP23017_DCMOTOR_ADDR  0x21  // U6_2 - DC motors & LEDs
```

### 3. MCP23017 Register Definitions

```cpp
// MCP23017 Registers (IOCON.BANK = 0, default)
#define MCP23017_IODIRA    0x00  // I/O Direction Register Port A
#define MCP23017_IODIRB    0x01  // I/O Direction Register Port B
#define MCP23017_GPIOA     0x12  // GPIO Port A
#define MCP23017_GPIOB     0x13  // GPIO Port B
#define MCP23017_OLATA     0x14  // Output Latch Port A
#define MCP23017_OLATB     0x15  // Output Latch Port B
```

### 4. Stepper Motor Bit Masks

```cpp
// Port B - Individual Motor STEP/DIR
#define STPR_M1_DIR_BIT   (1 << 0)  // GPB0
#define STPR_M1_STEP_BIT  (1 << 1)  // GPB1
#define STPR_M2_DIR_BIT   (1 << 2)  // GPB2
#define STPR_M2_STEP_BIT  (1 << 3)  // GPB3
#define STPR_M3_DIR_BIT   (1 << 4)  // GPB4
#define STPR_M3_STEP_BIT  (1 << 5)  // GPB5
#define STPR_M4_DIR_BIT   (1 << 6)  // GPB6
#define STPR_M4_STEP_BIT  (1 << 7)  // GPB7

// Port A - Motor 5 + Shared Control
#define LED_BAR_1_BIT     (1 << 0)  // GPA0
#define STPR_M5_STEP_BIT  (1 << 1)  // GPA1
#define STPR_M5_DIR_BIT   (1 << 2)  // GPA2
#define STPR_ALL_EN_BIT   (1 << 3)  // GPA3 - Active LOW
#define STPR_ALL_MS1_BIT  (1 << 4)  // GPA4
#define STPR_ALL_MS2_BIT  (1 << 5)  // GPA5
#define STPR_ALL_SPRD_BIT (1 << 6)  // GPA6
#define STPR_ALL_PDN_BIT  (1 << 7)  // GPA7 - Active LOW
```

### 5. Microstepping Configuration (MS1/MS2)

| MS1 | MS2 | Microstepping |
|-----|-----|---------------|
| 0   | 0   | Full step     |
| 1   | 0   | 1/2 step      |
| 0   | 1   | 1/4 step      |
| 1   | 1   | 1/8 step      |

---

## Source Information

- **KiCad Project**: `motor-control-pcbs-main/MCB/mecha_pico_mcb/`
- **Schematic Files**: `gpio_expander.kicad_sch`, `stepper_motor_drv.kicad_sch`
- **PCB File**: `mecha_pico_mcb.kicad_pcb`
- **Author**: Stephine Yearley, University of Alberta
- **License**: CC BY-NC 4.0
# Semi-Automated Roll-Fed Bottle Labelling Prototype

**Team A17 | SSN College of Engineering | 2026**  
*B.E. Mechanical Engineering Mini Project*

A low-cost, standalone semi-automatic labelling machine for cylindrical containers. Built from scratch using Arduino, stepper motors, 3D printed PLA parts, and a custom breadboard circuit — total cost under ₹10,000.

---

## What It Does

Place a cylindrical bottle on the rollers. Set the label length on the LCD. Press the encoder knob. The machine feeds exactly that length of label while simultaneously rotating the bottle to apply it.

- Label lengths: **15mm to 500mm** (adjustable in 5mm increments via rotary encoder)
- Container diameters: **30mm to 120mm**
- Trigger: **manual encoder push-button**
- No computer required — fully standalone

---

## System Overview

Two mechanical subsystems working in sync:

**Label Delivery (X-Axis)**  
NEMA 17 → GT2 timing belt → drive rod with rubber rollers → backing paper pulled over a 45° peeling wedge → label peels off and lands on the spinning bottle

**Bottle Rotation (Y-Axis)**  
NEMA 17 → flexible jaw coupling → rubber-sleeved steel rod → bottle spins via surface friction

Both motors step simultaneously through the same loop in firmware — perfectly synchronized.

---

## Hardware

| Component | Spec | Qty |
|---|---|---|
| Microcontroller | Arduino Nano (ATmega328P) | 1 (+ 1 spare) |
| Stepper Motors | NEMA 17, 1.8°/step, 4.2 kg·cm | 2 |
| Motor Drivers | A4988 Microstepping Driver | 2 (+ spares) |
| Frame | 2020 T-slot Aluminum Extrusion, Al6063-T6 | 1.5m |
| Drive Shafts | 8mm Ground Stainless Steel Linear Rods | 2 |
| Drive Rollers | 20mm OD Industrial Rubber Sleeves | 4 |
| Display | 16x2 I2C LCD (PCF8574T backpack) | 1 |
| Input | HW-040 Rotary Encoder Module | 1 |
| Power Supply | 12V DC 5A Switching Adapter | 1 |
| Bearings | 608zz Ball Bearings | bulk |
| 3D Print Material | 1.75mm PLA Filament | 0.5 kg |
| Fasteners | M3/M4 and M5 Screws, T-Nuts | bulk |

**Total build cost: under ₹10,000**

---

## Circuit

Built entirely on a prototyping breadboard — no prefabricated shields.  
Two isolated power rails: **5V logic** (Arduino onboard regulator) + **12V motor power** (external adapter). Common ground throughout.

### Pin Mapping

| Arduino Pin | Connected To | Function |
|---|---|---|
| D2 | A4988 #1 — STEP | X-Axis label feed step pulse |
| D3 | A4988 #2 — STEP | Y-Axis bottle rotation step pulse |
| D5 | A4988 #1 — DIR | X-Axis direction control |
| D6 | A4988 #2 — DIR | Y-Axis direction control |
| D9 | KY-040 — CLK | Encoder rotation clock |
| D10 | KY-040 — DT | Encoder rotation data |
| D11 | KY-040 — SW | Encoder push-button / dispense trigger |
| A4 (SDA) | LCD I2C — SDA | I2C data line |
| A5 (SCL) | LCD I2C — SCL | I2C clock line |
| 5V | LCD, Encoder, A4988 VDD | Logic power rail |
| GND | All components | Common ground |

→ **[Interactive Wokwi Simulation](https://wokwi.com/projects/464706010612204545)** — full breadboard layout, both drivers, encoder, and LCD

---

## Firmware

Written in C++ (Arduino IDE). Runs as a deterministic state machine — motor pulses are never interrupted by interface polling.

**Libraries:**
- `Wire.h` — I2C communication with LCD backpack
- `LiquidCrystal_I2C.h` — LCD control over I2C
- `KY040.h` — rotary encoder direction + button reading

**Key constants:**

```cpp
#define STEPS_PER_MM    3
#define STEP_DELAY_US   4000    // microseconds between step pulses
const int smallest_value = 10; // mm
const int largest_value  = 500; // mm
```

**State flow:**
1. **Boot** → initialize I2C, configure pins, display home screen
2. **Standby** → LCD shows label length, encoder adjusts in 5mm steps via PCINT interrupt
3. **Trigger** → encoder push-button detected, transition to execution
4. **Execute** → step loop drives both motors simultaneously for `v_value × STEPS_PER_MM` steps
5. **Complete** → LCD shows confirmation, resets to standby

Full source: [`code_a17.ino`](code_a17.ino)

---

## What Went Wrong

Full breakdown in [`issues_a17.pdf`](issues_a17.pdf). Short version:

**CNC Shield V4 — manufacturing defect (critical)**  
Clone shield had incorrect copper trace mapping — cross-wired STEP and DIRECTION registers, bypassed current-limiting resistors. First 12V power-up instantly fried multiple A4988 drivers and destroyed the primary Arduino Nano.  
→ Scrapped the shield entirely. Rebuilt on a heavy-duty prototyping breadboard with point-to-point jumper wiring.

**GT2 Belt Tension**  
Too loose = slipping on pulley teeth, no label movement. Too tight = high-frequency vibration and skewed labels. Pulley grub screws weren't seated on shaft flats so the pulley was spinning freely on the shaft.  
→ Re-seated grub screws on shaft flats. Calibrated tension using the pluck test. Increased `STEP_DELAY_US` from 2000 to 4000µs to kill the resonance.

**AC Foam Rollers**  
Foam compressed unevenly under bottle weight, changing the effective rolling radius mid-operation and destroying step-to-distance calibration.  
→ Switched to 20mm OD industrial rubber sleeves with high shore hardness.

**IR Sensor**  
Transparent glass jars: not detected. Reflective PET plastic: false triggers. Wasn't reliably fixable with placement or sensitivity tuning.  
→ Removed the sensor entirely. Machine is now semi-automatic, triggered by the encoder push-button. More reliable and operator-safe.

**Motor Direction Mismatch**  
Both NEMA 17s drove in opposite directions, fighting each other, no net movement.  
→ Single firmware fix: set DIR pin of second A4988 to HIGH.

---

`Arduino` `C++` `NEMA 17` `A4988` `I2C` `GT2 Timing Belt` `PLA` `2020 Extrusion`

---

*Department of Mechanical Engineering — SSN College of Engineering*  
*Mini Project, B.E. Mechanical Engineering, May 2026*

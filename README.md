# IoT People Counter using IR Sensors

A break-beam IR sensor pair + Arduino Uno system that counts people entering/exiting a room in real time, displays live IN/OUT/Current counts on a 16×2 I²C LCD, and triggers a buzzer alarm when occupancy hits a configurable limit. Built and tested to zero counting errors across 50 controlled crossings.

Published in **IJARCCE, Vol. 15, Issue 4, April 2026** — [10.17148/IJARCCE.2026.154156](https://doi.org/10.17148/IJARCCE.2026.154156)

---

## Why

A shared lab was rated for 20 people but regularly hit 25+ on busy afternoons, with no automated way to track it. This project explores whether a cheap, reliable people counter could be built for under $15 — and documents what *didn't* work along the way, because the debounce logic took three attempts to get right.

## How it works

Two HW-201 break-beam IR modules sit on either side of a doorframe (entry + exit). When a person interrupts the beam, the Arduino reads the transition and updates a running count — incrementing on entry, decrementing on exit — shown live on an LCD, with a buzzer that fires once per normal crossing and three times if the room is full.

```
Arduino Uno
├── D2  → Entry IR sensor (INT0-capable pin)
├── D3  → Exit IR sensor  (INT1-capable pin)
├── D4  → Buzzer
├── A4  → LCD SDA (I²C)
└── A5  → LCD SCL (I²C)
```

## The core problem: debouncing a break-beam sensor

A naive "read pin, increment if LOW" loop overcounts massively — a 16 MHz Uno polls fast enough that a single 400 ms beam-break registers as hundreds of thousands of increments. Three approaches were tried before landing on one that held up:

| Attempt | Approach | Result |
|---|---|---|
| 1 | Read + increment every loop | Counted a 10-crossing test as ~35 |
| 2 | Fixed 1s `delay()` after each read | Missed fast back-to-back crossings, 1s display lag |
| 3 | Rising-edge detection | Double-counted ~15% of slow-walk trials (signal bounce) |
| **4 (final)** | **Per-sensor latch**: fire the handler once when the beam breaks, hold for 500 ms, only re-arm once the beam clears | **0 errors across 50 test crossings** |

The 500 ms hold value was found empirically — 200 ms let a mid-crossing hesitation re-trigger the count, 750 ms started missing closely-following people. It's documented as a practical fix, not a first-principles one; see [Limitations](#limitations--future-work).

Full reasoning for each failed attempt (including why camera-based and ultrasonic counting were ruled out first) is in the [published paper](#publication).

## Hardware

| Part | Spec | Qty |
|---|---|---|
| Arduino Uno Rev3 | ATmega328P, 16 MHz, 5V | 1 |
| HW-201 IR module | Break-beam, digital out, adjustable sensitivity | 2 |
| 16×2 LCD | I²C via PCF8574 backpack (addr `0x27`) | 1 |
| Active buzzer | 5V, self-oscillating | 1 |
| Breadboard | 830 tie-point | 1 |
| Jumper wires | M-M / M-F, assorted | ~20 |

**Total cost: under $15.**

## Getting started

### Wiring
Connect sensors and peripherals per the pin table above. D2/D3 are used deliberately — they expose the Uno's hardware interrupt lines (INT0/INT1) for a future upgrade, even though the current firmware uses a polled loop.

### Upload
1. Open the sketch in the Arduino IDE.
2. Install the LCD I²C library (`LiquidCrystal_I2C` or equivalent) if not already present.
3. Select **Arduino Uno** as the board, select the correct port, and upload.
4. Power on — the LCD shows a 4-second splash sequence (sensor settling time) before the counter goes live.

### Adjust sensitivity
Trim the HW-201 potentiometers until the onboard LED responds cleanly to a hand passing through the beam at mounting height. Recalibrate if ambient lighting changes significantly.

## Testing summary

Tested in a second-floor corridor under fluorescent lighting (290–350 lux):

| Metric | Result |
|---|---|
| Crossings tested (25 in + 25 out) | 50 |
| Correct counts | 50/50 (100%) |
| Full-room alarm triggers | 10/10 correct |
| Slow-walk (~0.4 m/s) double-counts | 0/5 |
| Mid-crossing pause handling | Correct |
| Display refresh time | ~1.4 ms |
| Sensor-to-buzzer latency | ~3.6 ms (excl. 500 ms hold) |
| Peak current draw @ 5V | ~255 mA |

> **Note on the 100% figure:** this covers one person at a time, in controlled, sequential crossings. Two people passing simultaneously in opposite directions is untested and, logically, undefined for a two-sensor sequential scheme — not a realistic case for the target use (labs, server rooms, small classrooms), but worth stating plainly.

## Limitations & future work

- **500 ms blocking hold**: the main loop doesn't read sensors during the hold, so traffic above ~2 crossings/second will start missing events. The fix is moving detection onto hardware interrupts (D2/D3 already expose INT0/INT1 for this) with a non-blocking timer instead of `delay()` — not yet implemented.
- **No persistence across power loss**: counts reset to zero on reboot. The ATmega328P's onboard 1KB EEPROM is unused; writing counters there after each update (~3 ms/write) would fix this for unreliable power environments.
- **Simultaneous opposite-direction crossings** are untested and undefined for this sensor arrangement.

## Publication

**IOT PROJECT: PEOPLE COUNTER USING IR SENSORS**
*Sreejith S, Durai Raj R, Manish Kumar Mandal, Aravind Sriram, Deepak G*
Under the guidance of Ms. Charulatha R T, Assistant Professor, Dept. of CSE
Department of Computer Science and Engineering, SRM Institute of Science and Technology, Vadapalani, Chennai

Published in *International Journal of Advanced Research in Computer and Communication Engineering (IJARCCE)*, ISSN (Online) 2278-1021, ISSN (Print) 2319-5940 — Vol. 15, Issue 4, April 2026.
DOI: [10.17148/IJARCCE.2026.154156](https://doi.org/10.17148/IJARCCE.2026.154156)

## Team

Built by **Team Nova**: Sreejith S · Durai Raj R · Manish Kumar Mandal · Aravind Sriram · Deepak G

## Acknowledgements

Thanks to the CSE lab staff for the corridor access, and to our project guide, whose comment that "your debounce won't survive a slow walker" turned out to be exactly right.


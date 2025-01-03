
# Resurrection for an Akai-APC40 MKI using Teensy 4.1

## Brainstorm

### Static parts

The following objects are hard-wired and not subject to change:

- Hardware pinout:
  - Leds / MCP / i2c busses: assignation are static
  - Inputs gpio assignation / multiplexer addresses
  - MIDI buses (RX1/TX1 + RX2/TX2)
  - Error led pin (PIN-6)
- IC choise:
  - MCP23017 (i2c gpio expander) for leds annode driving (x3)
  - CD74AC138 (3-lines to 8-lines demux) for:
    - leds cathodes driving
    - inputs sink (mux diode annode) select

This is summarised by the following schematics
![](<doc/Teensy41 Pinout attribution V02.png>)
![alt text](<doc/IO Board - 4x mpx.png>)

Some CV outputs and inputs might be added:

- `A0` and `A1` will be used for analog input, multiplexed with HP4067 ICs (1x16 analog mux/demux)
- One out of the three i2c bus might be used for PWM output via PCA9685 (i2c 16-outs PWM expander)

### Dynamic parts

The following are software based and should be modifiable throught configuration files:

- MIDI messages attribution:
  - At least a mode for compatibility with the original device
  - An extended mode with additionnal messages or values for new display patterns, or actions
- User interaction patterns should be dynamic, to allow different uses:
  - step sequencer:
    - pad matrix to edit active steps
    - track select to edit a single step thought knobs
  - cv / midi sampler:
    - pad matrix to store patterns
    - track select to edit a single pattern
- Device state: should be encoded in human readable format

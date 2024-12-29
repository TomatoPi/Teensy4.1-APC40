/*
* Useful links: 
*   https://github.com/Richard-Gemmell/teensy4_i2c/blob/master/examples/wire/read_sensor/read_sensor.ino
*   
*/

#include <Arduino.h>
#include <usb_serial.h>
#include <SD.h>

#include <MIDI.h>

#include <i2c_device.h>
#include <imx_rt1060/imx_rt1060_i2c_driver.h>

#include "logging.hpp"
#include "errno.hpp"
#include "async.hpp"

#include "MultiplexDriver.hpp"

#include "src/hw/IOdef.hpp"

bool hw::async::DebugAsync = true;

struct global_flags_t
{
    bool serial_available;  ///< True if serial port is connected
    bool sd_available;      ///< True if sd card is inserted
} global_flags;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIA);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDIB);

bool setup_raw_io()
{
    for (const auto pin: hw::OUTPUT_PINS)
        { pinMode(pin, OUTPUT); }           ///< Pins used for all raw outs

    for (uint8_t pin=26; pin<=39; ++pin)
        { pinMode(pin, INPUT_PULLUP); }     ///< Pads source pins (active @low)
    for (uint8_t pin=20; pin<=23; ++pin)
        { pinMode(pin, INPUT_PULLUP); }     ///< Encoders input pins (active ??)
    
    pinMode(A16, INPUT_DISABLE);            ///< Master faders polling pins
    pinMode(A17, INPUT_DISABLE);            ///< Track faders polling pins

    pinMode(LED_BUILTIN, OUTPUT);           ///< Used for heart beat
    Serial.begin(9600);                     ///< USB Serial IO

    MIDIA.begin(MIDI_CHANNEL_OMNI);         ///< Connection to MIDI matrix
    MIDIB.begin(MIDI_CHANNEL_OMNI);         ///< Connection to Foot controler

        /**
         * Almost done: wait for serial to begin if available
         */

    elapsedMillis elapsed;
    while (!Serial && elapsed < 2000)
        { /* nothing */ }

        /**
         * Reports serial state in global bits
         */

    return static_cast<bool>(Serial);
}

bool setup_sd_card()
{
    if (!SD.begin(BUILTIN_SDCARD))
        { return false; }

    if (logging::is_logging_enabled_for(logging::severity::DEBUG))
        { Serial.println("SD Card initialised"); }

    return true;
}

void setup_i2c_buses()
{
    constexpr const size_t MASTERS_COUNT = 3;
    I2CMaster* masters[MASTERS_COUNT] = {&Master, &Master1, &Master2};

    for (size_t i=0; i<MASTERS_COUNT; ++i)
    {
        /**
         * @warning Pullup are mandatory as I2C is interrupt driven
         *  they should only be disabled with a complete hardware
         *  that is proper board with extern pullups
         */
        masters[i]->set_internal_pullups(InternalPullup::enabled_100k_ohm);
        masters[i]->begin(400'000);
    }
}

void setup() {
    
    elapsedMicros elapsed;

    // Clear all global bits
    memset(&global_flags, 0, sizeof(global_flags_t));
    if (Serial && logging::is_logging_enabled_for(logging::severity::DEBUG))
        { Serial.printf("global flags initialised in %lu(us)\n", static_cast<unsigned long>(elapsed)); }

        /**
         * Setup raw hardware
         */
    
    elapsed = 0;
    global_flags.serial_available = setup_raw_io();
    if (Serial && logging::is_logging_enabled_for(logging::severity::DEBUG))
    {
        if (global_flags.serial_available)
            { Serial.printf("Serial Initialised in %lu(us)\n", static_cast<unsigned long>(elapsed)); }
        else
            { /* can't print failure to init serial */ }
    }

    elapsed = 0;
    global_flags.sd_available = setup_sd_card();
    if (Serial && logging::is_logging_enabled_for(logging::severity::DEBUG))
    {
        if (global_flags.sd_available)
            { Serial.printf("SD card Initialised in %lu(us)\n", static_cast<unsigned long>(elapsed)); }
        else
            { Serial.printf("Failed init SD card: time taken %lu(us)\n", static_cast<unsigned long>(elapsed)); }
    }

    elapsed = 0;
    setup_i2c_buses();
    if (Serial && logging::is_logging_enabled_for(logging::severity::DEBUG))
        { Serial.printf("I2C buses initialised in %lu(us)\n", static_cast<unsigned long>(elapsed)); }

        /**
         * Setup software facilities
         */
    
    elapsed = 0;
    hw::LedsDriver::GlobalDriver.setup(800);
    if (Serial && logging::is_logging_enabled_for(logging::severity::DEBUG))
        { Serial.printf("\nLedsDriver initialised in %lu(us)\n", static_cast<unsigned long>(elapsed)); }


        /**
         * Done
         */

    // pinMode(LED_BUILTIN, OUTPUT);       ///< Used for heart beat
    // Serial.begin(9600);                 ///< USB Serial IO

    // elapsedMillis elapsed;
    // while (!Serial) // && elapsed < 2000)
    //     { /* nothing */ }

    // for (size_t row=0; row<5; ++row)
    const uint8_t row=4;
    for (size_t col=0; col<8; ++col)
    {
        hw::LedsDriver::GlobalDriver.safe_write(hw::ios::CLIP_Y_X[row][col]->led, true);
    }

    if (Serial)
    {
        Serial.printf("\nSetup done in %lu(us)\n", static_cast<unsigned long>(elapsed));
        Serial.printf("\tsd_available=%d\n", static_cast<int>(global_flags.sd_available));
    }

    hw::LedsDriver::DebugDriver = false;
    hw::async::DebugAsync = false;
}

void loop() {

    static elapsedMillis elapsed_since_heartbeat = 0;
    static constexpr const unsigned long heartbeat_ms = 100;
    static bool heartbeat_state = true;

    if (heartbeat_ms < elapsed_since_heartbeat)
    {
        digitalWrite(LED_BUILTIN, heartbeat_state ? HIGH : LOW);
        elapsed_since_heartbeat = 0;
        heartbeat_state = !heartbeat_state;
    }

    elapsedMicros elapsed;
    hw::LedsDriver::GlobalDriver.update();
    // Serial.printf("%lu\n", static_cast<unsigned long>(elapsed));

    // delay(100);
}

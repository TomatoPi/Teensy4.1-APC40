/**
 * 
 */

#ifndef DEF_HARDWARE_HPP
#define DEF_HARDWARE_HPP

#include <Arduino.h>
#include <cstdint>
#include <type_traits>

#include "devices/MCP23017.hpp"
#include "async_i2c.hpp"

namespace hw
{

    /**
     * Teensy board has more pins, but we're only using standard headers,
     *  remaining pins are normaly reserved for external QSPI memory and SD card
     */
    static constexpr const uint8_t TEENSY_PINS_COUNT = 42;

    /**
     * Number of colum addressing pins, common for inputs and outputs
     */
    static constexpr const uint8_t MULTIPLEX_COLUMS_COUNT = 8;

    /**
     * List of connected MCP io multiplexers @see <doc/Teensy41 Pinout attribution V02.png>
     */
    enum class mcp: uint8_t
    {
        MCP_0 = 0,
        MCP_1 = 1,
        MCP_2 = 2,
        COUNT = 3
    };
    /**
     * Derived from the enum 'mcp', used for index based access of devices
     */
    static constexpr const uint8_t MCP_COUNT = static_cast<uint8_t>(mcp::COUNT);
    using mcp_count_type = std::remove_const_t<decltype(MCP_COUNT)>;
    /**
     * Index based global access for MCP23017 devices
     */
    extern async::async_device MCP_DEVICES[MCP_COUNT];

    /**
     * Leds cathode addressing pins
     */
    static constexpr const uint8_t CATHODE_ADDR_PINA0 = 9;
    static constexpr const uint8_t CATHODE_ADDR_PINA1 = 10;
    static constexpr const uint8_t CATHODE_ADDR_PINA2 = 11;
    static constexpr const uint8_t CATHODE_ENABLE_PIN = 12;

    /**
     * Inputs sink addressing pins
     */
    static constexpr const uint8_t INPUT_SINK_ADDR_PINA0 = 2;
    static constexpr const uint8_t INPUT_SINK_ADDR_PINA1 = 3;
    static constexpr const uint8_t INPUT_SINK_ADDR_PINA2 = 4;
    static constexpr const uint8_t INPUT_SINK_ENABLE_PIN = 5;

    /**
     * Pin with a led attached to signal presence of errors to the user
     */
    static constexpr const uint8_t ERROR_PIN = 6;

    /**
     * List of all output pins
     */
    static constexpr const uint8_t OUTPUT_PINS[9] = {
        CATHODE_ADDR_PINA0, CATHODE_ADDR_PINA1, CATHODE_ADDR_PINA2, CATHODE_ENABLE_PIN,
        INPUT_SINK_ADDR_PINA0, INPUT_SINK_ADDR_PINA1, INPUT_SINK_ADDR_PINA2, INPUT_SINK_ENABLE_PIN,
        ERROR_PIN
    };

} /* endof namespace hw */

#endif /* DEF_HARDWARE_HPP */

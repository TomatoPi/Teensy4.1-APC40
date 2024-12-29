/**
 * 
 */

#ifndef DEF_GLOBAL_HPP
#define DEF_GLOBAL_HPP

namespace global
{

    struct flags_t
    {
        bool serial_available;          ///< True if serial port is connected
        bool sd_available;              ///< True if sd card is inserted and recognised
        bool leds_driver_available;     ///< True if all MCP are configured and ready
    };
}

#endif // DEF_GLOBAL_HPP

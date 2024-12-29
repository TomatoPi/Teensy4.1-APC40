/**
 * 
 */

#include <cstdint>
#include <cstddef>

#include <i2c_driver.h>

namespace hw
{

    namespace mcp23017
    {
        /**
         * PORTA and PORTB holds 8 pins each
         */
        static constexpr const uint8_t PINS_COUNT = 16;
        /**
         * MCP base address is 0x20 (32), which means avaible addresses range is 0x20 thru 0x27
         */
        static constexpr const uint8_t MCP_BASE_ADDR = 0x20;

        /**
         * 
         */
        enum class port: uint8_t
        {
            PORTA = 0,
            PORTB = 8
        };
        
        static constexpr uint8_t operator+ (const port p, const uint8_t idx)
            { return static_cast<uint8_t>(p) + idx; }

        /**
         * Registers addresses assuming IOCON.BANK bit is cleared
         */
        enum class registers: uint8_t
        {
            /// IO direction: Input or Output
            IODIRA = 0x00,
            IODIRB = 0x01,

            /// Configuration register
            IOCON = 0x0A,

            /// GPIO registers
            GPIOA = 0x12,
            GPIOB = 0x13,
        };

        constexpr uint8_t operator+ (const registers r, uint8_t offset)
            { return static_cast<uint8_t>(r) + offset; }

        /**
        * By default IOCON is filled with zeros, so IOCON.BANK is 0
        *   which means registers are sequencially addressed (see datasheet)
        * 
        * bits are: (LSB) NA - INTPOL - ODR - HAEN - DISSLW - SEQOP - MIRROR - BANK (MSB)
        */
        enum class IOCON_bit: uint8_t
        {
            INTPOL  = 0x02, ///< Set INT (interrupt) pins polarity, active with ODR cleared
                            ///<    0: active-low, 1: active-high

            ODR     = 0x04, ///< Open-Drain control for INT pins, overrides INTPOL register
                            ///<    0: active driver output, 1: open-drain output

            HAEN    = 0x08, ///< Hardware Address Enable, not used with I2C device
                            ///<    Not used here

            DISSLW  = 0x10, ///< SlewRate control for SDA bus
                            ///<    0: slew rate enable, 1: slew rate disable

            SEQOP   = 0x20, ///< Sequential operation mode
                            ///<    , allows write multiple registers without re-addressing
                            ///<    0: seqop enabled, 1: seqop disabled

            MIRROR  = 0x40, ///< INT pins mirror control
                            ///<    0: INTA <> PORTA, INTB <> PORTB, 1: INT pins are connected

            BANK    = 0x80, ///< Control registers addressing, @see enum registers (assume BANK is 0)
                            ///<    0: sequential registers addressing: (0x00, 0x01), (0x02, 0x03), ...
                            ///<    1: banked registers addressing: (0x00, 0x10), (0x01, 0x11), ...
        };

        constexpr uint8_t operator| (const uint8_t base, const IOCON_bit bit)
            { return base | static_cast<uint8_t>(bit); }

        constexpr uint8_t operator| (const IOCON_bit a, const IOCON_bit b)
            { return static_cast<uint8_t>(a) | static_cast<uint8_t>(b); }


    } // endof namespace MCP23017

} // endof namespace hw
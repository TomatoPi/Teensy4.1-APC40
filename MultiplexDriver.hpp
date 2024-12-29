/**
 * 
 */

#ifndef DEF_MULTIPLEXER_DRIVER_HPP
#define DEF_MULTIPLEXER_DRIVER_HPP

#include <cstdint>
#include <cstddef>

#include "errno.hpp"
// #include "async.hpp"

#include "src/hw/hardware.hpp"
#include "src/hw/addr.hpp"

namespace hw
{

    // static constexpr uint8_t LED_ERROR_DESCRIPTOR_COUNT = 3;
    // using led_err_stack = err::err_stack<led_error, LED_ERROR_DESCRIPTOR_COUNT>;
    
    class LedsDriver
    {
    public:
            
        struct error
        {
            hw::mcp fault_mcp;
            I2CError errcode;
        };

        enum class color: uint8_t
            { OFF=0x00, GREEN=0x01, RED=0x02, ORANGE=0x03 };

        static LedsDriver GlobalDriver;

        static bool DebugDriver;

        /**
         * Initialise I2C bus and MCP23017 registers
         */
        err::errcode setup(const unsigned long refresh_rate=20, const unsigned long timeout_ms=200);

        void clear_all();

        /**
         * 
         */
        err::errcode update();

        /**
         * 
         */
        err::errcode safe_write(const addr::output& addr, const bool state)
        {
            if (
                addr.mcp_index < MCP_COUNT
                && addr.annode_index < mcp23017::PINS_COUNT
                && addr.cathode_address < MULTIPLEX_COLUMS_COUNT
                )
                { return unsafe_write(addr, state); }
            else
                { return err::errcode::INVALID_ARGUMENT; }
        }


        /**
         * Set state of an ON-OFF led, change takes effect in few update calls
         */
        err::errcode write(const ios::monochrome_pad& pad, const bool state);
        err::errcode toggle(const ios::monochrome_pad& pad);
        err::errcode read(const ios::monochrome_pad& pad, bool* state) const;

        /**
         * Set state of bicolor led
         */
        err::errcode write(const ios::bicolor_pad& pad, const color c);
        err::errcode toggle(const ios::bicolor_pad& pad);
        err::errcode read(const ios::bicolor_pad& pad, color* c) const;

        /**
         * Set state of a ledring
         */
        err::errcode write(const ios::ring_encoder& encoder, const uint16_t state);
        err::errcode toggle(const ios::ring_encoder& encoder);
        err::errcode read(const ios::ring_encoder& encoder, uint16_t* state) const;

    private:

        enum class CycleState: uint8_t
        {
            READY,              ///< Driver is ready to begin a new cycle
            UPDATING_GPIOS,     ///< Writing GPIOS for all MCPs
            ENDING,             ///< Waiting for transmition ACK
        };

        err::errcode begin_cycle();
        err::errcode update_mcps();
        err::errcode end_cycle();


        err::errcode unsafe_write(const addr::output& addr, const bool state)
        {
            const uint8_t anode_bit = static_cast<uint8_t>(0x01) << addr.annode_index_in_reg();
            const uint8_t buffer_index = addr.is_port_A() ? 1 : 2;

            if (state)
                { _mcp_write_buffer[addr.cathode_address][addr.mcp_index][buffer_index] |= anode_bit; }
            else
                { _mcp_write_buffer[addr.cathode_address][addr.mcp_index][buffer_index] &= ~anode_bit; }
            
            return err::errcode::OK;
        }


        static const uint8_t WRITE_BUFFER_SIZE = 3;
        using buffer_size_t = decltype(WRITE_BUFFER_SIZE);

        uint8_t _mcp_write_buffer[MULTIPLEX_COLUMS_COUNT][MCP_COUNT][WRITE_BUFFER_SIZE];
        async::async_write _write_calls[MCP_COUNT];
        async::write_queue _write_nodes[MCP_COUNT];

        unsigned long _refresh_period;  ///< time between two write of all leds state (microsseconds)
        elapsedMicros _elapsed_time;    ///< time since last effective update
        CycleState _cycle_state;        ///< Driver's state within it's cycle
        uint8_t _current_index;         ///< Next colum to update

        // uint8_t _updated_mcps;          ///< bitfield tracking which MCPs where updated during this cycle

    };

    class InputsDriver
    {

    };

} // endof namespace hw

#endif // DEF_MULTIPLEXER_DRIVER_HPP5
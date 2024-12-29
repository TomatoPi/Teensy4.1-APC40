/**
 * 
 */

#ifndef DEF_IOS_HPP
#define DEF_IOS_HPP

#include <cstdint>
#include <cstddef>

#include "hardware.hpp"

namespace hw
{

    namespace addr
    {

        /**
         * Digital input address, uses an address decoder for sink enable, and a teensy pin for source read
         *  When corresponding sink is enabled, source pin will go LOW if the button is pressed
         */
        struct input
        {
            uint8_t source_pin;         ///< Input pin on Teensy
            uint8_t sink_address;       ///< Address for column decoder [0-7]

            template <uint8_t source_pin, uint8_t sink_address>
            static constexpr input make_safe()
            {
                static_assert(source_pin < TEENSY_PINS_COUNT);
                static_assert(sink_address < MULTIPLEX_COLUMS_COUNT);

                return input{
                    .source_pin = source_pin,
                    .sink_address = sink_address,
                };
            }

            constexpr bool sink_A() const
                { return static_cast<bool>(sink_address & 0x01); }

            constexpr bool sink_B() const
                { return static_cast<bool>(sink_address & 0x02); }

            constexpr bool sink_C() const
                { return static_cast<bool>(sink_address & 0x04); }
        };

        /**
         * Digital output address, uses an address decoder for common cathodes enable (active LOW)
         *  uses MCP23017 multiplexers for annodes enable (active UP)
         */
        struct output
        {
            uint8_t mcp_index;          ///< Index of annode driver [0-2]
            uint8_t annode_index;       ///< Pin on MCP multiplexer [0-7] on BANKA, [8-15] on BANKB
            uint8_t cathode_address;    ///< Address for column decoder [0-7]

            template <mcp mcp_index, uint8_t annode_index, uint8_t cathode_address>
            static constexpr output make_safe()
            {
                static_assert(static_cast<uint8_t>(mcp_index) < MCP_COUNT);
                static_assert(annode_index < mcp23017::PINS_COUNT);
                static_assert(cathode_address < MULTIPLEX_COLUMS_COUNT);

                return output{
                    .mcp_index = static_cast<uint8_t>(mcp_index),
                    .annode_index = annode_index,
                    .cathode_address = cathode_address,
                };
            }

            /**
             * Returns true if given led is on PORTA of it's register
             */
            constexpr bool is_port_A() const
                { return annode_index < 8; }
            /**
             * Returns true if given led is on PORTB of it's register
             */
            constexpr bool is_port_B() const
                { return !is_port_A(); }

            /**
             * Returns annode index within the corresponding register
             */
            constexpr uint8_t annode_index_in_reg() const
                { return annode_index % 8; }
        };

        /**
         * Rotary encoders using quadrature encoding on pinA and pinC
         */
        struct encoder
        {
            uint8_t mx_A_address;       ///< Index of pin A on analog multiplexer [0-7]
            uint8_t pinA;               ///< Teensy pin for A input
            uint8_t mx_C_address;       ///< Index of pin C on analog multiplexer [0-7]
            uint8_t pinC;               ///< Teensy pin for C input

            template <uint8_t mx_A_address, uint8_t pinA, uint8_t mx_C_address, uint8_t pinC>
            static constexpr encoder make_safe()
            {
                static_assert(mx_A_address < MULTIPLEX_COLUMS_COUNT);
                static_assert(pinA < TEENSY_PINS_COUNT);
                static_assert(mx_C_address < MULTIPLEX_COLUMS_COUNT);
                static_assert(pinC < TEENSY_PINS_COUNT);

                return encoder{
                    .mx_A_address = mx_A_address,
                    .pinA = pinA,
                    .mx_C_address = mx_C_address,
                    .pinC = pinC
                };
            }
        };

        /**
         * Analog fader
         */
        struct fader
        {
            uint8_t mx_address;         ///< Index on analog multiplexer [0-7]
            uint8_t pin;                ///< Corresponding teensy analog input

            template <uint8_t mx_address, uint8_t pin>
            static constexpr fader make_safe()
            {
                static_assert(mx_address < MULTIPLEX_COLUMS_COUNT);
                static_assert(pin < TEENSY_PINS_COUNT);

                return fader{
                    .mx_address = mx_address,
                    .pin = pin
                };
            }
        };

    } // endof namespace addr

    namespace ios
    {
        /**
         * Stop All Clips
         * 
         * Shift, Bank select, Nudge, Tap tempo
         * Play, Stop, Rec
         */
        struct blind_pad
        {
            addr::input addr;
        };
        /**
         * Clip stop, Scene launch, Track select, Activation, Solo, Rec arm
         * 
         * Pan, Send[A-C], Device controls
         */
        struct monochrome_pad: blind_pad
        {
            addr::output led;
        };
        /**
         * Clip launch zone
         */
        struct bicolor_pad: monochrome_pad
        {
            addr::output led1;
        };

        /**
         * Cue level
         */
        struct blind_encoder
        {
            addr::encoder addr;
        };
        /**
         * Tack control and Device control encoders
         */
        struct ring_encoder: blind_encoder
        {
            addr::output led_ring[15];
        };

        /**
         * Tracks and master faders, AB crossfade
         */
        struct fader
        {
            addr::fader addr;
        };

    } // endof namespace ios

} // endof namespace hw

#endif // DEF_IOS_HPP

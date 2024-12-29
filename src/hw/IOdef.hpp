/**
 * 
 */

#ifndef DEF_IO_DEFS_HPP
#define DEF_IO_DEFS_HPP

#include <Arduino.h>

#include "addr.hpp"
// #include "devices/MCP23017.hpp"

#define SCC                                     static constexpr const

#define MAKE_INPUT(SRC, SINK)                   hw::addr::input::make_safe<SRC, SINK>()
#define MAKE_OUTPUT(MCP_IDX, ANNODE, CATHODE)   hw::addr::output::make_safe<MCP_IDX, ANNODE, CATHODE>()
#define MAKE_ENCODER(ADDRA, PINA, ADDRC, PINC)  hw::addr::encoder::make_safe<ADDRA, PINA, ADDRC, PINC>()
#define MAKE_FADER(ADDR, PIN)                   hw::addr::fader::make_safe<ADDR, PIN>()

#define MAKE_BLIND_ROW(TYPE_T, SRC, A, B, C, D, E, F, G, H) \
    SCC TYPE_T A = { MAKE_INPUT(SRC, 0) }; \
    SCC TYPE_T B = { MAKE_INPUT(SRC, 1) }; \
    SCC TYPE_T C = { MAKE_INPUT(SRC, 2) }; \
    SCC TYPE_T D = { MAKE_INPUT(SRC, 3) }; \
    SCC TYPE_T E = { MAKE_INPUT(SRC, 4) }; \
    SCC TYPE_T F = { MAKE_INPUT(SRC, 5) }; \
    SCC TYPE_T G = { MAKE_INPUT(SRC, 6) }; \
    SCC TYPE_T H = { MAKE_INPUT(SRC, 7) }

#define MAKE_MONOCHROME_ROW(TYPE_T, SRC, MCP_IDX, ANNODE, A, B, C, D, E, F, G, H) \
    SCC TYPE_T A = { MAKE_INPUT(SRC, 0), MAKE_OUTPUT(MCP_IDX, ANNODE, 0) }; \
    SCC TYPE_T B = { MAKE_INPUT(SRC, 1), MAKE_OUTPUT(MCP_IDX, ANNODE, 1) }; \
    SCC TYPE_T C = { MAKE_INPUT(SRC, 2), MAKE_OUTPUT(MCP_IDX, ANNODE, 2) }; \
    SCC TYPE_T D = { MAKE_INPUT(SRC, 3), MAKE_OUTPUT(MCP_IDX, ANNODE, 3) }; \
    SCC TYPE_T E = { MAKE_INPUT(SRC, 4), MAKE_OUTPUT(MCP_IDX, ANNODE, 4) }; \
    SCC TYPE_T F = { MAKE_INPUT(SRC, 5), MAKE_OUTPUT(MCP_IDX, ANNODE, 5) }; \
    SCC TYPE_T G = { MAKE_INPUT(SRC, 6), MAKE_OUTPUT(MCP_IDX, ANNODE, 6) }; \
    SCC TYPE_T H = { MAKE_INPUT(SRC, 7), MAKE_OUTPUT(MCP_IDX, ANNODE, 7) }

#define MAKE_BICHROME_ROW(TYPE_T, SRC, midxA, annodeA, midxB, annodeB, A, B, C, D, E, F, G, H) \
    SCC TYPE_T A = { MAKE_INPUT(SRC, 0), MAKE_OUTPUT(midxA, annodeA, 0), MAKE_OUTPUT(midxB, annodeB, 0) }; \
    SCC TYPE_T B = { MAKE_INPUT(SRC, 1), MAKE_OUTPUT(midxA, annodeA, 1), MAKE_OUTPUT(midxB, annodeB, 1) }; \
    SCC TYPE_T C = { MAKE_INPUT(SRC, 2), MAKE_OUTPUT(midxA, annodeA, 2), MAKE_OUTPUT(midxB, annodeB, 2) }; \
    SCC TYPE_T D = { MAKE_INPUT(SRC, 3), MAKE_OUTPUT(midxA, annodeA, 3), MAKE_OUTPUT(midxB, annodeB, 3) }; \
    SCC TYPE_T E = { MAKE_INPUT(SRC, 4), MAKE_OUTPUT(midxA, annodeA, 4), MAKE_OUTPUT(midxB, annodeB, 4) }; \
    SCC TYPE_T F = { MAKE_INPUT(SRC, 5), MAKE_OUTPUT(midxA, annodeA, 5), MAKE_OUTPUT(midxB, annodeB, 5) }; \
    SCC TYPE_T G = { MAKE_INPUT(SRC, 6), MAKE_OUTPUT(midxA, annodeA, 6), MAKE_OUTPUT(midxB, annodeB, 6) }; \
    SCC TYPE_T H = { MAKE_INPUT(SRC, 7), MAKE_OUTPUT(midxA, annodeA, 7), MAKE_OUTPUT(midxB, annodeB, 7) }

#define MAKE_LED_RING(port, idx) \
    { \
        MAKE_OUTPUT(hw::mcp::MCP_1, port+1, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_1, port+2, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_1, port+3, idx), \
        \
        MAKE_OUTPUT(hw::mcp::MCP_1, port+4, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_1, port+5, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_1, port+6, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_1, port+7, idx), \
        \
        MAKE_OUTPUT(hw::mcp::MCP_0, port+0, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_0, port+1, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_0, port+2, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_0, port+3, idx), \
        \
        MAKE_OUTPUT(hw::mcp::MCP_0, port+4, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_0, port+5, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_0, port+6, idx), \
        MAKE_OUTPUT(hw::mcp::MCP_0, port+7, idx), \
    }

#define MAKE_RING_ENCODER(TYPE_T, PINA, PINC, ANNODE, A, B, C, D, E, F, G, H) \
    SCC TYPE_T A = { MAKE_ENCODER(0, PINA, 0, PINC), MAKE_LED_RING(ANNODE, 0) }; \
    SCC TYPE_T B = { MAKE_ENCODER(1, PINA, 1, PINC), MAKE_LED_RING(ANNODE, 1) }; \
    SCC TYPE_T C = { MAKE_ENCODER(2, PINA, 2, PINC), MAKE_LED_RING(ANNODE, 2) }; \
    SCC TYPE_T D = { MAKE_ENCODER(3, PINA, 3, PINC), MAKE_LED_RING(ANNODE, 3) }; \
    SCC TYPE_T E = { MAKE_ENCODER(4, PINA, 4, PINC), MAKE_LED_RING(ANNODE, 4) }; \
    SCC TYPE_T F = { MAKE_ENCODER(5, PINA, 5, PINC), MAKE_LED_RING(ANNODE, 5) }; \
    SCC TYPE_T G = { MAKE_ENCODER(6, PINA, 6, PINC), MAKE_LED_RING(ANNODE, 6) }; \
    SCC TYPE_T H = { MAKE_ENCODER(7, PINA, 7, PINC), MAKE_LED_RING(ANNODE, 7) }

#define MAKE_8(BASE, NAME, ...) BASE(__VA_ARGS__, \
    NAME ## _0, NAME ## _1, NAME ## _2, NAME ## _3, \
    NAME ## _4, NAME ## _5, NAME ## _6, NAME ## _7)

namespace hw
{
    namespace ios
    {
        SCC uint8_t CLIP_COLS = 8;
        SCC uint8_t CLIP_ROWS = 5;

        /**
         * CLIP PADS
         */

        MAKE_8(MAKE_BICHROME_ROW, CLIP_0, bicolor_pad, 26, mcp::MCP_2, mcp23017::port::PORTA+3, mcp::MCP_2, mcp23017::port::PORTB+3);
        MAKE_8(MAKE_BICHROME_ROW, CLIP_1, bicolor_pad, 27, mcp::MCP_2, mcp23017::port::PORTA+4, mcp::MCP_2, mcp23017::port::PORTB+4);
        MAKE_8(MAKE_BICHROME_ROW, CLIP_2, bicolor_pad, 28, mcp::MCP_2, mcp23017::port::PORTA+5, mcp::MCP_2, mcp23017::port::PORTB+5);
        MAKE_8(MAKE_BICHROME_ROW, CLIP_3, bicolor_pad, 29, mcp::MCP_2, mcp23017::port::PORTA+6, mcp::MCP_2, mcp23017::port::PORTB+6);
        MAKE_8(MAKE_BICHROME_ROW, CLIP_4, bicolor_pad, 30, mcp::MCP_2, mcp23017::port::PORTA+7, mcp::MCP_2, mcp23017::port::PORTB+7);

        SCC bicolor_pad* const CLIP_Y_X[CLIP_ROWS][CLIP_COLS] = {
            {&CLIP_0_0, &CLIP_0_1, &CLIP_0_2, &CLIP_0_3, &CLIP_0_4, &CLIP_0_5, &CLIP_0_6, &CLIP_0_7},
            {&CLIP_1_0, &CLIP_1_1, &CLIP_1_2, &CLIP_1_3, &CLIP_1_4, &CLIP_1_5, &CLIP_1_6, &CLIP_1_7},
            {&CLIP_2_0, &CLIP_2_1, &CLIP_2_2, &CLIP_2_3, &CLIP_2_4, &CLIP_2_5, &CLIP_2_6, &CLIP_2_7},
            {&CLIP_3_0, &CLIP_3_1, &CLIP_3_2, &CLIP_3_3, &CLIP_3_4, &CLIP_3_5, &CLIP_3_6, &CLIP_3_7},
            {&CLIP_4_0, &CLIP_4_1, &CLIP_4_2, &CLIP_4_3, &CLIP_4_4, &CLIP_4_5, &CLIP_4_6, &CLIP_4_7},
        };

        MAKE_8(MAKE_MONOCHROME_ROW, CLIP_STOP,    monochrome_pad, 31, mcp::MCP_2, mcp23017::port::PORTA+2);

        SCC monochrome_pad* const CLIP_STOP_X[CLIP_COLS] = {
            &CLIP_STOP_0, &CLIP_STOP_1, &CLIP_STOP_2, &CLIP_STOP_3,
            &CLIP_STOP_4, &CLIP_STOP_5, &CLIP_STOP_6, &CLIP_STOP_7
        };

        MAKE_8(MAKE_MONOCHROME_ROW, TRACK_SELECT, monochrome_pad, 32, mcp::MCP_2, mcp23017::port::PORTA+1);

        SCC monochrome_pad* const TRACK_SELECT_X[CLIP_COLS] = {
            &TRACK_SELECT_0, &TRACK_SELECT_1, &TRACK_SELECT_2, &TRACK_SELECT_3,
            &TRACK_SELECT_4, &TRACK_SELECT_5, &TRACK_SELECT_6, &TRACK_SELECT_7
        };

        /**
         * BANK CONTROL BUTTONS
         */

        MAKE_BLIND_ROW(blind_pad, 34,
            SHIFT, NUDGE_PLUS, NUDGE_MINUS, TAP_TEMPO,
            BANK_LEFT, BANK_RIGHT, BANK_UP, BANK_DOWN);

        SCC blind_pad* const BANK_CONTROL[CLIP_COLS] = {
            &SHIFT, &NUDGE_PLUS, &NUDGE_MINUS, &TAP_TEMPO,
            &BANK_LEFT, &BANK_RIGHT, &BANK_UP, &BANK_DOWN
        };

        /**
         * TRACK CONTROL BUTTONS AND ENCODERS
         */

        SCC uint8_t TRACK_CONTROL_COUNT = 4;

        SCC monochrome_pad PAN    = { MAKE_INPUT(35, 0), MAKE_OUTPUT(mcp::MCP_1, mcp23017::port::PORTA+0, 0) };
        SCC monochrome_pad SEND_A = { MAKE_INPUT(35, 1), MAKE_OUTPUT(mcp::MCP_1, mcp23017::port::PORTA+0, 1) };
        SCC monochrome_pad SEND_B = { MAKE_INPUT(35, 2), MAKE_OUTPUT(mcp::MCP_1, mcp23017::port::PORTA+0, 2) };
        SCC monochrome_pad SEND_C = { MAKE_INPUT(35, 3), MAKE_OUTPUT(mcp::MCP_1, mcp23017::port::PORTA+0, 3) };

        SCC monochrome_pad* const TRACK_CONTROL_X[TRACK_CONTROL_COUNT] = {
            &PAN, &SEND_A, &SEND_B, &SEND_C
        };

        MAKE_8(MAKE_RING_ENCODER, PAN, ring_encoder, 23, 22, mcp23017::port::PORTA);

        SCC ring_encoder* const TRACK_ENCODER_X[CLIP_COLS] = {
            &PAN_0, &PAN_1, &PAN_2, &PAN_3, &PAN_4, &PAN_5, &PAN_6, &PAN_7
        };

        /**
         * TRANSPORT CONTROL
         */

        // no button 4
        SCC blind_pad PLAY  = { addr::input::make_safe<35, 5>() };
        SCC blind_pad STOP  = { addr::input::make_safe<35, 6>() };
        SCC blind_pad REC   = { addr::input::make_safe<35, 7>() };

        /**
         * SCENE CONTROL
         */

        SCC monochrome_pad SCENE_LAUNCH_0 = { MAKE_INPUT(36, 0), MAKE_OUTPUT(mcp::MCP_2, mcp23017::port::PORTA+0, 0) };
        SCC monochrome_pad SCENE_LAUNCH_1 = { MAKE_INPUT(36, 1), MAKE_OUTPUT(mcp::MCP_2, mcp23017::port::PORTA+0, 1) };
        SCC monochrome_pad SCENE_LAUNCH_2 = { MAKE_INPUT(36, 2), MAKE_OUTPUT(mcp::MCP_2, mcp23017::port::PORTA+0, 2) };
        SCC monochrome_pad SCENE_LAUNCH_3 = { MAKE_INPUT(36, 3), MAKE_OUTPUT(mcp::MCP_2, mcp23017::port::PORTA+0, 3) };
        SCC monochrome_pad SCENE_LAUNCH_4 = { MAKE_INPUT(36, 4), MAKE_OUTPUT(mcp::MCP_2, mcp23017::port::PORTA+0, 4) };

        SCC monochrome_pad* const SCENE_LAUNCH_Y[CLIP_ROWS] = {
            &SCENE_LAUNCH_0, &SCENE_LAUNCH_1, &SCENE_LAUNCH_2, &SCENE_LAUNCH_3, &SCENE_LAUNCH_4
        };

        /**
         * MASTER CONTROLS
         */

        SCC blind_pad STOP_ALL_CLIPS      = { MAKE_INPUT(36, 5) };
        SCC monochrome_pad SELECT_MASTER  = { MAKE_INPUT(36, 6), MAKE_OUTPUT(mcp::MCP_2, mcp23017::port::PORTA+0, 6) };
        // no button 7

        SCC blind_encoder CUE_LEVEL = { MAKE_ENCODER(2, A16, 3, A16) };

        SCC fader MASTER_LEVEL  = { MAKE_FADER(0, A16) };
        SCC fader CROSSFADE     = { MAKE_FADER(1, A16) };

        /**
         * TRACK ACTION PADS
         */

        MAKE_8(MAKE_MONOCHROME_ROW, RECORD_ARM, monochrome_pad, 37, mcp::MCP_2, mcp23017::port::PORTB+0);
        MAKE_8(MAKE_MONOCHROME_ROW, SOLO_CUE,   monochrome_pad, 38, mcp::MCP_2, mcp23017::port::PORTB+1);
        MAKE_8(MAKE_MONOCHROME_ROW, ACTIVATOR,  monochrome_pad, 39, mcp::MCP_2, mcp23017::port::PORTB+2);

        SCC monochrome_pad* const RECORD_ARM_X[CLIP_COLS] = {
            &RECORD_ARM_0, &RECORD_ARM_1, &RECORD_ARM_2, &RECORD_ARM_3,
            &RECORD_ARM_4, &RECORD_ARM_5, &RECORD_ARM_6, &RECORD_ARM_7
        };
        SCC monochrome_pad* const SOLO_CUE_X[CLIP_COLS] = {
            &SOLO_CUE_0, &SOLO_CUE_1, &SOLO_CUE_2, &SOLO_CUE_3,
            &SOLO_CUE_4, &SOLO_CUE_5, &SOLO_CUE_6, &SOLO_CUE_7
        };
        SCC monochrome_pad* const ACTIVATOR_X[CLIP_COLS] = {
            &ACTIVATOR_0, &ACTIVATOR_1, &ACTIVATOR_2, &ACTIVATOR_3,
            &ACTIVATOR_4, &ACTIVATOR_5, &ACTIVATOR_6, &ACTIVATOR_7
        };

        /**
         * DEVICE CONTROL
         */

        MAKE_MONOCHROME_ROW(monochrome_pad, 33, mcp::MCP_1, mcp23017::port::PORTB+0,
            CLIP_TRACK, DEVICE_ON_OFF, LEFT_ARROW, RIGHT_ARROW,
            DETAIL_VIEW, REC_QUANTIZE, MIDI_OVERDUB, METRONOME);

        MAKE_8(MAKE_RING_ENCODER, CTRL,         ring_encoder, 21, 20, mcp23017::port::PORTB);

        SCC monochrome_pad* const DEVICE_CONTROL_X[CLIP_COLS] = {
            &CLIP_TRACK, &DEVICE_ON_OFF, &LEFT_ARROW, &RIGHT_ARROW,
            &DETAIL_VIEW, &REC_QUANTIZE, &MIDI_OVERDUB, &METRONOME
        };

        SCC ring_encoder* const DEVICE_ENCODER_X[CLIP_COLS] = {
            &CTRL_0, &CTRL_1, &CTRL_2, &CTRL_3, &CTRL_4, &CTRL_5, &CTRL_6, &CTRL_7
        };

        /**
         * TRACK FADERS
         */

        SCC fader TRACK_LEVEL_0 = { MAKE_FADER(0, A17) };
        SCC fader TRACK_LEVEL_1 = { MAKE_FADER(1, A17) };
        SCC fader TRACK_LEVEL_2 = { MAKE_FADER(2, A17) };
        SCC fader TRACK_LEVEL_3 = { MAKE_FADER(3, A17) };
        SCC fader TRACK_LEVEL_4 = { MAKE_FADER(4, A17) };
        SCC fader TRACK_LEVEL_5 = { MAKE_FADER(5, A17) };
        SCC fader TRACK_LEVEL_6 = { MAKE_FADER(6, A17) };
        SCC fader TRACK_LEVEL_7 = { MAKE_FADER(7, A17) };

        SCC fader* const TRACK_LEVEL_X[CLIP_COLS] = {
            &TRACK_LEVEL_0, &TRACK_LEVEL_1, &TRACK_LEVEL_2, &TRACK_LEVEL_3,
            &TRACK_LEVEL_4, &TRACK_LEVEL_5, &TRACK_LEVEL_6, &TRACK_LEVEL_7
        };

    } // endof namespace ios

} // endof namespace hw

#undef SCC
#undef MAKE_INPUT
#undef MAKE_OUTPUT
#undef MAKE_ENCODER
#undef MAKE_FADER
#undef MAKE_BLIND_ROW
#undef MAKE_MONOCHROME_ROW
#undef MAKE_BICHROME_ROW
#undef MAKE_LED_RING
#undef MAKE_RING_ENCODER
#undef MAKE_8

#endif // DEF_IO_DEFS_HPP

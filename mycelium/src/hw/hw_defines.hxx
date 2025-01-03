/**
 * 
 */

#ifndef DEF_HW_DEFINES_HXX
#define DEF_HW_DEFINES_HXX

#include <cstdint>

#define MAKE_8(NAME, PIN) \
    NAME ## _0 = 0b0 ## PIN ## 000, \
    NAME ## _1 = 0b0 ## PIN ## 001, \
    NAME ## _2 = 0b0 ## PIN ## 010, \
    NAME ## _3 = 0b0 ## PIN ## 011, \
    NAME ## _4 = 0b0 ## PIN ## 100, \
    NAME ## _5 = 0b0 ## PIN ## 101, \
    NAME ## _6 = 0b0 ## PIN ## 110, \
    NAME ## _7 = 0b0 ## PIN ## 111

namespace hw
{

static constexpr const uint8_t COLUMNS_COUNT = 8;
static constexpr const uint8_t COLUMN_BITMASK= 0b0'0000'111;
static constexpr const uint8_t PIN_BITMASK   = 0b0'1111'000;

    /**
     * 
     */
    template <typename T>
    struct row_type;

    /**
     * 
     */
    template <typename T>
    static constexpr typename row_type<T>::type get_row(T p)
        { return static_cast<typename row_type<T>::type>((static_cast<uint8_t>(p) & PIN_BITMASK) >> 3); }

    /**
     * 
     */
    template <typename T>
    static constexpr uint8_t get_column(T p)
        { return static_cast<uint8_t>(p) & COLUMN_BITMASK; }

    /**
     * 
     */
    template <typename T>
    static constexpr bool is_blind(T);

/**
 * 
 */
namespace pads
{

/**
 * 
 */
enum class Pad: uint8_t
    {
    __FIRST_PAD__ = 0,

    /** Bicolor pads */

        /**
         * LAUNCH PADS
         *  pins 26, 27, 28, 29, 30
         */
        MAKE_8(CLIP_0, '0000'),
        MAKE_8(CLIP_1, '0001'),
        MAKE_8(CLIP_2, '0010'),
        MAKE_8(CLIP_3, '0011'),
        MAKE_8(CLIP_4, '0100'),

    /** Monochrome pads */

        /**
         * MASTER CONTROLS
         *  pins 31, 32, 33
         */

        MAKE_8(CLIP_STOP,    '0101'),
        MAKE_8(TRACK_SELECT, '0110'),

        SCENE_LAUNCH_0 = 0b0'0111'000,
        SCENE_LAUNCH_1,
        SCENE_LAUNCH_2,
        SCENE_LAUNCH_3,
        SCENE_LAUNCH_4,
        STOP_ALL_CLIPS,     /* blind pad */
        SELECT_MASTER,
        __UNUSED_33_7__,    /* no column 7 */

        /**
         * TRACK ACTION PADS
         *  pins 34, 35, 36
         */
        MAKE_8(RECORD_ARM,  '1000'),
        MAKE_8(SOLO_CUE,    '1001'),
        MAKE_8(ACTIVATOR,   '1010'),

        /**
         * DEVICE CONTROL
         *  pin 37
         */
        CLIP_TRACK  = 0b0'1011'000,
        DEVICE_ON_OFF,
        LEFT_ARROW,
        RIGHT_ARROW,
        DETAIL_VIEW,
        REC_QUANTIZE,
        MIDI_OVERDUB,
        METRONOME,

        /**
         * TRANSPORT CONTROLS
         *  pin 38
         */
        PAN         = 0b0'1100'000,
        SEND_A,
        SEND_B,
        SEND_C,

    /** Blind pads */

        __UNUSED_39_4__,
        PLAY,
        STOP,
        REC,

        /**
         * BANK CONTROL
         *  pin 39
         */
        SHIFT       = 0b0'1101'000,
        NUDGE_PLUS,
        NUDGE_MINUS,
        TAP_TEMPO,
        BANK_LEFT,
        BANK_RIGHT,
        BANK_UP,
        BANK_DOWN,

    __PADS_COUNT__
    }; /* endof enum Pad */

constexpr bool operator< (Pad lhs, Pad rhs)
    { return static_cast<uint8_t>(lhs) < static_cast<uint8_t>(rhs); }

/**
 * 
 */
enum class PadRow: uint8_t
    {
    __FIRST_ROW__ = 0,

    /** Bicolor pads */

        Clip_0 = 0,
        Clip_1,
        Clip_2,
        Clip_3,
        Clip_4,

    /** Monochrome pads */

        ClipStop,
        TrackSelect,
        MasterControl,  /**< scene launch, stop all clips, select master */
        RecordArm,
        SoloCue,
        Activator,
        DeviceControl,  /**< clip track thru metronome */

    /** Blind pads */

        TransportControl,
        BankControl,

    __ROWS_COUNT__
    }; /* endof enum PadRow */

/**
 * Returns true if given pad has two led attached,
 *  only true for CLIP_Y_X pads
 */
static constexpr bool is_bichrome(Pad p)
    { return p < Pad::CLIP_STOP_0; }

/**
 * Returns true if given pad has a single led attached
 */
static constexpr bool is_monochrome(Pad p)
    { return !(is_blind(p) || is_bichrome(p)); }

/**
 * Utility defines for easy walkthrough existing pads
 */
    static constexpr const uint8_t CLIP_COLS = COLUMNS_COUNT;
    static constexpr const uint8_t CLIP_ROWS = 5;

    /** LAUNCH PADS */

    static constexpr const Pad CLIP_Y_X[CLIP_ROWS][CLIP_COLS] = {
        {Pad::CLIP_0_0, Pad::CLIP_0_1, Pad::CLIP_0_2, Pad::CLIP_0_3, Pad::CLIP_0_4, Pad::CLIP_0_5, Pad::CLIP_0_6, Pad::CLIP_0_7},
        {Pad::CLIP_1_0, Pad::CLIP_1_1, Pad::CLIP_1_2, Pad::CLIP_1_3, Pad::CLIP_1_4, Pad::CLIP_1_5, Pad::CLIP_1_6, Pad::CLIP_1_7},
        {Pad::CLIP_2_0, Pad::CLIP_2_1, Pad::CLIP_2_2, Pad::CLIP_2_3, Pad::CLIP_2_4, Pad::CLIP_2_5, Pad::CLIP_2_6, Pad::CLIP_2_7},
        {Pad::CLIP_3_0, Pad::CLIP_3_1, Pad::CLIP_3_2, Pad::CLIP_3_3, Pad::CLIP_3_4, Pad::CLIP_3_5, Pad::CLIP_3_6, Pad::CLIP_3_7},
        {Pad::CLIP_4_0, Pad::CLIP_4_1, Pad::CLIP_4_2, Pad::CLIP_4_3, Pad::CLIP_4_4, Pad::CLIP_4_5, Pad::CLIP_4_6, Pad::CLIP_4_7},
    };

    /** MASTER CONTROLS */

    static constexpr const Pad CLIP_STOP_X[COLUMNS_COUNT] = {
        Pad::CLIP_STOP_0, Pad::CLIP_STOP_1, Pad::CLIP_STOP_2, Pad::CLIP_STOP_3,
        Pad::CLIP_STOP_4, Pad::CLIP_STOP_5, Pad::CLIP_STOP_6, Pad::CLIP_STOP_7
    };

    static constexpr const Pad TRACK_SELECT_X[COLUMNS_COUNT] = {
        Pad::TRACK_SELECT_0, Pad::TRACK_SELECT_1, Pad::TRACK_SELECT_2, Pad::TRACK_SELECT_3,
        Pad::TRACK_SELECT_4, Pad::TRACK_SELECT_5, Pad::TRACK_SELECT_6, Pad::TRACK_SELECT_7
    };

    static constexpr const Pad SCENE_LAUNCH_Y[CLIP_ROWS] = {
        Pad::SCENE_LAUNCH_0, Pad::SCENE_LAUNCH_1, Pad::SCENE_LAUNCH_2, Pad::SCENE_LAUNCH_3, Pad::SCENE_LAUNCH_4
    };

    /** TRACK ACTIONS */

    static constexpr const Pad RECORD_ARM_X[COLUMNS_COUNT] = {
        Pad::RECORD_ARM_0, Pad::RECORD_ARM_1, Pad::RECORD_ARM_2, Pad::RECORD_ARM_3,
        Pad::RECORD_ARM_4, Pad::RECORD_ARM_5, Pad::RECORD_ARM_6, Pad::RECORD_ARM_7
    };

    static constexpr const Pad SOLO_CUE_X[COLUMNS_COUNT] = {
        Pad::SOLO_CUE_0, Pad::SOLO_CUE_1, Pad::SOLO_CUE_2, Pad::SOLO_CUE_3,
        Pad::SOLO_CUE_4, Pad::SOLO_CUE_5, Pad::SOLO_CUE_6, Pad::SOLO_CUE_7
    };

    static constexpr const Pad ACTIVATOR_X[COLUMNS_COUNT] = {
        Pad::ACTIVATOR_0, Pad::ACTIVATOR_1, Pad::ACTIVATOR_2, Pad::ACTIVATOR_3,
        Pad::ACTIVATOR_4, Pad::ACTIVATOR_5, Pad::ACTIVATOR_6, Pad::ACTIVATOR_7
    };

    /** DEVICE CONTROL */

    static constexpr const Pad DEVICE_CONTROL_X[COLUMNS_COUNT] = {
        Pad::CLIP_TRACK, Pad::DEVICE_ON_OFF, Pad::LEFT_ARROW, Pad::RIGHT_ARROW,
        Pad::DETAIL_VIEW, Pad::REC_QUANTIZE, Pad::MIDI_OVERDUB, Pad::METRONOME
    };

    /** TRACK CONTROL */

    static constexpr const uint8_t TRACK_CONTROL_COUNT = 4;

    static constexpr const Pad TRACK_CONTROL_X[TRACK_CONTROL_COUNT] = {
        Pad::PAN, Pad::SEND_A, Pad::SEND_B, Pad::SEND_C
    };

    /** BANK CONTROL */

    static constexpr const Pad BANK_CONTROL[COLUMNS_COUNT] = {
        Pad::SHIFT, Pad::NUDGE_PLUS, Pad::NUDGE_MINUS, Pad::TAP_TEMPO,
        Pad::BANK_LEFT, Pad::BANK_RIGHT, Pad::BANK_UP, Pad::BANK_DOWN
    };

} /* endof namespace pads */

namespace analog
{

/**
 * 
 */
enum class Encoder: uint8_t
    {
    __FIRST_ENCODER__   = 0,

    /** Encoders with led-ring */

        MAKE_8(PAN, '0000'),
        MAKE_8(CTRL, '0001'),

    /** blind encoder */

        CUE_LEVEL,

    __ENCODERS_COUNT__
    }; /* endof enum Encoder */

/**
 * 
 */
enum class EncoderGroup: uint8_t
    {
    __FIRST_GROUP__     = 0,

        Pan             = 0,
        DeviceControl,
        MasterControl,

    __GROUPS_COUNT__
    }; /* endof enum EncoderGroup */

/**
 * 
 */
enum class Fader: uint8_t
    {
    __FIRST_FADER__ = 0,

        MAKE_8(TRACK_LEVEL, '0000'),

        MASTER_LEVEL,
        CROSSFADE,

    __FADERS_COUNT__
    }; /* endof enum Fader */

/**
 * 
 */
enum class FaderGroup: uint8_t
    {
    __FIRST_GROUP__ = 0,

        TrackLevel  = 0,
        MasterControl,

    __GROUPS_COUNT__
    }; /* endof enum FaderGroup */

    static constexpr const Encoder TRACK_ENCODER_X[COLUMNS_COUNT] = {
        Encoder::PAN_0, Encoder::PAN_1, Encoder::PAN_2, Encoder::PAN_3,
        Encoder::PAN_4, Encoder::PAN_5, Encoder::PAN_6, Encoder::PAN_7
    };
    static constexpr const Encoder DEVICE_ENCODER_X[COLUMNS_COUNT] = {
        Encoder::CTRL_0, Encoder::CTRL_1, Encoder::CTRL_2, Encoder::CTRL_3,
        Encoder::CTRL_4, Encoder::CTRL_5, Encoder::CTRL_6, Encoder::CTRL_7
    };

    static constexpr const Fader TRACK_LEVEL_X[COLUMNS_COUNT] = {
        Fader::TRACK_LEVEL_0, Fader::TRACK_LEVEL_1, Fader::TRACK_LEVEL_2, Fader::TRACK_LEVEL_3,
        Fader::TRACK_LEVEL_4, Fader::TRACK_LEVEL_5, Fader::TRACK_LEVEL_6, Fader::TRACK_LEVEL_7
    };

} /* endof namespace analog */

} /* endof namespace hw */

#undef MAKE_8

#include "hw_defines.hpp"

#endif
/**
 * 
 */

#ifndef DEF_LEDS_TYPES_HXX
#define DEF_LEDS_TYPES_HXX

#include <cstdint>
#include <cstddef>

namespace hw
{
namespace leds_driver
{

/**
 * List of allowed colors for bicolor and monochrome pads,
 *  Monochrome pads uses ON or GREEN discarding base color
 */
enum class pad_color: uint8_t
    { OFF=0x00, ON=0x01, GREEN=0x01, RED=0x02, ORANGE=0x03 };

/**
 * Used as conversion operator to extract a single channel state from a color
 *  (color_A & GREEN) returns true if channel green is HIGH for color_A
 */
constexpr bool operator& (pad_color lhs, pad_color rhs)
    { return static_cast<bool>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)); }

/**
 * Encodes state of a ledring when only a contiguous range is lit
 *  powerring off all leds is done by setting count to 0,
 *  or by setting start to 15
 */
struct ledring_state
{
    uint8_t start :4;   /**< indexof first led [0-14] */
    uint8_t count :4;   /**< number of on leds [0-15], write 0 to set off */

    /**
     * Converts state to a single two bytes word, with each bit encoding a single led
     *  Note that low significant bit is always zero
     */
    constexpr uint16_t word() const
    {
        return
            (static_cast<uint32_t>(0x01) << (start+count+1))-1  /* 0...0_start+count << 1...1 */
            &
            ~((static_cast<uint32_t>(0x01) << (start+1))-1)     /* 1...1 << start_0...0 */
        ;
    }

    /**
     * Returns low significant byte of corresponding state word
     */
    constexpr uint8_t lsb() const
        { return word() & 0xFE; }

    /**
     * Returns most significant byte of corresponding state word
     */
    constexpr uint8_t msb() const
        { return word() >> 8; }

    /**
     * Utility returning state corresponding to a ring with only center led powered on
     */
    static constexpr ledring_state center()
        { return ledring_state{7, 1}; }

}; /* endof struct ledring_state */

static_assert(sizeof(ledring_state) == sizeof(uint8_t));

static_assert(ledring_state{0,0}.word()     == 0x0);    /* 0 */
static_assert(ledring_state{0,0xFF}.word()  == 0xFFFE); /* 0b1111'1111'1111'111_ */
static_assert(ledring_state{3,5}.word()     == 0x01F0); /* 0b0000'0001'1111'000_ */
static_assert(ledring_state{0xFF,5}.word()  == 0x0);    /* 0 */
static_assert(ledring_state::center().word()== 0x0100); /* 0b0000'0001'0000'000_ */

// /**
//  * 
//  */
// struct led_ring_bytes
// {
//     using value_type = uint16_t;

//     static constexpr const value_type LSBMASK = 0x00FE; /**< 7 bits */
//     static constexpr const value_type MSBMASK = 0xFF00; /**< 8 bits */
//     static constexpr const value_type BITMASK = LSBMASK | MSBMASK; /**< 15 bits */

//     value_type bits;

//     explicit led_ring_bytes(value_type bits)
//         : bits{(bits << 1) & BITMASK}
//         {}

//     explicit operator value_type() const
//         { return (bits & BITMASK) >> 1; }
    
//     uint8_t lsb() const
//         { return static_cast<uint8_t>(bits & LSBMASK); }
    
//     uint8_t msb() const
//         { return static_cast<uint8_t>((bits & MSBMASK) >> 8); }
// };

} /* endof namespace leds_driver */
} /* endof namespace hw */

#endif /* DEF_LEDS_TYPES_HXX */
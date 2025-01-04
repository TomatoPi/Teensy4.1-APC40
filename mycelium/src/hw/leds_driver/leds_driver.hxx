/**
 * 
 */

#ifndef DEF_LEDS_DRIVER_HXX
#define DEF_LEDS_DRIVER_HXX

#include "../../error.hxx"

#include <cstdint>
#include <cstddef>

namespace hw
{
namespace leds_driver
{

/**
 * Number of colum addressing pins, common for inputs and outputs
 */
static constexpr const uint8_t MULTIPLEX_COLUMS_COUNT = 8;
/**
 * Bitmask to force a value to be in correct range
 */
static constexpr const uint8_t MULTIPLEX_COLUMN_BITMASK = 0x07;

/**
 * Leds cathode addressing pins
 */
static constexpr const uint8_t CATHODE_ADDR_PINA0 = 9;
static constexpr const uint8_t CATHODE_ADDR_PINA1 = 10;
static constexpr const uint8_t CATHODE_ADDR_PINA2 = 11;
static constexpr const uint8_t CATHODE_ENABLE_PIN = 12;

/**
 * 
 */
enum class annode_driver: uint8_t
    { LedsRingLow=0, LedsRingHigh=1, PushButtons=1, PadsMatrix=2 };
static constexpr const size_t ANNODE_DRIVER_COUNT = 3;

/**
 * 
 */
struct DriverDefaultSettings
{
    /**
     * Target frequency to refresh whole device ouputs in Hz
     * 
     *  Will be multiplied by multiplexer buses count (columns count)
     *      to obtain the update rate for the driver, @c LedsDriver::update
     * 
     *  @note defaults to 50Hz, leading to an update rate of 400Hz
     * */
    static unsigned long RefreshRate;

    /**
     * Filters out logs below given severity
     * @note defaults to @c error::severity::DEBUG if macro NDEBUG is undefined
     *      else defaults to @c error::severity::INFO
     */
    static error::severity LogLevel;
};

/**
 * 
 */
template <typename _Settings=DriverDefaultSettings, typename _Context>
class LedsDriver
{
public:
    using Settings = _Settings;
    using Context = _Context;

    error::status_byte setup();
    error::status_byte update();

    /**
     * Changes state of a single object
     */
    template <typename Address, typename State>
    error::status_byte set_state(Address addr, State state);

    template <typename Address, typename State>
    error::status_byte get_state(Address addr, State* state);

private:
    static const uint8_t WRITE_BUFFER_SIZE = 3;

    enum class CycleState: uint8_t
    {
        READY,              ///< Driver is ready to begin a new cycle
        UPDATING_GPIOS,     ///< Writing GPIOS for all MCPs
        ENDING,             ///< Waiting for transmition ACK
    };

    error::errcode begin_cycle();
    error::errcode update_mcps();
    error::errcode end_cycle();

    uint8_t _mcp_write_buffer[MULTIPLEX_COLUMS_COUNT][ANNODE_DRIVER_COUNT][WRITE_BUFFER_SIZE];
    

}; /* endof class LedsDriver */

} /* endof namespace leds_driver */
} /* endof namespace hw */

#include "leds_driver.hpp"

#endif /* DEF_LEDS_DRIVER_HXX */
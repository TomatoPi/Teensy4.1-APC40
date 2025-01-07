/**
 * 
 */

#ifndef DEF_ERROR_HXX
#define DEF_ERROR_HXX

#if defined(ARDUINO_TEENSY41)
    #include <Arduino.h>
#endif

#include <cstdint>

namespace error
{

/**
 * Error handling is done with functions returning an error code and module specific descriptors
 */
enum class errcode: uint8_t
{
    OK = 0,                     ///< Success
    /** 
     * Internal errors, may occur at runtime
     */
    HWERROR         = 0x01,     ///< More infos may be available at module specific error pointer
    GENERIC_ERROR   = 0x02,     ///< Random error without any clear cause
    MEMORY_ERROR    = 0x03,     ///< Not enought memory available
    TIMEOUT_ERROR   = 0x04,     ///< Async call took too long, service not available or timeout too short
    
    /**
     * Programming errors, shame on you
     */
    INVALID_STATE       = 0x10, ///< algorithm error
    INVALID_CALL        = 0x11, ///< called a method that should not be called at this time
    INVALID_ARGUMENT    = 0x12, ///< called a method with out of domain argument, see error message
    INFINITE_LOOP       = 0x13, ///< runned an unbounded iteration over the predefined limit
};

/**
 * Converts an error code to a static str
 */
static constexpr const char* errname(errcode err);

/**
 * 
 */
enum class severity: uint8_t
{
    EMERGENCY   = 0,    ///< System is unusable
    ALERT       = 1,    ///< Action must be taken immediately
    CRITICAL    = 2,    ///< System may only be available in a degraded state

    ERROR       = 3,    ///< An unary task have failed but state is recoverable

    WARNING     = 4,    ///< Some non critical functions might be unavailable or degraded
                        ///,    but system should behave as expected

    NOTICE      = 5,    ///< Normal but significant condition
                        ///,    Not an error but may require special handling

    INFO        = 6,    ///< Confirmation that the program is working as expected
    DEBUG       = 7,    ///< Information normally of use only when debugging
};

/**
 * 
 */
static constexpr const char* severity_name(const severity s);

/**
 * 
 */
struct status_byte
{
    uint8_t code :5;
    uint8_t level :3;

    constexpr status_byte(const errcode code=errcode::OK, const severity level=severity::INFO)
        : code{static_cast<uint8_t>(code)}, level{static_cast<uint8_t>(level)}
        {}

    constexpr operator errcode() const
        { return static_cast<errcode>(code); }

    constexpr operator severity() const
        { return static_cast<severity>(level); }

    constexpr operator bool() const
        { return static_cast<errcode>(code) == errcode::OK; }
};
static_assert(sizeof(status_byte) == sizeof(uint8_t));

/**
 * Syntax sugar to return status_byte from an errcode and an code
 */
constexpr status_byte operator| (errcode code, severity sv)
    { return {code, sv}; }

/**
 * Pin with a led attached to signal presence of errors to the user
 */
static constexpr const uint8_t ERROR_PIN = 6;

/**
 * Global flag indicating if an error have been raised somewhere
 */
extern bool HasError;

#if defined(ARDUINO_TEENSY41)
    inline static void raise_error()
        { HasError = true; digitalWriteFast(ERROR_PIN, HIGH); }

    inline static void clear_error()
        { HasError = false; digitalWriteFast(ERROR_PIN, LOW); }
#else
    inline static void raise_error()
        { HasError = true; }
    inline static void clear_error()
        { HasError = false; }
#endif

} /* namespace error */

#include "_error.hpp"

#endif /* DEF_ERROR_HXX */

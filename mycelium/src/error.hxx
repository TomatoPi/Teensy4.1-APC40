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
     * Internal errors, that may occur at runtime
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
};

/**
 * Converts an error code to a static str
 */
static constexpr const char* errname(errcode err)
{
    switch(err)
    {
        case errcode::OK:                 return "OK";

        case errcode::HWERROR:            return "HWERROR";
        case errcode::TIMEOUT_ERROR:      return "TIMEOUT_ERROR";
        case errcode::MEMORY_ERROR:       return "MEMORY_ERROR";
        case errcode::GENERIC_ERROR:      return "GENERIC_ERROR";

        case errcode::INVALID_ARGUMENT:   return "INVALID_ARGUMENT";
        case errcode::INVALID_CALL:       return "INVALID_CALL";
        case errcode::INVALID_STATE:      return "INVALID_STATE";

        default:                          return "UNKNOWN";
    }
}

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
static constexpr const char* severity_name(const severity s)
{
    switch (s)
    {
        case severity::EMERGENCY:   return "EMERG";
        case severity::ALERT:       return "ALERT";
        case severity::CRITICAL:    return "CRIT!";

        case severity::ERROR:       return "ERROR";
        case severity::WARNING:     return "WARN";
        case severity::NOTICE:      return "NOTE";

        case severity::INFO:        return "INFO";
        case severity::DEBUG:       return "DEBUG";

        default:                    return "UNKNOWN";
    }
}

/**
 * 
 */
struct status_byte
{
    uint8_t errno :5;
    uint8_t level :3;

    constexpr status_byte(const errcode errno=errcode::OK, const severity level=severity::INFO)
        : errno{static_cast<uint8_t>(errno)}, level{static_cast<uint8_t>(level)}
        {}

    constexpr operator errcode() const
        { return static_cast<errcode>(errno); }

    constexpr operator severity() const
        { return static_cast<severity>(level); }
};
static_assert(sizeof(status_byte) == sizeof(uint8_t));

/**
 * Syntax sugar to return status_byte from an errcode and an errno
 */
constexpr status_byte operator| (errcode errno, severity sv)
    { return {errno, sv}; }

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

#endif /* DEF_ERROR_HXX */

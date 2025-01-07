/**
 * 
 */

#include "error.hpp"

namespace error
{

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

} /* endof namespace error */

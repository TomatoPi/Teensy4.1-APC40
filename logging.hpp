/**
 *
 * 
 */

#ifndef DEF_LOGGING_HPP
#define DEF_LOGGING_HPP

#include <Arduino.h>

#include <cstdint>
#include <cstddef>

#ifndef NDEBUG
    #define DEFAULT_SEVERITY   severity::DEBUG
#else
    #define DEFAULT_SEVERITY   severity::INFO
#endif

#define LOG_FOLDER          "logs"
#define LOG_FILE            "logs/log-v12024-12-22.log"

namespace logging
{

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

    static constexpr const char* severity_name(const severity s)
    {
        switch (s)
        {
            case severity::EMERGENCY:   return "EMERG";
            case severity::ALERT:       return "ALERT";
            case severity::CRITICAL:    return "CRIT";

            case severity::ERROR:       return "ERROR";
            case severity::WARNING:     return "WARNING";
            case severity::NOTICE:      return "NOTICE";

            case severity::INFO:        return "INFO";
            case severity::DEBUG:       return "DEBUG";

            default:                    return "UNKNOWN";
        }
    }

    /**
     * Global logging message buffer
     */
    static constexpr const size_t LOGGING_BUFFER_SIZE = 2048;
    static DMAMEM char _msg_buffer[LOGGING_BUFFER_SIZE] = {0};
    static size_t _msg_len = 0;
    static severity _log_level = DEFAULT_SEVERITY;

    /**
     * Returns true if given severity level is enabled for logging
     *  useful to avoid costly function calls if logging is disabled
     */
    bool is_logging_enabled_for(const severity s)
        { return static_cast<uint8_t>(s) <= static_cast<uint8_t>(_log_level); }

    /**
     * Initialise logging module and creates log file
     *  returns true if successful
     */
    bool init_logger();

    /**
     * Flush content of _msg_buffer into the logfile on SD card
     *  returns true if successful
     */
    bool flush_to_logfile();

    /**
     * Empty the log buffer
     */
    inline void clear_buffer()
    {
        _msg_buffer[0] = '\0';
        _msg_len = 0;
    }

    /**
     * Root function to add text in logging buffer, works like printf functions family
     */
    template <typename ...Args>
    bool write_raw(const char* fmt, Args... args)
    {
        elapsedMicros elapsed;
        Serial.printf(fmt, args...);
            /**
             * Avoid bad call if buffer is full
             */
        if (LOGGING_BUFFER_SIZE <= _msg_len)
        {
            if (Serial)
                { Serial.println("@write_raw: ERROR: buffer is full"); }
            return false;
        }

            /**
             * Computes remaining size and buffer pointer
             */
        const size_t available_buffsize = LOGGING_BUFFER_SIZE - _msg_len;
        char* msg_ptr = _msg_buffer + _msg_len;

            /**
             * Writes bytes
             */
        int bytes_written = snprintf(msg_ptr, available_buffsize, fmt, args...);
        if (bytes_written < 0)
        {
            if (Serial)
                { Serial.printf("@write_raw: ERROR: snprintf returned errno: %d", bytes_written); }
            return false;
        }

            /**
             * Check if buffer is full
             */
        if (available_buffsize < static_cast<size_t>(bytes_written))
        {
            if (Serial)
                { Serial.printf("@write_raw: WARNING: output truncated... %d bytes missing",
                    static_cast<size_t>(bytes_written) - available_buffsize); }

            // assume output has failed (avoid subsequent calls)
            _msg_len = LOGGING_BUFFER_SIZE;
            return false;
        }

            /**
             * Update buffer pointer
             */
        _msg_len += static_cast<size_t>(bytes_written);

        if (static_cast<bool>(Serial) && is_logging_enabled_for(severity::DEBUG))
            { Serial.printf("@write_raw: SUCCESS: bytes_written=%d, time_taken=%lu(us)", bytes_written, static_cast<unsigned long>(elapsed)); }
        return true;
    }

    /**
     * Explicit logging function for any level
     */
    template <typename ...Args>
    bool log_raw(const char* facility, const severity s, const char* fmt, Args... args)
    {
        if (!is_logging_enabled_for(s))
            { return true; /* nothing to do */ }
        if (!write_raw("%u:%-5s: @%5s: ", millis(), severity_name(s), facility))
            { return false; /* buffer is full */ }
        return write_raw(fmt, args...);
    }

    template <typename ...Args>
    bool emergency(const char* facility, const char* fmt, Args... args)
        { return log_raw(facility, severity::EMERGENCY, fmt, args...); }

    template <typename ...Args>
    bool alert(const char* facility, const char* fmt, Args... args)
        { return log_raw(facility, severity::ALERT, fmt, args...); }

    template <typename ...Args>
    bool critical(const char* facility, const char* fmt, Args... args)
        { return log_raw(facility, severity::CRITICAL, fmt, args...); }

    template <typename ...Args>
    bool error(const char* facility, const char* fmt, Args... args)
        { return log_raw(facility, severity::ERROR, fmt, args...); }

    template <typename ...Args>
    bool warning(const char* facility, const char* fmt, Args... args)
        { return log_raw(facility, severity::WARNING, fmt, args...); }

    template <typename ...Args>
    bool notice(const char* facility, const char* fmt, Args... args)
        { return log_raw(facility, severity::NOTICE, fmt, args...); }

    template <typename ...Args>
    bool info(const char* facility, const char* fmt, Args... args)
        { return log_raw(facility, severity::INFO, fmt, args...); }

    template <typename ...Args>
    bool debug(const char* facility, const char* fmt, Args... args)
        { return log_raw(facility, severity::DEBUG, fmt, args...); }

} // endof namespace logging

#endif // DEF_LOGGING_HPP

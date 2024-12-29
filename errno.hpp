/**
 * 
 * 
 */

#ifndef DEF_ERRNO_HPP
#define DEF_ERRNO_HPP

#include <Arduino.h>

#include <cstdint>
#include <cstddef>
#include <cstdlib>

#include "src/hw/hardware.hpp"
#include "logging.hpp"

namespace err
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
        TIMEOUT_ERROR   = 0x02,     ///< Async call took too long, service not available or timeout too short
        MEMORY_ERROR    = 0x03,     ///< Not enought memory available
        GENERIC_ERROR   = 0x04,     ///< Random error without any clear cause
        /**
         * Programming errors, shame on you
         */
        INVALID_ARGUMENT    = 0x80, ///< self explanatory, see error message
        INVALID_CALL        = 0x81, ///< called a method that should not be called at this time
        INVALID_STATE       = 0x82, ///< algorithm error
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

    // /**
    //  * Global error message buffer, if it's too short try logging to SD card
    //  */
    // static constexpr const size_t MAX_ERR_MSG_SIZE = 512;
    // static DMAMEM char _error_msg_buffer[MAX_ERR_MSG_SIZE] = {0};
    // static DMAMEM size_t _error_msg_len = 0;

    // /**
    //  * Write given message to the error buffer and set error led up
    //  */
    // template <typename ...Args>
    // inline errcode raise_raw_error(const error errno, const char* fmt, Args ...args)
    // {
    //     snprintf(error_msg_buffer, MAX_ERR_MSG_SIZE, fmt, args...);
    //     digitalWrite(err_pin, HIGH);
    //     return errno;
    // }

    template <typename ...Args>
    inline errcode raise_error(const char* facility, const errcode errno, const logging::severity severity, const char* fmt, Args... args)
    {
        if(logging::log_raw(facility, severity, "%s: ", errname(errno)))
            { logging::write_raw(fmt, args...); }
        /* don't react to logging output as it already print out bad writes */
        digitalWrite(hw::ERROR_PIN, HIGH);
        return errno;
    }

    /**
     * Clear error buffer and set led down
     */
    inline void clear_errors()
    {
        // error_msg_buffer[0] = '\0';
        digitalWrite(hw::ERROR_PIN, LOW);
    }

    template <typename ErrT, size_t MaxErrCount>
    struct err_stack
    {
        /**
         * Type helpers
         */
        using type_t = err_stack<ErrT, MaxErrCount>;
        static constexpr size_t MAX_ERRORS_COUNT = MaxErrCount;

        /**
         * Static storage for error descriptors
         */
        static type_t _errors_array[MAX_ERRORS_COUNT];
        static type_t* _error_ptr;
        static size_t _errors_count;

        /**
         * Descriptor datas
         */
        ErrT error;                 ///< Error type specific datas
        const type_t* prev_error;   ///< Pointer to previous error on the stack

        /**
         * Push a new error on the stack, may fail if no more cells are available
         *      If Args are given, they are passed to raw raise_error function,
         *      if not this method is supposed to represent a soft error
         */
        template <typename ...Args>
        static errcode raise(const char* facility, ErrT e, Args ...args)
        {
            if (MAX_ERRORS_COUNT <= _errors_count)
                { return raise_error(facility, errcode::INVALID_STATE, logging::severity::CRITICAL, "Not enought cells for errors"); }
            
            _errors_array[_errors_count] = type_t{.error = e, .prev_error = _error_ptr};
            _error_ptr = _errors_array + _errors_count;
            _errors_count += 1;

            if (0 < sizeof...(Args))
                { return raise_error(facility, args...); }
            else
                { return errcode::OK; }
        }

        /**
         * Resets the stack
         */
        static void clear_errors()
        { 
            _error_ptr = nullptr;
            _errors_count = 0;
        }

        static bool has_errors()
            { return _error_ptr != nullptr; }
    };

    // template <typename T>
    // struct result
    // {
    //     error errno;
    //     T result;

    //     explicit result(const T& obj): errno{error::OK}, result{obj}
    //         {}

    //     explicit result(error err): errno{err}, result{}
    //         {}

    //     constexpr operator bool() const
    //         { return errno == error::OK; }
    // };

} // endof namespace err

#endif // DEF_ERRNO_HPP
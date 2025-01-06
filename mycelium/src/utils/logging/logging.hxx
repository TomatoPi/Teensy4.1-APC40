/**
 * Results in 1090 lines of asm when compiled with -O2
 * 
 */

#ifndef DEF_LOGGING_HXX
#define DEF_LOGGING_HXX

#include "../../error.h"
#include "../mycelium/mycelium.h"

#include <cstddef>
#include <utility>
#include <tuple>

#include <cstdarg>

namespace logging
{

// template <typename Impl>
// class Printer
// {
// public:

//     /**
//      * Output method using printf style,
//      * @param fmt: printf like formatted string
//      * @param args: arguments corresponding to @c fmt placeholders
//      */
//     template <typename ...Args>
//     error::status_byte operator() (const char* fmt, Args... args)
//     { return static_cast<Impl*>(this)->print_impl(fmt, std::forward<Args>(args)...); }

//     // /**
//     //  * Output method using printf style,
//     //  * @param fmt: printf like formatted string
//     //  * @param args: arguments corresponding to @c fmt placeholders
//     //  */
//     // error::status_byte print(const char* fmt, ...)
//     // {
//     //     va_list args;
//     //     va_start(args, fmt);
//     //     auto err = static_cast<Impl*>(this)->print_impl(fmt, args);
//     //     va_end(args);
//     //     return err;
//     // }

//     /**
//      * Ensures that any previous print calls are physically transmitted
//      *  to the corresponding media:
//      *  - all serial bytes are sent,
//      *  - write on SD card complete,
//      *  - ...
//      */
//     error::status_byte flush()
//         { return static_cast<Impl*>(this)->flush(); }

// protected:
//     Printer()               = default;
//     Printer(const Printer&) = default;
//     Printer(Printer&&)      = default;
// };

template <typename C> struct AbstractOutput;
template <typename C> struct AbstractHeader;
template <typename C> struct AbstractFilter;

struct DefaultContext;

template <typename Context=DefaultContext>
struct AbstractOutput
{
    using raw_context = Context;
    using context = typename mycelium::contextof<raw_context>::type;

    /**
     * Output method using printf style,
     * @param fmt: printf like formatted string
     * @param args: arguments corresponding to @c fmt placeholders
     * @note uses compiler attribute @c __attribute__(format) to check passed arguments
     *      
     */
    error::status_byte operator() (const char* fmt, ...);
    
    /**
     * Ensures that any previous print calls are physically transmitted
     *  to the corresponding media:
     *  - all serial bytes are sent,
     *  - write on SD card complete,
     *  - ...
     */
    virtual error::status_byte flush()      = 0;

    /**
     * checks stream's availability, should returns false if writing
     *  to the stream will fail for sure
     */
    virtual explicit operator bool() const  = 0;

protected:
    AbstractOutput()                        = default;
    AbstractOutput(const AbstractOutput&)   = default;
    AbstractOutput(AbstractOutput&&)        = default;

    /**
     * Implementation of output method, called by @c operator(fmt,...)
     * @note arguments listed in 'args' are checked by the compiler
     *      and can be presumed correct regarding of 'fmt'
     */
    virtual error::status_byte print_impl(const char* fmt, va_list args) = 0;
};

template <typename Context=DefaultContext>
struct AbstractHeader
{
    using raw_context = Context;
    using context = typename mycelium::contextof<raw_context>::type;
    using output_type = typename context::output_type;
    using filter_type = typename context::filter_type;

    /**
     * Dumps the header in given output stream,
     *  @returns stream's status byte
     */
    virtual error::status_byte operator() (output_type&)    = 0;

    /**
     * Returns true if output is enabled given this header and given filter
     *  Simulates double dispatch using visitor pattern,
     *  allowing a header to bypass filtering rules
     */
    virtual bool operator() (const filter_type& filter)     = 0;
};

template <typename Context=DefaultContext>
struct AbstractFilter
{
    using raw_context = Context;
    using context = typename mycelium::contextof<raw_context>::type;
    using header_type = typename context::header_type;

    /**
     * Returns true if output is enabled for given header,
     *  should be overriden and overloaded to dispatch on header's true type
     */
    virtual bool operator() (const header_type&) = 0;
};

template <typename Printer, typename Filter>
struct Logger
{
    using printer_type = Printer;
    using filter_type = Filter;
    using type = Logger<printer_type, filter_type>;

    using context = typename mycelium::contextof<type>::type;

    template <typename Header, typename ...Args>
        error::status_byte
    operator() (Header msg_header, const char* fmt, Args... args)
        {
            if (!is_logging_enabled_for(msg_header))
                { return error::status_byte{}; }
            if (auto err = msg_header(printer); context::is_error(err))
                { return context::raise_error(err, "failed print header"); }
            else
                { return printer(fmt, std::forward<Args>(args)...); }
        }

    /**
     * Returns true if given severity level is enabled for logging
     *  useful to avoid costly function calls if logging is disabled
     */
    template <typename Header>
    constexpr bool is_logging_enabled_for(const Header& header) const
        { return filter(header); }

    printer_type    printer;
    filter_type     filter;
};

template <typename ...Ltps>
struct LoggersTie
{
    using logs_tp = std::tuple<Ltps...>;

    template <typename Header, typename ...Args>
        error::status_byte
    operator() (Header msg_header, const char* fmt, Args... args)
        {
            auto helper = [&](auto logger) { return logger(msg_header, fmt, args...); };
            std::apply([helper](auto ...l){std::make_tuple(helper(l)...); }, loggers);
            return error::status_byte{};
        }

    template <typename Header>
    constexpr bool is_logging_enabled_for(const Header& header) const
        {
            bool result = false;
            auto helper = [&result, &header](auto logger) {
                return result |= logger.is_logging_enabled_for(header);
                };
            std::apply([helper](auto ...l){std::make_tuple(helper(l)...); }, loggers);
            return result;
        }

    logs_tp loggers;
};

template <typename ...Logs>
LoggersTie<Logs...> tie_logs(Logs... loggers)
    { return LoggersTie<Logs...>{ std::tie(std::forward<Logs&>(loggers)...) }; }

struct severity_filter
{
    template <typename Header>
    constexpr bool operator() (const Header& s) const
        { return static_cast<error::severity>(s) <= log_level; }
    
    error::severity log_level;
};

template <typename Filter>
struct filter_not: Filter
{
    template <typename Header>
    constexpr bool operator() (const Header& h) const
        { return ! static_cast<const Filter&>(*this)(h); }
};

struct raw_header
{
    template <typename PrintFn>
    error::status_byte operator() (PrintFn print) const
        { return print("%lu:%-5s: @%5s: %s: ", timestamp, error::severity_name(errbyte), facility, error::errname(errbyte)); }

    constexpr explicit operator error::severity() const
        { return errbyte; }

    const char* facility;
    error::status_byte errbyte;
    unsigned long int timestamp;
};

} /* endof namespace logging */

#include "logging.hpp"

#endif /* DEF_LOGGING_HXX */

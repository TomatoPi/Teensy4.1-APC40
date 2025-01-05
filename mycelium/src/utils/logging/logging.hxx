/**
 * 
 */

#ifndef DEF_LOGGING_HXX
#define DEF_LOGGING_HXX

#include "../../error.h"
#include "../mycelium/mycelium.h"

#include <utility>
#include <tuple>

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
    bool is_logging_enabled_for(const Header& header) const
        { return filter(header); }

    printer_type    printer;
    filter_type     filter;
};

template <typename ...Logs>
struct LoggersTie
{
    using loggers_tp = std::tuple<std::add_lvalue_reference_t<Logs>...>;

    template <typename Header, typename ...Args>
        error::status_byte
    operator() (Header msg_header, const char* fmt, Args... args)
        {
            _call_impl<Header, Args...>(
                msg_header, fmt, std::forward<Args>(args)...,
                std::index_sequence_for<Logs...>{}
                );
            return error::status_byte{};
        }

    template <typename Header, typename ...Args, size_t ...Indexes>
        error::status_byte
    _call_impl(Header msg_header, const char* fmt, Args... args, std::index_sequence<Indexes...>);

    template <typename Header>
    bool is_logging_enabled_for(const Header& header) const
        { return _is_logging_enabled_for_impl<Header, std::index_sequence_for<Logs...>>(header); }

    template <typename Header, size_t Idx, size_t ...Indexes>
    bool _is_logging_enabled_for_impl(const Header& header) const
        {
            if (std::get<Idx>(loggers).is_logging_enabled_for(header))
                { return true; }
            else if (0 < sizeof...(Indexes))
                { return false; }
            
            return _is_logging_enabled_for_impl<Header, Indexes...>(header);
        }


    loggers_tp loggers;
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

struct raw_header
{
    template <typename PrintFn>
    error::status_byte operator() (PrintFn print) const
        { return print("%lu:%-5s: @%5s: %s: ", timestamp, error::severity_name(errbyte), facility, error::errname(errbyte)); }

    explicit operator error::severity() const
        { return errbyte; }

    const char* facility;
    error::status_byte errbyte;
    unsigned long int timestamp;
};

} /* endof namespace logging */

#include "logging.hpp"

#endif /* DEF_LOGGING_HXX */
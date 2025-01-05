/**
 * 
 */

#include "logging.hxx"

namespace logging
{

template <typename ...Logs>
    template <typename Header, typename ...Args, size_t Idx>
    error::status_byte
LoggersTie<Logs...>::_call_impl<Header, Args..., Idx>(Header msg_header, const char* fmt, Args... args, std::index_sequence<Idx>)
    {
        std::get<Idx>(loggers)(msg_header, fmt, args...);
    }


template <typename ...Logs>
    template <typename Header, typename ...Args, size_t Idx, size_t ...Indexes>
    error::status_byte
LoggersTie<Logs...>::_call_impl<Header, Args..., Idx, Indexes...>(Header msg_header, const char* fmt, Args... args, std::index_sequence<Idx, Indexes...>)
    {
        std::get<Idx>(loggers)(msg_header, fmt, args...);
        _call_impl<Header, Args..., Indexes...>(msg_header, fmt, args...,
            std::index_sequence<Indexes...>{});
    }
    
} /* endof namespace logging */

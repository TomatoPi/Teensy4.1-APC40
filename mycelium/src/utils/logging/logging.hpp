/**
 * 
 */

#include "logging.hxx"

namespace logging
{

// template <typename Tie, typename Fn, size_t Idx, size_t ...Indexes>
//     void
// _call_impl_helper(Tie tp, Fn call, std::index_sequence<Idx, Indexes...>)
//     {
//         call(std::get<Idx>(tp));
//     }

// template <typename Tie, typename Fn, size_t Idx>
//     void
// _call_impl_helper<Tie, Fn, Idx>(Tie tp, Fn call, std::index_sequence<Idx>)
//     {
//         call(std::get<Idx>(tp));
//     }

// template <typename ...Logs>
//     template <typename Header, typename ...Args, size_t ...Indexes>
//     error::status_byte
// LoggersTie<Logs...>::_call_impl(Header msg_header, const char* fmt, Args... args, std::index_sequence<Indexes...> seq)
//     {
//         _call_impl_helper(loggers, [&](auto logger){ logger(msg_header, fmt, args); }, seq);
//     }

// template <typename ...Logs>
//     template <typename Header, typename ...Args>
//     error::status_byte
// LoggersTie<Logs...>::_call_impl<Header, Args..., sizeof...(Logs)-1>(Header msg_header, const char* fmt, Args... args, std::index_sequence<Idx>)
//     {
//         std::get<Idx>(loggers)(msg_header, fmt, args...);
//     }


// template <typename ...Logs>
//     template <typename Header, typename ...Args, size_t Idx, size_t ...Indexes>
//     error::status_byte
// LoggersTie<Logs...>::_call_impl<Header, Args..., Idx, Indexes...>(Header msg_header, const char* fmt, Args... args, std::index_sequence<Idx, Indexes...>)
//     {
//         std::get<Idx>(loggers)(msg_header, fmt, args...);
//         _call_impl<Header, Args..., Indexes...>(msg_header, fmt, args...,
//             std::index_sequence<Indexes...>{});
//     }
    
} /* endof namespace logging */

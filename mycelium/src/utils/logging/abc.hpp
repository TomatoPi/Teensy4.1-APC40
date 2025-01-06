/**
 * 
 */

#include "abc.hxx"

#include <cstdarg>

namespace logging2
{

template <> struct typeof_status<DefaultContext> { using type = int; };
template <> struct typeof_transport<DefaultContext> { using type = AbstractTransport<DefaultContext>; };
template <> struct typeof_message<DefaultContext> { using type = AbstractMessage<DefaultContext>; };
template <> struct typeof_filter<DefaultContext> { using type = AbstractFilter<DefaultContext>; };

template <typename Ctx>
    typename AbstractTransport<Ctx>::status_type
AbstractTransport<Ctx>::operator() (const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        auto err = this->transmit_impl(fmt, args);
        va_end(args);
        return err;
    }

} /* endof namespace logging */
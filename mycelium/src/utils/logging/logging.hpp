/**
 * 
 */

#include "logging.hxx"

template <>
struct mycelium::contextof<logging::DefaultContext>
{
    struct type
    {
        using context = logging::DefaultContext;
        using output_type = logging::AbstractOutput<context>;
        using header_type = logging::AbstractHeader<context>;
        using filter_type = logging::AbstractFilter<context>;
    };
};

namespace logging
{

template <typename Context>
error::status_byte AbstractOutput<Context>::operator() (const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        auto err = this->print_impl(fmt, args);
        va_end(args);
        return err;
    }
    
} /* endof namespace logging */

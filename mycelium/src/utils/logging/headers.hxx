/**
 * 
 */

#ifndef DEF_LOGGING_HEADERS_HXX
#define DEF_LOGGING_HEADERS_HXX

#include "logging.hxx"

namespace logging
{
namespace headers
{

template <typename Context=DefaultContext>
struct timestamp: AbstractHeader<Context>
{
    using context_type = Context;
    using output_type = typename context_type::output_type;
    using filter_type = typename context_type::filter_type;

    using time_point = typename context_type::time::time_point;

    error::status_byte operator() (output_type& out) override
        { return out(""); }
    
    bool operator() (filter_type& filter) const override
        { return filter(*this); }
    
    time_point time;
};

// template <typename Context=DefaultContext>
// struct raw_header: AbstractHeader<Context>
// {
//     error::status_byte operator() (output_type& out) override
//         { return print("%lu:%-5s: @%5s: %s: ", timestamp, error::severity_name(errbyte), facility, error::errname(errbyte)); }

//     bool operator() (filter_type& filter) const
//         { return filter(*this); }

//     raw_header raise(error::status_byte newbyte) const
//         {
//             raw_header cpy{facility, newbyte, context::now()};
//         }

//     const char* facility;
//     error::status_byte errbyte;
//     typename context::time_type timestamp;
// };

} /* endof namespace headers */
} /* endof namespace logging */

#endif /* DEF_LOGGING_HEADERS_HXX */

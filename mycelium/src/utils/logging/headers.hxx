/**
 * 
 */

#ifndef DEF_LOGGING_HEADERS_HXX
#define DEF_LOGGING_HEADERS_HXX

#include "abc.hxx"

namespace logging2
{
namespace messages
{

template <typename Ctx=DefaultContext>
struct timestamp: public AbstractMessage<Ctx>
{
    using context = Ctx;
    using base_t = AbstractMessage<Ctx>;
    using time = typeof_timepoint_t<context>;

    typename base_t::status_type
    dump(typename base_t::transport_type& transport) const override
        { return transport("%ld-", time::hours(tps)); }
    
    bool is_accepted_by(const typename base_t::filter_type& filter) const override
        { return filter.is_transmission_allowed_for(*this); }
    
    typename time::time_point tps;
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

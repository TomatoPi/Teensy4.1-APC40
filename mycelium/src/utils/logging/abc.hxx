/**
 * 
 */

#ifndef DEF_LOGGING_ABC_HXX
#define DEF_LOGGING_ABC_HXX

namespace logging2
{


/**
 * Encodes a return type
 */
template <typename T>
struct typeof_status;

template <typename T>
using typeof_status_t = typename typeof_status<T>::type;

/**
 * Encodes an error handler
 */
template <typename T>
struct typeof_handler;

template <typename T>
using typeof_handler_t = typename typeof_handler<T>::type;

/**
 * Encodes a timestamp
 */
template <typename T>
struct typeof_timepoint;

template <typename T>
using typeof_timepoint_t = typename typeof_timepoint<T>::type;

/**
 * Encodes a message transport module (serial, stdio, ...)
 */
template <typename T>
struct typeof_transport;

template <typename T>
using typeof_transport_t = typename typeof_transport<T>::type;

/**
 * Encodes a message type
 */
template <typename T>
struct typeof_message;

template <typename T>
using typeof_message_t = typename typeof_message<T>::type;

template <typename T>
struct typeof_filter;

template <typename T>
using typeof_filter_t = typename typeof_filter<T>::type;

struct DefaultContext {};

template <typename Ctx=DefaultContext>
class AbstractTransport
{
public:
    using context = Ctx;
    using status_type = typeof_status_t<context>;

    /**
     * Output method using printf style,
     * @param fmt: printf like formatted string
     * @param args: arguments corresponding to @c fmt placeholders
     * @note uses compiler attribute @c __attribute__(format) to check passed arguments
     */
    status_type operator() (const char* fmt, ...)
        __attribute__(( format(printf, 2, 3) ));
    
    /**
     * Ensures that any previous print calls are physically transmitted
     *  to the corresponding media:
     *  - all serial bytes are sent,
     *  - write on SD card complete,
     *  - ...
     */
    virtual status_type flush()                 = 0;

    /**
     * checks stream's availability, should returns false if writing
     *  to the stream will fail for sure
     */
    virtual explicit operator bool() const      = 0;

protected:
    AbstractTransport()                         = default;
    AbstractTransport(const AbstractTransport&) = default;
    AbstractTransport(AbstractTransport&&)      = default;

    /**
     * Implementation of output method, called by @c transmit(fmt,...)
     * @note arguments listed in 'args' are checked by the compiler
     *      and can be presumed correct regarding of 'fmt'
     */
    virtual status_type transmit_impl(const char* fmt, va_list args) = 0;
};

template <typename Ctx=DefaultContext>
class AbstractMessage
{
public:
    using context = Ctx;
    using status_type = typeof_status_t<context>;
    using transport_type = typeof_transport_t<context>;
    using filter_type = typeof_filter_t<context>;

    /**
     * Dumps the message in given output stream,
     *  @returns stream's status byte
     */
    virtual status_type dump(transport_type&) const = 0;

    /**
     * Returns true if output is enabled given this header and given filter
     *  Simulates double dispatch using visitor pattern,
     *  allowing a header to bypass filtering rules
     */
    virtual bool is_accepted_by(const filter_type& filter) const = 0;

protected:
    AbstractMessage()                       = default;
    AbstractMessage(const AbstractMessage&) = default;
    AbstractMessage(AbstractMessage&&)      = default;
};

template <typename Ctx=DefaultContext>
struct AbstractFilter
{
public:
    using context = Ctx;
    using message_type = typeof_message_t<context>;

    /**
     * Returns true if output is enabled for given header,
     *  should be overriden and overloaded to dispatch on header's true type
     */
    virtual bool is_transmission_allowed_for(const message_type&) const = 0;

protected:
    AbstractFilter()                        = default;
    AbstractFilter(const AbstractFilter&)   = default;
    AbstractFilter(AbstractFilter&&)        = default;
};

} /* endof namespace logging */

#include "abc.hpp"

#endif /* DEF_LOGGING_ABC_HXX */
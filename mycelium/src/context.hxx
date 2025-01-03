/**
 * 
 */

#ifndef DEF_CONTEXT_HXX
#define DEF_CONTEXT_HXX

#include "utils/containers/queues.hxx"
#include "error.hxx"

namespace context
{

template <typename Impl>
class ContextCRT
{
    template <typename ...Args>
    static void assert_error(Args... args)
        { Impl::assert_error(args...); }

    template <typename ...Args>
    static bool is_error(Args... args)
        { return Impl::is_error(args...); }

    template <typename ...Args>
    static bool is_recoverable(Args... args)
        { return Impl::is_recoverable(args...); }
};

struct DummyContext
{
    template <typename ...Args>
    static void assert_error(Args...) {}

    template <typename ...Args>
    static bool is_error(Args...) { return false; }

    template <typename ...Args>
    static bool is_recoverable(Args...) { return true; }
    
};

template <typename Transport>
struct DefaultContext
{
    template <typename ...Args>
    void assert_error(Args...);
};

} /* endof namespace context */

#endif /* DEF_CONTEXT_HXX */
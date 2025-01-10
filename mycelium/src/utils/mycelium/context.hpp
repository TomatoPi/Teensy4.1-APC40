/**
 * 
 */

#ifndef DEF_CONTEXT_HXX
#define DEF_CONTEXT_HXX

#include "error.hpp"
#include <cstddef>

#define DECL_ITERATION_SENTINEL \
    size_t __itermax = context::untrusted_iteration_limit

#define ITERATION_SENTINEL_NOT_REACHED \
    (0 < __itermax && __itermax <= context::untrusted_iteration_limit)

#define UPDATE_ITERATION_SENTINEL \
    __itermax -= 1

#define ACCEPT_ITERATION_SENTINEL(LOCATION) \
    if (0 == __itermax) \
        { context::assert_error(error::errcode::INFINITE_LOOP, LOCATION); }

namespace context
{

/**
 * Notify the presence of programming error:
 *  system reached a state which should have never been,
 * Shorthand for raise_error(EMERGENCY, code, msg).
 */
static void assert_error(error::errcode code, const char* msg)
    __attribute__((cold));

/**
 * Returns a number which can be used as a sentinel to bound iterations
 *  where they may be unbounded,
 * Usefull to prevent the program from beeing stuck in an infinite loop
 *  and allows it to run in degraded state.
 */
static constexpr size_t untrusted_iteration_limit = 1024;

} /* endof namespace context */

#endif /* DEF_CONTEXT_HXX */

/**
 * 
 */

#ifndef DEF_MYCELIUM_HXX
#define DEF_MYCELIUM_HXX

#include <cstddef>
#include "../../error.h"

#ifndef ARDUINO_TEENSY41
    #include <cassert>
    #include <iostream>
    #define __MYCELIUM_ASSERT(...) assert(__VA_ARGS__)
#else 
    #define __MYCELIUM_ASSERT(...) error::raise_error()
#endif

namespace mycelium
{

template <typename T>
struct contextof
{
    struct type
    {
        static void assert_error(error::status_byte errno, const char* msg)
            { raise_error(errno, msg); __MYCELIUM_ASSERT(false); }

        static void raise_error(error::status_byte errno, const char* msg)
            {  }

#ifndef ARDUINO_TEENSY41
        struct logger
        {
            template <typename ...Args>
            static void error(const char* fmt, Args ...args)
                { fprintf(s)}
        };
#else
        struct logger
        {

        };
#endif
    };
};

class AbstractContext
{
public:
    virtual void trace(...) const = 0;
};

} /* endof namespace mycelium */

#endif /* DEF_MYCELIUM_HXX */

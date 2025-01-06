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
    #include <chrono>
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
        static void assert_error(error::status_byte errbyte, const char* msg)
            { raise_error(errbyte, msg); __MYCELIUM_ASSERT(false); }

        static error::status_byte raise_error(error::status_byte errbyte, const char* msg)
            { return errbyte; }

        static bool is_error(error::status_byte errbyte)
            { return false; }

#ifndef ARDUINO_TEENSY41
        struct time
        {
            using clock = std::chrono::system_clock;

            struct time_point
            {
                time_point(std::time_t point=clock::now())
                    : point{point}
                    {}

                std::time_t point;
            };
        };
#else
        struct logger
        {

        };
#endif
    };
};

struct DummyContext {};

class AbstractContext
{
public:
    virtual void trace(...) const = 0;
};

} /* endof namespace mycelium */

#endif /* DEF_MYCELIUM_HXX */

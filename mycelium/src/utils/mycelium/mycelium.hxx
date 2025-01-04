/**
 * 
 */

#ifndef DEF_MYCELIUM_HXX
#define DEF_MYCELIUM_HXX

#include <cstddef>

namespace mycelium
{

template <typename T>
struct contextof;

class AbstractContext
{
public:
    virtual void trace(...) const = 0;
};

} /* endof namespace mycelium */

#endif /* DEF_MYCELIUM_HXX */

/**
 * 
 */

#include "set.hxx"

namespace containers
{
namespace set
{

template <typename T, typename CompT, typename SizeT, SizeT Size>
bool
fixed_size_set<T, CompT, SizeT, Size>::add(value_type val)
{
    if (is_full())
        { return false; }

    for (const auto& v: *this)
    {
        if (comparator_type{}(v, val))
            { return false; }
    }

    _datas[_size] = val;
    _size += 1;
    return true;
}

} /* endof namespace set */
} /* endof namespace containers */

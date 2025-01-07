/**
 * 
 */

#include "set.hpp"

namespace containers
{

template <typename T, typename CompT, typename SizeT, SizeT Size>
bool
fixed_size_set<T, CompT, SizeT, Size>::add(value_type val)
    {
        if (is_full())
        {
            context::assert_error(error::errcode::INVALID_CALL, "@set::add: set is full");
            return false;
        }

        for (size_type i=0; i<_size && i<MaxSize; ++i)
        {
            if (comparator_type{}(_datas[i], val))
                { return false; }
        }

        _datas[_size] = val;
        _size += 1;
        return true;
    }

template <typename T, typename CompT, typename SizeT, SizeT Size>
    typename fixed_size_set<T, CompT, SizeT, Size>::value_type&
fixed_size_set<T, CompT, SizeT, Size>::iterator::operator*()
    {
        static value_type __dummy;
        if (!this->operator bool())
        {
            context::assert_error(error::errcode::INVALID_CALL, "@set::iterator: dereferencing invalid iterator");
            return __dummy;
        }
        return set._datas[idx];
    }

template <typename T, typename CompT, typename SizeT, SizeT Size>
    const typename fixed_size_set<T, CompT, SizeT, Size>::value_type&
fixed_size_set<T, CompT, SizeT, Size>::iterator::operator*() const
    {
        static value_type __dummy;
        if (!this->operator bool())
        {
            context::assert_error(error::errcode::INVALID_CALL, "@set::iterator: dereferencing invalid iterator");
            return __dummy;
        }
        return set._datas[idx];
    }

} /* endof namespace containers */

/**
 * 
 */

#ifndef DEF_SET_HXX
#define DEF_SET_HXX

#include "context.hpp"
#include <type_traits>
#include <algorithm>

namespace containers
{

/**
 * Rudimentary set type used to store a few values,
 *  usefull when @c add appens once and set is only used for iteration over values
 */
template <typename T, typename CompT, typename SizeT, SizeT Size>
class fixed_size_set
{
public:
    using value_type        = T;
    using comparator_type   = CompT;
    using size_type         = std::remove_const_t<SizeT>;
    
    static constexpr const size_type MaxSize = Size;
    static_assert(MaxSize < context::untrusted_iteration_limit);

    explicit fixed_size_set(value_type initval=value_type())
        : _datas{{initval}}, _size{0}
        {}

    fixed_size_set(const fixed_size_set&)               = default;
    fixed_size_set& operator= (const fixed_size_set&)   = default;

    fixed_size_set(fixed_size_set&&)                    = default;
    fixed_size_set& operator= (fixed_size_set&&)        = default;

    ~fixed_size_set()                                   = default;

    class iterator
    {
    public:
        using difference_type   = size_t;
        using value_type        = fixed_size_set::value_type;
        using pointer           = value_type*;
        using reference         = value_type&;
        using iterator_category = std::forward_iterator_tag;

        iterator(fixed_size_set& s, size_type idx=0)
            : set{s}, idx{idx}
            {}
        
        operator bool() const       { return 0 <= idx && idx < set._size && idx < MaxSize; }

        value_type& operator* ();
        const value_type& operator* () const;

        iterator& operator++ ()     { idx = idx+1; return *this; }
        
    private:
        fixed_size_set& set;
        size_type idx;
    };


    bool is_empty() const           { return _size == 0; }
    bool is_full() const            { return MaxSize <= _size; }
    size_type size() const          { return _size; }
    
    bool add(value_type val);

    void clear()                    { _size = 0; }

    iterator find(value_type val)   { return std::find(begin(), end(), val); }

    iterator begin()                { return iterator(*this, 0); }
    iterator end()                  { return iterator(*this, _size); }

    const iterator begin() const    { return iterator(*this, 0); }
    const iterator end()   const    { return iterator(*this, _size); }

    const iterator cbegin() const   { return iterator(*this, 0); }
    const iterator cend()   const   { return iterator(*this, _size); }

private:
    value_type _datas[MaxSize];
    size_type _size;

}; /* endof class fixed_size_set */

} /* endof namespace containers */

#include "_set.hpp"

#endif /* DEF_SET_HXX */

/**
 * 
 */

#ifndef DEF_SET_HXX
#define DEF_SET_HXX

namespace containers
{
namespace set
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

    explicit fixed_size_set(value_type initval=value_type())
        : _datas{{initval}}, _size{0}
        {}

    fixed_size_set(const fixed_size_set&)               = default;
    fixed_size_set& operator= (const fixed_size_set&)   = default;

    fixed_size_set(fixed_size_set&&)                    = default;
    fixed_size_set& operator= (fixed_size_set&&)        = default;

    ~fixed_size_set()                                   = default;

    bool is_empty() const           { return _size == 0; }
    bool is_full() const            { return MaxSize <= _size; }
    size_type size() const          { return _size; }
    
    bool add(value_type val);

    value_type begin()              { return _datas[0]; }
    value_type end()                { return _datas[_size]; }

    const value_type begin() const  { return _datas[0]; }
    const value_type end()   const  { return _datas[_size]; }

    const value_type cbegin() const { return _datas[0]; }
    const value_type cend()   const { return _datas[_size]; }

private:
    value_type _datas[MaxSize];
    size_type _size;

}; /* endof class fixed_size_set */

} /* endof namespace set */
} /* endof namespace containers */

#endif /* DEF_SET_HXX */

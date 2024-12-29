/**
 * 
 */

#ifndef DEF_SET_HPP
#define DEF_SET_HPP

#include <type_traits>

namespace utils
{

    /**
     * Rudimentary set type used to store a few values,
     *  usefull when @c add appens once and set is only used for iteration over values
     */
    template <typename T, typename SizeT, SizeT Size>
    struct fixed_size_set
    {
        using value_type = T;
        using size_type = std::remove_const_t<SizeT>;
        
        static constexpr const size_type MaxSize = Size;

        value_type datas[MaxSize];
        size_type size;

        explicit fixed_size_set(value_type val): datas{{val}}, size{0}
            {}

        bool is_full() const
            { return MaxSize <= size; }
        
        bool add(value_type val)
        {
            if (is_full())
                { return false; }

            for (size_type i=0; i<size; ++i)
            {
                if (datas[i] == val)
                    { return false; }
            }

            datas[size] = val;
            size += 1;
            return true;
        }

        value_type begin()  { return datas[0]; }
        value_type end()    { return datas[size]; }

        const value_type begin() const { return datas[0]; }
        const value_type end()   const { return datas[size]; }
    };

} /* namespace utils */

#endif /* DEF_SET_HPP */
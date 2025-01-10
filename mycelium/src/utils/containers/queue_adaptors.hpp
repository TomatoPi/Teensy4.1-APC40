/**
 * 
 */

#ifndef DEF_QUEUE_ADAPTORS_HPP
#define DEF_QUEUE_ADAPTORS_HPP

#include "queues.hpp"

#include <utility>
#include <tuple>
#include <array>

namespace containers
{

/**
 * Transparent adaptor object used to enforce duck-typing over different nodes types
 */
template <typename NodeT, typename ValueT>
struct node_adaptor: public NodeT
{
    using type = NodeT;
    using value_type = ValueT;

    void reinint(value_type val)            { static_cast<type*>(this)->reinint(std::forward<value_type>(val)); }
    value_type operator= (value_type val)   { return static_cast<type&>(*this) = std::forward<value_type>(val); }

    value_type value()                      { return static_cast<type*>(this)->value(); }
    const value_type value() const          { return static_cast<const type*>(this)->value(); }

    void pop_self()                         { static_cast<type*>(this)->pop_self(); }
};

/**
 * Transparent adaptor object used to enforce duck-typing over different queue types
 */
template <typename QueueT>
struct queue_adaptor: public QueueT
{
    using type = QueueT;
    using node_type = node_adaptor<typename type::node_type, typename type::node_type::value_type>;

    node_type* next()               { return static_cast<      type*>(this)->next(); }
    const node_type* next() const   { return static_cast<const type*>(this)->next(); }

    bool push(node_type& node)      { return static_cast<      type*>(this)->push(std::forward<node_type>(node)); }

    void pop() const                {        static_cast<const type*>(this)->pop(); }

    bool is_empty() const           { return static_cast<const type*>(this)->is_empty(); }

    void fast_clear() const         {        static_cast<const type*>(this)->fast_clear(); }
    void deep_clear() const         {        static_cast<const type*>(this)->deep_clear(); }

    template <typename Transport>
    void dump(Transport ts) const   { return static_cast<const type*>(this)->dump(std::forward<Transport>(ts)); }
};

} /* endof namespace containers */

#include "_queue_adaptors.hpp"

#endif /* DEF_QUEUE_ADAPTORS_HPP */

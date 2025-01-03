/**
 * 
 */

#ifndef DEF_STACK_HXX
#define DEF_STACK_HXX

#include "double_linked_list.hxx"

#include <array>

namespace containers
{

template <typename Impl>
class QueueCRT
{
    using node_type = typename Impl::node_type;

    node_type* next()
        { return static_cast<Impl*>(this)->next(); }

    const node_type* next() const
        { return static_cast<const Impl*>(this)->next(); }

    void push(node_type* node)
        { static_cast<Impl*>(this)->push(); }

    void pop()
        { static_cast<Impl*>(this)->pop(); }
};

/**
 * 
 */
template <typename ValueT, typename ContextT>
class stack: public anchored_list<ValueT, ContextT>
{
public:

    node_type* top() { return front(); }
    const node_type* top() const { return front(); }

    void push(node_type* node) { push_front(node); }
};

/**
 * 
 */
template <typename ValueT, typename ContextT>
class queue: public anchored_list<ValueT, ContextT>
{
public:

    node_type* next() { return front(); }
    const node_type* next() const { return front(); }

    void push(node_type* node) { push_back(node); }
};



template <typename ValueT, typename ContextT>
class priority_queue: public anchored_list<ValueT, ContextT>
{

};

} /* endof namespace containers */

#endif /* DEF_STACK_HXX */
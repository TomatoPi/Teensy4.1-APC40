/**
 * 
 */

#ifndef DEF_STACK_HXX
#define DEF_STACK_HXX

#include "double_linked_list.hxx"

#include <array>

namespace containers
{

// template <typename Impl>
// class QueueCRT
// {
//     using node_type = typename Impl::node_type;

//     bool is_empty() const
//         { return static_cast<const Impl*>(this)->is_empty(); }

//     node_type* next()
//         { return static_cast<Impl*>(this)->next(); }

//     const node_type* next() const
//         { return static_cast<const Impl*>(this)->next(); }

//     void push(node_type& node)
//         { static_cast<Impl*>(this)->push(node); }

//     void pop()
//         { if (!is_empty()) { next()->pop_self(); } }
// };

/**
 * 
 */
template <typename ValueT>
class Stack: private anchored_list<ValueT> //, public QueueCRT<Stack<ValueT>>
{
private:
    using base_type = anchored_list<ValueT>;

public:
    using node_type = typename base_type::node_type;

    node_type* next()               { return this->front(); }
    const node_type* next() const   { return this->front(); }

    bool push(node_type& node)      { this->push_front(node); return true; }

    void pop()                      { if (!is_empty()) { next()->pop_self(); } }

    using base_type::is_empty;

    using base_type::fast_clear;
    using base_type::deep_clear;

    using base_type::dump;
};

/**
 * 
 */
template <typename ValueT>
class Queue: private anchored_list<ValueT> //, public QueueCRT<Queue<ValueT>>
{
private:
    using base_type = anchored_list<ValueT>;

public:
    using node_type = typename base_type::node_type;

    node_type* next()               { return this->front(); }
    const node_type* next() const   { return this->front(); }

    bool push(node_type& node)      { this->push_back(node); return true; }

    void pop()                      { if (!is_empty()) { next()->pop_self(); } }

    using base_type::is_empty;

    using base_type::fast_clear;
    using base_type::deep_clear;

    using base_type::dump;
};

template <typename ValueT, typename ComparatorT, size_t SIZE>
class Heap //: public QueueCRT<Heap<ValueT, ComparatorT, SIZE>>
{
public:
    static constexpr const size_t SIZE_MAX = SIZE;

    using value_type = ValueT;
    using context = typename mycelium::contextof<type>::type;

    /**
     * Specialise node type to prevent the priority queue from going into a bad state
     */
    class node_type
    {
    public:
        /** expose default constructor */
        explicit node_type(value_type datas = value_type{})
            : _datas{datas}, _heap{nullptr}, _index{0}
            {}

        /** Copy contruction or assignation not allowed */
        node_type(const node_type&)             = delete;
        node_type& operator= (const node_type&) = delete;

        /** Move construction or assignation not allowed */
        node_type(node_type&& rhs)              = delete;
        node_type& operator= (node_type&& rhs)  = delete;

        /** removes node from the heap on object destruction */
        ~node_type()                            { pop_self(); }

        /** Assignement operator to override holded value, maintains heap structure */
        value_type operator= (value_type val);

        /** Only expose const getter as changing value would put the heap in a bad state */
        const value_type value() const          { return _datas; }

        /** Returns true if node is not inside a heap */
        bool is_orphan()                        { return _heap == nullptr; }

        /**
         * Removes the node from it's heap, preserving it's structure
         * @post @c is_orphan() returns true
         */
        void pop_self();

        friend class Heap;

    // private:
        value_type _datas;
        Heap* _heap;
        size_t _index;
    };
    
    struct comparator_type
    {
        bool operator() (const node_type* lhs, const node_type* rhs) const;
    };

    Heap();

    Heap(const Heap&)               = delete;
    Heap& operator=(const Heap&)    = delete;

    Heap(Heap&&)                    = delete;
    Heap& operator=(Heap&&)         = delete;

    ~Heap();

    /**  */
    bool is_empty() const           { return _size == 0; }

    node_type* next()               { return is_empty() ? nullptr : _nodes[0]; }
    const node_type* next() const   { return is_empty() ? nullptr : _nodes[0]; }

    bool push(node_type& node);

    void clear();

    void pop()                      { if (!is_empty()) { next()->pop_self(); } }

    /** Integrity check, returns true if holded values are valid and forms a heap */
    bool check() const;

    template <typename OutFn>
    void dump(OutFn ofn) const;

private:

    size_t parentof(size_t index)   { return (index-1) / 2; }
    size_t childof(size_t index)    { return (2*index) + 1; }

    void shift_up(size_t index);
    void shift_down(size_t index);

    std::array<node_type*, SIZE_MAX> _nodes;
    size_t _size;
};

} /* endof namespace containers */

#include "queues.hpp"

#endif /* DEF_STACK_HXX */
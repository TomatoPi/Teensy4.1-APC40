/**
 * Utility library for Double linked lists with external memory management
 */

#ifndef DEF_DOUBLE_LINKED_LIST_HXX
#define DEF_DOUBLE_LINKED_LIST_HXX

#include <utility>

namespace containers
{

template <typename ValueT>
class anchored_list;

/**
 * Queue
 * Helper class to store list of requested writes
 *  memory is externaly managed, also caller is allowed to cancel a write at any time
 *  double linked list with anchor point represented with null writer
 * 
 * @param ValueT:   stored data type on each link node
 * @param ContextT: helper type storing error handling and logging facilities
 */
template <typename ValueT>
class double_linked_node
{
public:
    using value_type = ValueT;
    using type = double_linked_node<value_type>;

    friend class anchored_list<ValueT>;

private:
    /** Pointer to stored object, may be null */
    value_type* _datas;

    /** internal members */
    double_linked_node* _prev;
    double_linked_node* _next;

public:
    /** default contructor */
    explicit double_linked_node(value_type* _datas=nullptr)
        : _datas{_datas}, _prev{this}, _next{this}
        {}

    /** Disable Copy construction: hardly definable behaviour */
    double_linked_node(const double_linked_node& list)              = delete;

    /** Disable Copy assignement operator: hardly definable behaviour */
    double_linked_node& operator=(const double_linked_node& list)   = delete;
    
    /** Disable Move constructor */
    double_linked_node(double_linked_node&& list)                   = delete;
    
    /** Disable Move assignement operator: hardly definable behaviour on non empty destination */
    double_linked_node& operator=(double_linked_node&& list)        = delete;

    /** Destructor: remove node from any linked list to prevent weird behavior */
    ~double_linked_node();
    
    /** Pseudo constructor, recreates a fresh node in-place, pops from owning list */
    void reinit(value_type* val);

    /** Assignement opertor to override holded value */
    value_type* operator= (value_type* val);

    /**
     * Dereference operator, access stored datas
     * @warning returns nullptr on anchor nodes
     */
    value_type* value();
    const value_type* value() const;

    /**
     * Returns true if node doesn't holds datas
     */            
    bool is_anchor() const;

    /**
     * Caller is responsible to not call this on an orphan node
     * @warning a node with datas but not in a list appears as empty
     */
    bool is_empty() const;

    /**
     * Returns true if the node does not resides in a list
     * @note orphan nodes behaves as anchors when pushing new nodes into
     */
    bool is_orphan() const;

    /**
     * Returns @c _next ptr
     * @warning an orphan node IS NOT the front, as it's not a list
     */
    double_linked_node* front();
    const double_linked_node* front() const;

    /**
     * Returns @c _prev ptr
     * @warning an orphan node IS NOT the back, as it's not a list
     */
    double_linked_node* back();
    const double_linked_node* back() const;
    
    /**
     * Removes the node from it's list,
     * @note this method can be called by the owner, to cancel a write before it happens
     * @note calling this method on an anchor clears the list
     */
    void pop_self();

    /**
     * Merges to queues together by transfering nodes chain,
     *  if 'node' is an anchor, the corresponding list will be empty after push
     *  else anchor is supposed to be the oldest of a un-anchored list
     *      (which may only contains a single element)
     */
    void push_back(double_linked_node& node);

    void push_front(double_linked_node& node)
        { _next->push_back(node); }

    /**
     * Exchange two nodes within their respective lists,
     * @note if both nodes are empty (not in a list), this is a no-op
     */
    template <typename T>
    friend void swap(double_linked_node<T>& lhs, double_linked_node<T>& rhs);

    /**
     * 
     */
    template <typename OutputFn>
    void dump(OutputFn output_fn) const;

}; /* endof struct double_linked_node */

template <typename ValueT>
class anchored_list
{
public:
    using value_type = ValueT;
    using type = anchored_list<value_type>;
    using node_type = typename double_linked_node<value_type>::type;

private:
    node_type _anchor;

public:
    bool is_empty() const           { return _anchor.is_empty(); }

    /**
     * Remove the anchor from the list, which leaves all existing nodes linked together
     *  Usefull if leaving out an orphaned list is not critical
     * */
    void fast_clear()               { _anchor.pop_self(); }

    /**
     * Clears the list and leave every node as singular orphaned nodes
     *  usefull if @c fast_clear() is not usable
     */
    void deep_clear();

    node_type* front()              { return _anchor.front(); }
    const node_type* front() const  { return _anchor.front(); }

    node_type* back()               { return _anchor.back(); }
    const node_type* back() const   { return _anchor.back(); }

    void push_back(type& lst)       { return _anchor.push_back(lst._anchor); }
    void push_front(type& lst)      { return _anchor.push_front(lst._anchor); }

    void push_back(node_type& node) { return _anchor.push_back(node); }
    void push_front(node_type& node){ return _anchor.push_front(node); }

    template <typename OutputFn>
    void dump(OutputFn outfn) const { return _anchor.dump(outfn); }

    const node_type* anchor() const { return &_anchor; }

}; /** endof class anchored_list */

} /* endof namespace container */

#include "_double_linked_list.hpp"

#endif /* DEF_DOUBLE_LINKED_LIST_HXX */

/**
 * Utility library for Double linked lists with external memory management
 */

#ifndef DEF_DOUBLE_LINKED_LIST_HPP
#define DEF_DOUBLE_LINKED_LIST_HPP

#include <utility>

namespace containers
{

template <typename ValueT, typename ContextT>
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
template <typename ValueT, typename ContextT>
class double_linked_node
{
public:
    using value_type = ValueT;
    using context_type = ContextT;

    friend class anchored_list<ValueT, ContextT>;

private:
    /** Pointer to stored object, may be null */
    value_type* _datas;

    /** internal members */
    double_linked_node* _prev;
    double_linked_node* _next;

public:
    /** default contructor */
    double_linked_node(value_type* _datas=nullptr)
        : _datas{_datas}, _prev{this}, _next{this}
        {}

    /** Disable copy construction: hardly definable behaviour */
    double_linked_node(const double_linked_node& list) = delete;

    /** Disable copy assignement: hardly definable behaviour */
    double_linked_node& operator=(const double_linked_node& list) = delete;
    
    /** Move constructor: move _datas and preserve structure */
    double_linked_node(double_linked_node&& list)
        : double_linked_node(std::move(list._datas))
        {
            if (!list.is_empty())
                {
                    /** Swaps this node with the new one */
                    _next = list._next;
                    _prev = list._prev;
                    _next->_prev = this;
                    _prev->_next = this;
                    list._next = list._prev = &list;
                }
        }
    
    /** Move assignement operator: moves datas and preserve structure */
    double_linked_node& operator=(double_linked_node&& list)
        {
            _datas=std::move(list._datas);
            return *this;
        }

    /** Destructor: remove node from any linked list to prevent weird behavior */
    ~double_linked_node()
        { pop_self(); }
    
    /** Pseudo constructor, recreates the node in-place */
    void init(value_type* val)
        {
            pop_self();
            _datas = val;
        }

    /**
     * Dereference operator, access stored datas
     * @warning returns nullptr on anchor nodes
     */
    value_type* operator*() { return _datas; }
    const value_type* operator*() const { return _datas; }

    /**
     * Returns true if node doesn't holds datas
     */            
    bool is_anchor() const
        { return _datas == nullptr; }

    /**
     * Caller is responsible to not call this on an orphan node
     * @warning a node with datas but not in a list appears as empty
     */
    bool is_empty() const
        { return _next == this; }

    /**
     * Returns true if the node does not resides in a queue
     * @note orphan nodes behaves as anchors when pushing new nodes into
     */
    bool is_orphan() const
        { return is_empty() && !is_anchor(); }

    /**
     * Returns @c _next ptr as pushing is done on @c _prev
     * @warning an orphan node IS NOT the front, as it's not a queue
     */
    double_linked_node* front()
        { return is_empty() ? nullptr : _next; }
        
    const double_linked_node* front() const
        { return is_empty() ? nullptr : _next; }

    double_linked_node* back()
        { return is_empty() ? nullptr : _prev; }

    const double_linked_node* back() const
        { return is_empty() ? nullptr : _prev; }
    
    /**
     * Removes the node from it's queue,
     * @note this method can be called by the owner, to cancel a write before it happens
     * @note calling this method on an anchor clears the queue
     */
    void pop_self()
        {
            _prev->_next = _next;
            _next->_prev = _prev;
            _next = _prev = this;
        }

    /**
     * Merges to queues together by transfering content,
     *  if 'node' is an anchor, the corresponding queue will be empty after push
     *  else anchor is supposed to be the oldest of a un-anchored list
     *      (which may only contains a single element)
     */
    void push_back(double_linked_node* node)
        {
            if (nullptr == node)
                {
                    context_type::assert_error("push: Null pointer argument");
                    return;
                }
            
            if (node->is_anchor())
            {
                if (node->is_empty())
                    { return; /* avoid adding an other anchor */ }

                /* preserve structure */
                node->_next->_prev = _prev;
                node->_prev->_next = this;
                /* new head comes after current queue */
                _prev->_next = node->_next;
                /* new tail comes at last */
                _prev = node->_prev;
                /* preserve empty queue structure */
                node->_next = node->_prev = node;
            }
            else
            {
                double_linked_node* current_last = _prev;
                /* new head (node) comes after current queue */
                _prev->_next = node;
                /* new tail (node->_prev) becames the latest */
                node->_prev->_next = this;
                _prev = node->_prev;
                /* keep structure correctness */
                node->_prev = current_last;
            }
        }

    /**
     * 
     */
    friend void swap(double_linked_node* lhs, double_linked_node* rhs)
        {
            double_linked_node* tmp;
            lhs->_prev->_next = rhs;
            rhs->_prev->_next = lhs;

            lhs->_next->_prev = rhs;
            rhs->_next->_prev = lhs;

            tmp = lhs->_prev;
            lhs->_prev = rhs->_prev;
            rhs->_prev = tmp;

            tmp = lhs->_next;
            lhs->_next = rhs->_next;
            rhs->_next = tmp;
        }

    /**
     * 
     */
    template <typename OutputFn>
    void dump(OutputFn output_fn) const
        {
            if (is_anchor())
                { output_fn("\nAnchor:"); }

            output_fn("\n\tnode=%-10p next=%-10p prev=%-10p _datas={",
                this, _next, _prev);
            if (_datas)
                { _datas->dump(output_fn); }
            output_fn("}");

            if (is_anchor())
                {
                    output_fn("\nNodes:");
                    for (double_linked_node* ptr=_next; ptr!=this; ptr=ptr->_next)
                        { ptr->dump(output_fn); }
                    output_fn("\nEnd ===");
                }
        }

}; /* endof struct double_linked_node */

template <typename ValueT, typename ContextT>
class anchored_list
{
public:
    using value_type = ValueT;
    using context_type = ContextT;

    using node_type = double_linked_node<value_type, context_type>;

private:
    node_type _anchor;

public:
    bool is_empty() const { return _anchor.is_empty(); }

    /**
     * Remove the anchor from the queue, which leaves all existing nodes linked together
     * Usefull if leaving out an orphaned list is not critical
     * */
    void fast_clear() { _anchor.pop_self(); }

    /**
     * 
     */
    void deep_clear()
        {
            node_type* ptr = _anchor._next;
            do
            {
                node_type* next_ptr = ptr->_next;
                ptr->_next = ptr->_prev = ptr;
                ptr = next_ptr;

            } while (ptr != &_anchor);
            _anchor.init(nullptr);
        }

    node_type* front()             { return _anchor.front(); }
    const node_type* front() const { return _anchor.front(); }

    node_type* back()             { return _anchor.back(); }
    const node_type* back() const { return _anchor.back(); }

    void push_back(node_type* node) { return _anchor.push_back(node); }
    void push_front(node_type* node) { return _anchor._next.push_back(node); }

    template <typename OutputFn>
    void dump(OutputFn outfn) const { return _anchor.dump(outfn); }

}; /** endof class anchored_list */

} /* endof namespace container */

#include "double_linked_list.hpp"

#endif /* DEF_DOUBLE_LINKED_LIST_HPP */
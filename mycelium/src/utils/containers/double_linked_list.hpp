/**
 * 
 */

#include "double_linked_list.hxx"

namespace containers
{

//     /** Move constructor: move _datas and preserve structure */
// template <typename ValueT>
// double_linked_node<ValueT>::double_linked_node(double_linked_node&& list)
//     : double_linked_node(std::move(list._datas))
//     {
//         if (!list.is_empty())
//             {
//                 /** Swaps this node with the new one */
//                 _next = list._next;
//                 _prev = list._prev;
//                 _next->_prev = this;
//                 _prev->_next = this;
//                 list._next = list._prev = &list;
//             }
//     }

    /** Destructor: remove node from any linked list to prevent weird behavior */
template <typename ValueT>
double_linked_node<ValueT>::~double_linked_node()
    { pop_self(); }
    
    /** Pseudo constructor, recreates the node in-place */
template <typename ValueT>
void double_linked_node<ValueT>::init(value_type* val)
    {
        pop_self();
        _datas = val;
    }

/** Assignement opertor to override holded value */
template <typename ValueT>
typename double_linked_node<ValueT>::value_type* 
double_linked_node<ValueT>::operator= (value_type* val)
    { _datas = val; return val; }

    /**
     * Dereference operator, access stored datas
     * @warning returns nullptr on anchor nodes
     */
template <typename ValueT>
typename double_linked_node<ValueT>::value_type*
double_linked_node<ValueT>::value()
    { return _datas; }
    
template <typename ValueT>
const typename double_linked_node<ValueT>::value_type*
double_linked_node<ValueT>::value() const
    { return _datas; }

    /**
     * Returns true if node doesn't holds datas
     */            
template <typename ValueT>
bool double_linked_node<ValueT>::is_anchor() const
    { return _datas == nullptr; }

    /**
     * Caller is responsible to not call this on an orphan node
     * @warning a node with datas but not in a list appears as empty
     */
template <typename ValueT>
bool double_linked_node<ValueT>::is_empty() const
    { return _next == this; }

    /**
     * Returns true if the node does not resides in a list
     * @note orphan nodes behaves as anchors when pushing new nodes into
     */
template <typename ValueT>
bool double_linked_node<ValueT>::is_orphan() const
    { return is_empty() && !is_anchor(); }

    /**
     * Returns @c _next ptr
     * @warning an orphan node IS NOT the front, as it's not a list
     */
template <typename ValueT>
double_linked_node<ValueT>*
double_linked_node<ValueT>::front()
    { return is_empty() ? nullptr : _next; }
        
template <typename ValueT>
const double_linked_node<ValueT>*
double_linked_node<ValueT>::front() const
    { return is_empty() ? nullptr : _next; }

    /**
     * Returns @c _prev ptr
     * @warning an orphan node IS NOT the back, as it's not a list
     */
template <typename ValueT>
double_linked_node<ValueT>*
double_linked_node<ValueT>::back()
    { return is_empty() ? nullptr : _prev; }

template <typename ValueT>
const double_linked_node<ValueT>*
double_linked_node<ValueT>::back() const
    { return is_empty() ? nullptr : _prev; }

    /**
     * Removes the node from it's list,
     * @note this method can be called by the owner, to cancel a write before it happens
     * @note calling this method on an anchor clears the list
     */
template <typename ValueT>
void double_linked_node<ValueT>::pop_self()
    {
        _prev->_next = _next;
        _next->_prev = _prev;
        _next = _prev = this;
    }

    /**
     * Merges to queues together by transfering nodes chain,
     *  if 'node' is an anchor, the corresponding list will be empty after push
     *  else anchor is supposed to be the oldest of a un-anchored list
     *      (which may only contains a single element)
     */
template <typename ValueT>
void double_linked_node<ValueT>::push_back(double_linked_node& node)
    {
        auto ptr = &node;
        if (ptr->is_anchor())
        {
            if (ptr->is_empty())
                { return; /* avoid adding an other anchor */ }

            /* preserve structure */
            ptr->_next->_prev = _prev;
            ptr->_prev->_next = this;
            /* new head comes after current list */
            _prev->_next = ptr->_next;
            /* new tail comes at last */
            _prev = ptr->_prev;
            /* preserve empty list structure */
            ptr->_next = ptr->_prev = ptr;
        }
        else
        {
            double_linked_node* current_last = _prev;
            /* new head (ptr) comes after current list */
            _prev->_next = ptr;
            /* new tail (ptr->_prev) becames the latest */
            ptr->_prev->_next = this;
            _prev = ptr->_prev;
            /* keep structure correctness */
            ptr->_prev = current_last;
        }
    }

    /**
     * Exchange two nodes within their respective lists
     * @note if both nodes are empty (not in a list), this is a no-op
     */
template <typename ValueT>
void swap(double_linked_node<ValueT>& lhs, double_linked_node<ValueT>& rhs)
    {
        if (lhs.is_empty() && rhs.is_empty())
            { return; }

        auto replace = [](auto& empty, auto& non_empty) -> void
            {
                empty._next = non_empty._next;
                empty._prev = non_empty._prev;
                non_empty._next->_prev = non_empty._prev->_next = &empty;
                non_empty._next = non_empty._prev = &non_empty;
            };

        if (lhs.is_empty())
        {
            replace(lhs, rhs);
        }
        else if (rhs.is_empty())
        {
            replace(rhs, lhs);
        }
        else
        {
            lhs._prev->_next = &rhs;
            rhs._prev->_next = &lhs;

            lhs._next->_prev = &rhs;
            rhs._next->_prev = &lhs;

            auto tmp = lhs._prev;
            lhs._prev = rhs._prev;
            rhs._prev = tmp;

            tmp = lhs._next;
            lhs._next = rhs._next;
            rhs._next = tmp;
        }
    }

    /**
     * 
     */
template <typename ValueT>
    template <typename OutputFn>
void double_linked_node<ValueT>::dump(OutputFn output_fn) const
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

template <typename ValueT>
void anchored_list<ValueT>::deep_clear()
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

} /* endof namespace containers */

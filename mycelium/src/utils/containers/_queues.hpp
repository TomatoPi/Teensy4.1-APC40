/**
 * 
 */

#include "queues.hpp"

#include <algorithm>

namespace containers
{


template <typename ValueT, typename ComparatorT, size_t SMax>
bool
Heap<ValueT, ComparatorT, SMax>::comparator_type::operator() (const node_type* lhs, const node_type* rhs) const
    {
        return ComparatorT{}(lhs->value(), rhs->value());
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
typename Heap<ValueT, ComparatorT, SMax>::value_type
Heap<ValueT, ComparatorT, SMax>::node_type::operator= (value_type val)
    {
        _datas = val;
        if (is_orphan())
            { return val; /* no heap to preserve */ }

        if (_heap->_size <= _index || _heap->_nodes[_index] != this)
        {
            context::assert_error(error::errcode::INVALID_STATE, "@heap::node::operator=: broken node");
            return val;
        }

        /** might looks dirty but in a correct heap, shifts are no-ops */
        _heap->shift_up(_index);
        _heap->shift_down(_index);

        return val;
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
void
Heap<ValueT, ComparatorT, SMax>::node_type::pop_self()
    {
        if (nullptr == _heap)
            { return; }

        if (_heap->_size <= _index || _heap->_nodes[_index] != this)
        {
            context::assert_error(error::errcode::INVALID_STATE, "@heap::node::pop_self: broken node");
            return;
        }

        size_t last = _heap->_size -1;
        if (last != _index)
        {
            std::swap(_heap->_nodes[_index], _heap->_nodes[last]);
            _heap->_nodes[_index]->_index = _index;
            _heap->_nodes[last] = nullptr;
            _heap->_size -= 1;
            _heap->shift_down(_index);
        }
        else
        { 
            _heap->_nodes[_index] = nullptr;
            _heap->_size -= 1;
        }
        _heap = nullptr;
        _index = 0;
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
Heap<ValueT, ComparatorT, SMax>::Heap()
    : _nodes{}, _size{0}
    {
        for (auto& ptr: _nodes)
            { ptr = nullptr; }
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
Heap<ValueT, ComparatorT, SMax>::~Heap()
    {
        clear();
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
bool
Heap<ValueT, ComparatorT, SMax>::push(node_type& node)
    {
        if (!node.is_orphan())
        {
            context::assert_error(error::errcode::INVALID_ARGUMENT, "@heap::push: called with non orphan node");
            return false;
        }
        if (MaxSize <= _size)
        {
            context::assert_error(error::errcode::INVALID_CALL, "@heap::push: Heap is full");
            return false;
        }

        size_t index=_size;
        _nodes[index] = &node;
        node._index = index;
        node._heap = this;
        _size += 1;

        shift_up(index);
        return true;
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
void
Heap<ValueT, ComparatorT, SMax>::clear()
    {
        for (auto ptr: _nodes)
        {
            if (nullptr != ptr)
            {
                ptr->_heap = nullptr;
                ptr->_index = 0;
            }
        }
        _size = 0;
    }
    
template <typename ValueT, typename ComparatorT, size_t SMax>
void
Heap<ValueT, ComparatorT, SMax>::shift_up(size_t index)
    {
        if (index == 0)
            { return; /* don't shift up root */ }
        
        const size_t parent = parentof(index);

            /* if parent < index */
        if (comparator_type{}(_nodes[parent], _nodes[index]))
        {
            std::swap(_nodes[parent], _nodes[index]);
            _nodes[parent]->_index = parent;
            _nodes[index]->_index = index;
            shift_up(parent);
        }
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
void
Heap<ValueT, ComparatorT, SMax>::shift_down(size_t index)
    {
        if (_size <= (index+1))
            { return; /* don't shift down last object */ }
        
        const size_t child = childof(index);
        if (_size <= child)
            { return; /* don't shift down leaves */ }
        
        node_type** swap_child = nullptr;

        if (_size <= child+1)
            { swap_child = &_nodes[child]; }
        else
        {
            /* test if child < child+1 */
            if (comparator_type{}(_nodes[child], _nodes[child+1]))
                { swap_child = &_nodes[child+1]; }
            else
                { swap_child = &_nodes[child]; }
        }

            /* if node < swap_child */
        if (comparator_type{}(_nodes[index], *swap_child))
        {
            std::swap(_nodes[index], *swap_child);
            std::swap(_nodes[index]->_index, (*swap_child)->_index);
            shift_down((*swap_child)->_index);
        }
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
bool
Heap<ValueT, ComparatorT, SMax>::check() const
    {
        if (MaxSize < _size)
            { return false; }
        
        for (size_t i=0; i<_size && _size < context::untrusted_iteration_limit; ++i)
        {
            if (nullptr == _nodes[i])
                { return false; }
            if (_nodes[i]->_index != i)
                { return false; }
            if (_nodes[i]->_heap != this)
                { return false; }
        }

        return std::is_heap(std::begin(_nodes), std::begin(_nodes) + _size, comparator_type{});
    }

template <typename ValueT, typename ComparatorT, size_t SMax>
    template <typename OutFn>
void
Heap<ValueT, ComparatorT, SMax>::dump(OutFn ofn) const
    {
        ofn("\nStack: size=%lu datas=[", _size);
        /* no iteration sentinel: uses builtin range over std::array */
        for (const auto& ptr: _nodes)
        {
            // ofn(" %p:", ptr);
            if (ptr)
            {
                ptr->value().dump(ofn);
                ofn(":%lu", ptr->_index);
            }
        }
        ofn(" ]");
    }

} /* endof namespace containers */
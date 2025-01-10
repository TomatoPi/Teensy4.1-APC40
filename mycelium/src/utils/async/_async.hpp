/**
 * 
 */

#include "async.hpp"

#include "context.hpp"

namespace async
{

template <typename Q>
    async_state
Scheduler<Q>::state() const
    {
        if (!is_ready())
            { return async_state::MasterBusy; }
        
        if (_queue.is_empty())
            { return async_state::Idle; }
        else
            { return _queue.next()->value().state(); }
    }

template <typename Q>
    async_state
Scheduler<Q>::update()
    { return do_update(context::untrusted_iteration_limit); }

template <typename Q>
    async_state
Scheduler<Q>::do_update(size_t recursion_sentinel)
    {
        if (recursion_sentinel == 0)
            {
                context::assert_error(error::errcode::INFINITE_LOOP, "@scheduler::update: max recursion depth reached");
                return async_state::MasterFailed;
            }

        if (_queue.is_empty())
            { return async_state::Idle; }
        
        node_type* nptr = _queue.next();
        if (nullptr == nptr)
            {
                context::assert_error(error::errcode::INVALID_STATE, "@scheduler::update: next is null");
                return async_state::Failed;
            }
        coro_type* cptr = nptr->value();
        if (nullptr == cptr)
            {
                context::assert_error(error::errcode::INVALID_STATE, "@scheduler::update: coro is null");
                return async_state::Failed;
            }

        switch (cptr->state())
        {
            /** Coroutine not started */
        case async_state::Idle:
            __attribute__((fallthrough));

            /** Tried to launch but master not ready */
        case async_state::MasterBusy:
            __attribute__((fallthrough));

            /** Previously tried to launch but not ready */
        case async_state::Waiting:
            if (!is_ready())
                { return cptr->_state = async_state::MasterBusy; }
            else if (!cptr->is_ready())
                { return cptr->_state = async_state::Waiting; }
            else
                { return cptr->_state = cptr->launch(); }
        
            /** Coroutine waiting for completion */
        case async_state::Launched:
            if (cptr->is_finished())
                { return cptr->_state = async_state::Finished; }
            else
                { return waiting_for_completion(*cptr); }

            /** Remove coroutine and recall for update */
        case async_state::Finished:
            nptr->pop_self();
            return do_update(recursion_sentinel -1); /* may not explode as wqueue is decreasing */

            /**
             * Has already failed but may be successful
             * @warning caller is responsible for aborting task if too many failures
             */
        case async_state::Recoverable:
            return recover(*cptr);

            /** Remove coroutine and recall for update */
        case async_state::Failed:
            nptr->pop_self();
            return do_update(recursion_sentinel -1); /* may not explode as wqueue is decreasing */

        case async_state::MasterFailed:
            context::assert_error(error::errcode::INVALID_CALL, "@scheduler::update: master not available");
            return async_state::MasterFailed;

        default:
            /* should not happens: all cases covered */
            context::assert_error(error::errcode::INVALID_STATE, "@scheduler::update: invalid coro state");
            return async_state::Failed;
        }
    }


template <typename Q>
    typename Scheduler<Q>::node_type*
Scheduler<Q>::next()
    { return _queue.next(); }

template <typename Q>
    error::status_byte
Scheduler<Q>::push(node_type& node)
    {
        if (!_queue.push(node))
            { return error::errcode::MEMORY_ERROR | error::severity::ERROR; }
        node.value()->_state = async_state::Idle;
    }

template <typename Q>
    void
Scheduler<Q>::fast_clear()
    { _queue.fast_clear(); }

template <typename Q>
    void
Scheduler<Q>::deep_clear()
    { _queue.deep_clear(); }


template <typename Q>
async_state Scheduler<Q>::set_coro_state(coro_type& coro, async_state s)
    { return coro._state = s; }


} /* endof namespace async */

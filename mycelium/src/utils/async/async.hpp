/**
 * 
 */

#ifndef DEF_ASYNC_HPP
#define DEF_ASYNC_HPP

#include "queue_adaptors.hpp"
#include "error.hpp"

namespace async
{
/**
 * 
 */
enum class async_state
{
    /** 
     * Waiting for run states:
     *  one should try launch the callback
     */
    Idle=0,             ///< Task has not been launched yet
    MasterBusy=1,       ///< Scheduler not ready to launch a task
    Waiting=2,          ///< Tried to launch task but not ready
    /**
     * Running states
     */
    Launched=3,         ///< Task launched, waiting for completion
    /**
     * Finished states:
     *  may have failed or not
     */
    Finished=4,         ///< Task done with success
        /**
         * Failure states
         */
    Recoverable=5,      ///< Task has failed but launching it again may work
    Timedout=6,         ///< Task has failed to run in imparted time
    Failed=7,           ///< Task has failed, launching again is useless

    MasterFailed=8,     ///< Scheduler failure, async engine not available

}; /* endof enum async_state */

static constexpr bool is_error(async_state s)
    { return async_state::Recoverable <= s; }

template <typename Q>
class Scheduler;

class Coroutine
{
public:

    template <typename Q>
    friend class Scheduler;

    Coroutine()
        : _state{async_state::Idle}
        {}

    virtual bool is_ready() const = 0;
    virtual async_state launch() = 0;
    virtual bool is_finished() const = 0;

    async_state state() const { return _state; }

private:

    async_state _state;
};

// template <template <class> class QueueT>
template <typename QueueT>
class Scheduler
{
public:
    using queue_type = containers::queue_adaptor<QueueT>;
    using node_type = containers::node_adaptor<typename QueueT::node_type, Coroutine*>;
    using coro_type = typename node_type::value_type;

    /**
     * Customisation point to disable all updates and coroutines
     *  handled by this scheduler,
     * Should returns true if the scheduler is able to run pending tasks
     */
    virtual bool is_ready() const = 0;

    /**
     * Shorthand for next()->state() with a simple switch over scheduler state
     *  if @c is_ready() is false, returns MasterBusy
     *  if @c next() is null, returns Idle
     */
    async_state state() const;
    /**
     * Tries to run next() coroutine or test for it's completion, returns Idle if there are no pending jobs
     * 
     * Moreover if next task is not aborted by the user, successing call to update 
     *      will always return values running through the following sequence:
     * 
     *  *empty queue*, [Idle...], *push*, [Waiting...], [Launched ...], [Finished | Failed]
     */
    async_state update();

    node_type* next();
    error::status_byte push(node_type& node);

    void fast_clear();
    void deep_clear();

protected:

    /**
     * 
     */
    static async_state set_coro_state(coro_type& coro, async_state s);

    /**
     * Customisation point called when coroutine pointed by @c next() is Launched
     *  allows the scheduler to fine tune it's behaviour when waiting for task completion.
     * 
     * @param coro: 
     */
    virtual async_state waiting_for_completion(coro_type& coro) = 0;

    /**
     * Customisation point called when a coroutine is in @c Recoverable state
     *  the scheduler is responsible to change this state depending on the error context.
     */
    virtual async_state recover(coro_type& coro) = 0;

private:
    async_state do_update(size_t recursion_sentinel);

    queue_type _queue;
};

} /* endof namespace async */

#include "_async.hpp"

#endif /* DEF_ASYNC_HPP */

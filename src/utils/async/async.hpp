/**
 * 
 */

#ifndef DEF_ASYNC_HPP
#define DEF_ASYNC_HPP

#include "../containers/double_linked_list.hpp"

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
    Waiting=1,          ///< Tried to launch task but not ready for complete
    /**
     * Finished states:
     *  may have failed or not
     */
    Finished=2,         ///< Task done with success
        /**
         * Failure states
         */
    Recoverable=3,      ///< Task has failed but launching it again may work
    Failed=4,           ///< Task has failed, launching again is useless
    Timedout=5,         ///< Task has failed to run in imparted time
}; /* endof enum async_state */

enum class launch_state
{
    Launched=0,
    NotReady=1,
    Failed=2,
};

// template <typename ContextT>
// class coroutine
// {
// public:
//     using context_type = ContextT;

//     bool is_ready() const = 0;
//     launch_state launch() = 0;
//     bool is_finished() const = 0;
// };

/**
 * 
 */
template <typename MasterT, typename CallbackT, typename ContextT>
class async_master
{
public:
    using master_type = MasterT;
    using context_type = ContextT;
    using callback_type = CallbackT;

    using queue_type = containers::anchored_list<callback_type, context_type>;
    using node_type = typename queue_type::node_type;

    async_state state() const
    {
        if (wqueue.is_empty())
            { return async_state::Idle; }
        else
            { return wqueue.oldest()->status(); }
    }

    async_state update()
    {
        if (wqueue.is_empty())
            { return async_state::Idle; }

        node_type* ptr = wqueue.front();
        if (nullptr == ptr || nullptr == *ptr)
            {
                callback_type* callptr = ptr ? *ptr : nullptr;
                context_type::assert_error("Null frontptr{%p} or null datas{%p}", ptr, callptr);
                return async_state::Failed;
            }
        
        switch ((*ptr)->status())
        {
            /** Coroutine not started */
        case async_state::Idle:
            if (!master.read())
                { return async_state::Waiting; }
            else
                { return _call_coro(ptr); }

            /** Coroutine waiting for completion */
        case async_state::Waiting:
            if (!master.ready())
                { return I2CError::master_not_ready; }
            else
            {
                if (!master.has_error() && ptr->writer->successfull(master))
                    { return update_coro_state(ptr, I2CError::ok, async_state::Finished); }
                else
                    /* hard error or mismatch between sent bits and expected, might be recoverable */
                    {
                        ASYNC_GPRINT_RAW("\n\tERROR: errno=%u sent_bytes=%lu expected=%u",
                            master.error(), master.get_bytes_transferred(), ptr->writer->num_bytes);
                        return update_coro_state(ptr, master.error(), async_state::Recoverable);
                    }
            }

            /** Remove coroutine and recall for update */
        case async_state::Finished:
            ASYNC_GPRINT_RAW("state=Finished", master_index);
            ptr->pop_self();
            return update(); /* may not explode as wqueue is decreasing */
        
            /**
             * Has already failed but may be successful
             * @warning caller is responsible for aborting task if too many failures
             */
        case async_state::Recoverable:
            ASYNC_GPRINT_RAW("state=Recoverable", master_index);
            return call_coro(ptr);

            /** Remove coroutine and recall for update */
        case async_state::Failed:
            ASYNC_GPRINT_RAW("state=Failed", master_index);
            ptr->pop_self();
            return update(); /* may not explode as wqueue is decreasing */
            
        default:
            /* should not happens: all cases covered */
            context_type::assert_error("\n\tBAD STATE");
            return I2CError::ok;
        }
    }

    bool finish(unsigned long timeout_ms=20)
    {
        elapsedMillis elapsed;
        while (elapsed < timeout_ms)
        {
            if (master.finished())
                { return true; }
        }
        return false;
    }

    /**
     * Never returns a nullptr
     */
    const write_queue& pending_calls() const
        { return wqueue; }

    write_queue* next_call()
        { return wqueue.oldest(); }
    
    const write_queue* next_call() const
        { return wqueue.oldest(); }

    void push(write_queue* node)
        { wqueue.push(node); }

    uint8_t id() const
        { return master_index; }

    template <typename OutputFn>
    void dump(OutputFn outfn) const
    {
        Serial.printf("\nMaster=%u\nAnchor", master_index);
        wqueue.debug_node();
        Serial.println("\nList");
        size_t n=0;
        for (write_queue* ptr=wqueue.next; n<10 && ptr != &wqueue; ptr=ptr->next, ++n)
            { ptr->debug_node(); }
        Serial.println("\nEnd");
    }

private:

    /**
     * Private constructor ensures no illegal build
     *  @c async_master::get<>()
     */
    async_master(I2CMaster& master, uint8_t master_index):
        master{master}, wqueue{}, master_index{master_index}
        {}

    I2CError _update_coro_state(write_queue* ptr, I2CError err, async_state ok_state)
    {
        ASYNC_GPRINT_RAW("\n\tupdate_coro: err=%u ok_state=%u", err, ok_state);
        if (I2CError::ok == err)
            { ptr->state = ok_state; }
        else if (i2c_error_is_recoverable(err))
            /* Transaction failed but may be successfull later */
            { ptr->state = async_state::Recoverable; }
        else
            /* bad request */
            { ptr->state = async_state::Failed; }

        /* propagate error */
        return err;
    }

    I2CError _call_coro(write_queue* ptr)
    {
        ASYNC_GPRINT_RAW("\n\tcalling coro: ptr=%p writer=%p", master_index, ptr, ptr->writer);

        if (!master.finished())
            { return I2CError::master_not_ready; }

        I2CError err = (*(ptr->writer))(master);
        /** I2C takes time, transaction will never be done yet, set state to @c Waiting */
        _update_coro_state(ptr, err, async_state::Waiting);
        return err;
    }

    master_type& master;
    queue_type wqueue;

}; /* endof class async_master */

} /* endof namespace async */

#endif /* DEF_ASYNC_HPP */

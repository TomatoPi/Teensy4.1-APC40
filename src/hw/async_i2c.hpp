/**
 * 
 */

#ifndef DEF_ASYNC_I2C_HPP
#define DEF_ASYNC_I2C_HPP

#include <i2c_driver.h>
#include <imx_rt1060/imx_rt1060_i2c_driver.h>
#include <Arduino.h>

#ifndef NDEBUG
    #define ASYNC_GPRINT(...) \
        if (Serial && hw::async::DebugAsync) \
            { Serial.print("\n[Async]:" __VA_ARGS__); }

    #define ASYNC_GPRINTLN(...) \
        if (Serial && hw::async::DebugAsync) \
            { Serial.println("\n[Async]:" __VA_ARGS__); }

    #define ASYNC_GPRINTF(...) \
        if (Serial && hw::async::DebugAsync) \
            { Serial.printf("\n[Async]:" __VA_ARGS__); }

    #define ASYNC_GPRINT_RAW(...) \
        if (Serial && hw::async::DebugAsync) \
            { Serial.printf(__VA_ARGS__); }

    #define ASYNC_IFG(CODE) \
        if (hw::async::DebugAsync) \
            { CODE }

#else
    #define ASYNC_GPRINT(...)    {}
    #define ASYNC_GPRINTLN(...)  {}
    #define ASYNC_GPRINTF(...)   {}
    #define ASYNC_GPRINT_RAW(...){}
    #define ASYNC_IFG(CODE)      {}
#endif

namespace hw
{
    namespace async
    {
        extern bool DebugAsync;
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
             * Runned states:
             *  may have failed or not
             */
            Finished=2,         ///< Task done with success
                /**
                 * Failure states
                 */
            Recoverable=3,      ///< Task has failed but launching it again may work
            Failed=4,           ///< Task has failed, launching again is useless
        };

        bool i2c_error_is_recoverable(I2CError err)
        {
            switch (err)
            {
            case I2CError::ok:
            case I2CError::arbitration_lost:
            case I2CError::master_not_ready:
            case I2CError::master_fifo_error:
            case I2CError::master_fifos_not_empty:
            case I2CError::data_nak:
            case I2CError::bit_error:
                return true;
            case I2CError::buffer_overflow:
            case I2CError::buffer_underflow:
            case I2CError::invalid_request:
            case I2CError::address_nak:
                return false;
            default:
                return false;
            }
        }
        
        /**
         * Functor utility object to represent a writing to an i2c device
         */
        struct async_write
        {
            uint8_t address;
            const uint8_t* buffer;
            size_t num_bytes;
            bool send_stop;

            /**
             * Coroutine callback
             *  - MUST NOT be called before a succesfull call to @c ready()
             */
            I2CError operator() (I2CMaster& master) const
            {
                master.write_async(address, buffer, num_bytes, send_stop);
                return master.error();
            }

            // /**
            //  * Polls coroutine pre-condition
            //  *  - @c operator() MUST NOT be called if @c ready() returns false
            //  * 
            //  * @bug May returns true if an other @c async_wryte on the same master has finished
            //  *  thus, running this coroutine may break the finished task if send_stop is false
            //  *  moreover, once we've launched this one, the finished task may report as not finished
            //  * 
            //  * @bug MUST implement a notion of physical resources to mitigate race conditions
            //  */
            // bool ready(I2CMaster& master) const
            //     { return master.finished(); }

            // /**
            //  * Polls coroutine finished condition
            //  *  - this call is irrevelant if @c operator() has not been called
            //  */
            // bool done(I2CMaster& master) const
            //     { return master.finished(); }

            /**
             * Polls coroutine post-condition
             *  - this call is irrevelant if @c done() doesn't returns true
             */
            bool successfull(I2CMaster& master) const
                { return num_bytes == 0 || num_bytes == master.get_bytes_transferred(); }

            void debug_call() const
            {
                Serial.printf("\taddress=%u num_bytes=%lu send_stop=%u datas_ptr=%p",
                    address, num_bytes, static_cast<uint8_t>(send_stop), buffer);
                if (num_bytes != 0)
                {
                    Serial.print(" 0x[");
                    for (size_t i=0; i<num_bytes; ++i)
                        { Serial.printf(" %x", buffer[i]); }
                    Serial.print(" ]");
                }
            }
        };

        /**
         * FIFO queue:
         * 
         * Helper class to store list of requested writes
         *  memory is externaly managed, also caller is allowed to cancel a write at any time
         *  double linked list with anchor point represented with null writer
         */
        struct write_queue
        {
            const async_write* writer;
            write_queue* prev;
            write_queue* next;

            async_state state;

            write_queue():
                writer{nullptr}, prev{this}, next{this}, state{async_state::Idle}
                { } // Serial.println("New node"); debug_node(); }

            void init(const async_write* writer)
            {
                this->writer = writer;
                this->state = async_state::Idle;
            }

            const async_write* datas() const
                { return writer; }
            
            const async_state status() const
                { return state; }

            /**
             * range based iteration adaptors
             * @warning non-anchored lists are not trivially iterables, as first not will be ignored
             */
            write_queue* begin(){ return next; }
            write_queue* end()  { return this; }

            const write_queue* begin()  const { return next; }
            const write_queue* end()    const { return this; }

            /**
             * Returns true if node is not callable
             */            
            bool is_anchor() const
                { return writer == nullptr; }

            /**
             * Caller is responsible to not call this on an orphan node
             * @warning a node with a writer but no anchor appears as empty
             */
            bool is_empty() const
                { return next == this; }

            /**
             * Returns true if the node does not resides in a queue
             * @note orphan nodes behaves as anchors when pushing new nodes into
             */
            bool is_orphan() const
                { return is_empty() && !is_anchor(); }

            /**
             * Returns @c next ptr as pushing is done on @c prev
             * @warning an orphan node IS NOT the oldest, as it's not a queue
             */
            write_queue* oldest()
                { return is_empty() ? nullptr : next; }
                
            const write_queue* oldest() const
                { return is_empty() ? nullptr : next; }
            
            /**
             * Removes the node from it's queue,
             * @note this method can be called by the owner, to cancel a write before it happens
             * @note calling this method on an anchor clears the queue
             */
            void pop_self()
            {
                prev->next = next;
                next->prev = prev;
                next = prev = this;
            }

            /**
             * Merges to queues together by transfering content,
             *  if 'node' is an anchor, the corresponding queue will be empty after push
             *  else anchor is supposed to be the oldest of a un-anchored list
             *      (which may only contains a single element)
             */
            void push(write_queue* node)
            {
                if (node->is_anchor())
                {
                    if (node->is_empty())
                        { return; /* avoid adding an other anchor */ }

                    /* preserve structure */
                    node->next->prev = prev;
                    node->prev->next = this;
                    /* new head comes after current queue */
                    prev->next = node->next;
                    /* new tail comes at last */
                    prev = node->prev;
                    /* preserve empty queue structure */
                    node->next = node->prev = node;
                }
                else
                {
                    write_queue* current_last = prev;
                    /* new head (node) comes after current queue */
                    prev->next = node;
                    /* new tail (node->prev) becames the latest */
                    node->prev->next = this;
                    prev = node->prev;
                    /* keep structure correctness */
                    node->prev = current_last;
                }
            }

            void debug_node() const
            {
                Serial.printf("\n\tnode=%-10p writer=%-10p next=%-10p prev=%-10p state=%u",
                    this, writer, next, prev, static_cast<uint8_t>(state));
                if (writer)
                    { writer->debug_call(); }
            }

        }; /* endof struct write_queue */


        class async_master
        {
        public:

            /**
             * Template singleton delegated constructor
             *  Ensures that all @c async_master are built statically
             *  and eliminates risks of duplication over the same I2C bus
             */
            template <decltype(Master)& MASTER>
            static async_master& get()
            {
                uint8_t index;
                if (&MASTER == &Master)
                    { index = 0; }
                else if (&MASTER == &Master1)
                    { index = 1; }
                else if (&MASTER == &Master2)
                    { index = 2; }

                static async_master m(MASTER, index);
                return m;
            }

            async_state state() const
            {
                if (wqueue.is_empty())
                    { return async_state::Idle; }
                else
                    { return wqueue.oldest()->status(); }
            }

            I2CError update()
            {
                ASYNC_GPRINTF("Master:%u begin update ... ", master_index);

                if (wqueue.is_empty())
                    { return I2CError::ok; }
                
                if (!master.finished())
                {
                    ASYNC_GPRINT_RAW("Busy bus");
                    return I2CError::master_not_ready;
                }

                ASYNC_IFG(
                    Serial.printf("queue not empty ... ");
                    debug_queue();
                );

                write_queue* ptr = wqueue.oldest();
                switch (ptr->status())
                {
                    /** Coroutine not started */
                case async_state::Idle:
                    ASYNC_GPRINT_RAW("state=Idle", master_index);
                    return call_coro(ptr);

                    /** Coroutine waiting for completion */
                case async_state::Waiting:
                    ASYNC_GPRINT_RAW("state=Waiting", master_index);
                    if (!master.finished())
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
                    ASYNC_GPRINT_RAW("\n\tBAD STATE");
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

            void debug_queue() const
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

            I2CError update_coro_state(write_queue* ptr, I2CError err, async_state ok_state)
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

            I2CError call_coro(write_queue* ptr)
            {
                // Serial.printf("Master:%u calling coro: ptr=%p writer=%p\n", master_index, ptr, ptr->writer);

                if (!master.finished())
                    { return I2CError::master_not_ready; }

                I2CError err = (*(ptr->writer))(master);
                /** I2C takes time, transaction will never be done yet, set state to @c Waiting */
                update_coro_state(ptr, err, async_state::Waiting);
                return err;
            }

            I2CMaster& master;
            write_queue wqueue;
            uint8_t master_index;

        }; /* endof struct async_master */

        /**
         * Conveniant wrapper holding physical infos for a single device
         */
        struct async_device
        {
            enum class state : uint8_t
            {
                Unknown = 0,
                Available = 1,
                Failed = 2
            };

            async_master& master;
            uint8_t address;
            state status;

            explicit operator bool() const
                { return status != state::Failed; }
        }; /* endof async_device */

    } /* endof namespace async */

} /* endof namespace i2c */

#endif /* DEF_ASYNC_I2C_HPP */
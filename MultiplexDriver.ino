/**
 * 
 */

#include "MultiplexDriver.hpp"

#include <Arduino.h>

#include "src/hw/async_i2c.hpp"

#include "set.hpp"

#define FLEDS       "LEDS_DRIVER"
#define FINS        "INS_DRIVER"

/** ********************************************************
 *  Static members initialisation
 */
        
hw::LedsDriver hw::LedsDriver::GlobalDriver;
bool hw::LedsDriver::DebugDriver = true;

hw::async::async_device hw::MCP_DEVICES[hw::MCP_COUNT] = {
    {hw::async::async_master::get<Master>(), hw::mcp23017::MCP_BASE_ADDR  , hw::async::async_device::state::Unknown},
    {hw::async::async_master::get<Master>(), hw::mcp23017::MCP_BASE_ADDR+1, hw::async::async_device::state::Unknown},
    {hw::async::async_master::get<Master>(), hw::mcp23017::MCP_BASE_ADDR+2, hw::async::async_device::state::Unknown},
};

using masters_set = utils::fixed_size_set<hw::async::async_master*, hw::mcp_count_type, hw::MCP_COUNT>;
masters_set _targeted_masters(nullptr);

/** ********************************************************
 *  Functor helpers
 */

static const hw::async::async_write async_address_poll(const hw::async::async_device& dev, const bool send_stop)
{
    static uint8_t buffer[1] = {0}; // safety to avoid null pointer dereference
    return { .address=dev.address, .buffer=buffer, .num_bytes=0, .send_stop=send_stop };
}

static hw::async::async_write async_setup_iocon(const hw::async::async_device& dev, const bool send_stop)
{
    using namespace hw::mcp23017;
    /**
     * enable third MSB bit which disable sequential operation,
     *  as BANK=0, address pointer will toggle between A/B registers pair
     *  so writing two bytes in IODIRA will push the second in IODIRB
     */
    static constexpr size_t IOCON_SIZE = 2;
    static constexpr uint8_t DEFAULT_IOCON = 0x00 | IOCON_bit::SEQOP;
    static constexpr uint8_t iocon_buffer[IOCON_SIZE] = {
        static_cast<uint8_t>(registers::IOCON),
        DEFAULT_IOCON
    };
    return { .address=dev.address, .buffer=iocon_buffer, .num_bytes=IOCON_SIZE, .send_stop=send_stop };
    // master.write_async(address, iocon_buffer, IOCON_SIZE, send_stop);
    // return master.error();
}

static hw::async::async_write async_setup_iodir(const hw::async::async_device& dev, const bool send_stop)
{
    using namespace hw::mcp23017;
    /**
     * Setup all pins as output (device default is input)
     *  as SEQOP=1 and BANK=0, we can send both IODIRA and IODIRB in a single transaction
     */
    static constexpr size_t IODIR_SIZE = 3;
    static constexpr uint8_t DEFAULT_IODIR = 0x00;
    static constexpr uint8_t iodir_buffer[IODIR_SIZE] = {
        static_cast<uint8_t>(registers::IODIRA),
        DEFAULT_IODIR,
        DEFAULT_IODIR
    };
    return { .address=dev.address, .buffer=iodir_buffer, .num_bytes=IODIR_SIZE, .send_stop=send_stop };
    // master.write_async(address, iodir_buffer, IODIR_SIZE, send_stop);
    // return master.error();
}

/** ********************************************************
 *  LedsDriver methods
 */

#ifndef NDEBUG
    #define LEDS_GPRINT(...) \
        if (Serial && hw::LedsDriver::DebugDriver) \
            { Serial.print("\n[LedsDriver]:" __VA_ARGS__); }

    #define LEDS_GPRINTLN(...) \
        if (Serial && hw::LedsDriver::DebugDriver) \
            { Serial.println("\n[LedsDriver]:" __VA_ARGS__); }

    #define LEDS_GPRINTF(...) \
        if (Serial && hw::LedsDriver::DebugDriver) \
            { Serial.printf("\n[LedsDriver]:" __VA_ARGS__); }

    #define LEDS_GPRINT_RAW(...) \
        if (Serial && hw::LedsDriver::DebugDriver) \
            { Serial.printf(__VA_ARGS__); }

    #define LEDS_IFG(CODE) \
        if (hw::LedsDriver::DebugDriver) \
            { CODE }

    #define SHOW_MASTERS(SET) \
        LEDS_GPRINTF("Pending Masters:"); \
        LEDS_IFG( \
            for (auto master: SET.datas) \
                { Serial.printf("\n\t%p", master); } \
        )

#else
    #define LEDS_GPRINT(...)    {}
    #define LEDS_GPRINTLN(...)  {}
    #define LEDS_GPRINTF(...)   {}
    #define LEDS_GPRINT_RAW(...){}
    #define LEDS_IFG(CODE)      {}
    #define SHOW_MASTERS(SET)   {}
#endif

/**
 * Initialise I2C bus and MCP23017 registers
 */
err::errcode hw::LedsDriver::setup(const unsigned long refresh_rate, const unsigned long timeout_ms)
{
    using namespace hw::async;
    /**
     * reserve memory on the stack
     * @warning reassignating write_queue objects may break structure !!
     *      use wnode[i].writer = ...; instead
     */
    constexpr const hw::mcp_count_type PER_DEV_MSGS_COUNT = 3;
    constexpr const mcp_count_type WRITE_NODES_COUNT = MCP_COUNT * PER_DEV_MSGS_COUNT;
    auto base_indexof = [](mcp_count_type mcp) -> mcp_count_type
        { return PER_DEV_MSGS_COUNT * mcp; };

    async_write  addr_poll[MCP_COUNT]       = {0};
    async_write  iocons[MCP_COUNT]          = {0};
    async_write  iodirs[MCP_COUNT]          = {0};
    write_queue  wnodes[WRITE_NODES_COUNT];


    static uint8_t tmp_buffer[2] = {0};
    tmp_buffer[0] = static_cast<uint8_t>(mcp23017::registers::IODIRA);
    tmp_buffer[1] = 0x00;// static_cast<uint8_t>(0x01) << _current_index;
    Master.write_async(mcp23017::MCP_BASE_ADDR+3, tmp_buffer, 2, true);

    /**
     * Enqueue write calls
     * */
    LEDS_GPRINTLN("Creating Queues");

    _targeted_masters = masters_set(nullptr);
    for (mcp_count_type i=0; i<MCP_COUNT; ++i)
    {
        /* fill write calls */
        addr_poll[i] = async_address_poll(MCP_DEVICES[i], true);
        iocons[i] = async_setup_iocon(MCP_DEVICES[i], true);
        iodirs[i] = async_setup_iodir(MCP_DEVICES[i], true);

        /* enqueue calls */
        wnodes[base_indexof(i) +0].init(addr_poll+i);
        wnodes[base_indexof(i) +1].init(iocons +i);
        wnodes[base_indexof(i) +2].init(iodirs +i);

        LEDS_IFG(
            Serial.printf("\nNODES @%u:\n", i);
            wnodes[base_indexof(i)+0].debug_node();
            wnodes[base_indexof(i)+1].debug_node();
            wnodes[base_indexof(i)+2].debug_node();
            )

        MCP_DEVICES[i].master.push(wnodes + base_indexof(i) +0);
        MCP_DEVICES[i].master.push(wnodes + base_indexof(i) +1);
        MCP_DEVICES[i].master.push(wnodes + base_indexof(i) +2);

        _targeted_masters.add(&MCP_DEVICES[i].master);
    }
    SHOW_MASTERS(_targeted_masters);

    /**
     * Send all registers
     * */
    LEDS_GPRINTLN("Sending registers");

    elapsedMillis elapsed;
    bool all_masters_ok = false;
    while (elapsed < timeout_ms && !all_masters_ok)
    {
        LEDS_GPRINTF("Elapsed: %lu", static_cast<unsigned long int>(elapsed));
        all_masters_ok = true;
        for (auto master: _targeted_masters.datas)
        {
            if (nullptr == master)
                { continue; }
            LEDS_GPRINT_RAW("\tbus=%u ... ", master->id())

            if (master->next_call() == nullptr)
                { continue; }
            else
                { all_masters_ok = false; }

            I2CError errno = master->update();
            LEDS_GPRINTF("Returned errno: %u ... ", static_cast<uint8_t>(errno));

            if (errno == I2CError::ok)
                { continue; }
            else            
            {
                if (errno == I2CError::master_not_ready)
                    { continue; }
                else if (i2c_error_is_recoverable(errno))
                    { continue; /* nothing to be done, only hopes */ }
                else
                    { 
                        LEDS_GPRINT_RAW("Unrecoverable error: ");
                        write_queue* bad_write = master->next_call();
                        const size_t node_offset = bad_write - wnodes;
                        const size_t mcp_index = node_offset / PER_DEV_MSGS_COUNT;

                        if (
                            nullptr == bad_write
                            || bad_write < wnodes+0
                            || wnodes+WRITE_NODES_COUNT <= bad_write
                            )
                            {
                                LEDS_GPRINT_RAW("ILLEGAL STATE: next_write=%p node_offset=%lu mcp_index=%lu ... ABORT",
                                    bad_write, node_offset, mcp_index);
                                return err::errcode::INVALID_STATE;
                            }
                        LEDS_GPRINT_RAW("Abort transaction for mcp=%lu", mcp_index);

                        /** Abort transaction for this device */
                        for (size_t i=node_offset; i<base_indexof(mcp_index+1); ++i)
                            { wnodes[i].pop_self(); }
                        MCP_DEVICES[mcp_index].status = async_device::state::Failed;
                    }
            }
        }
        delayMicroseconds(500);
    }

    err::errcode errno = err::errcode::OK;
    for (hw::mcp_count_type i=0; i<MCP_COUNT; ++i)
    {
        auto& device = MCP_DEVICES[i];
        if (device.status == async::async_device::state::Failed)
            {
                LEDS_GPRINTF("Bad device for bus=%u addr=%x", device.master.id(), device.address);
                device.status = async::async_device::state::Failed;
                errno = err::errcode::HWERROR;
            }
        else if (auto status=wnodes[base_indexof(i+1)-1].status(); status != async::async_state::Finished)
            {
                LEDS_GPRINTF("Timeout on bus=%u addr=%x\tlast state=%u", device.master.id(), device.address, static_cast<uint8_t>(status));
                device.status = async::async_device::state::Failed;
                errno = err::errcode::TIMEOUT_ERROR;
            }
        else
            {
                LEDS_GPRINTF("Device succesfully initialised on bus=%u addr=%x", device.master.id(), device.address);
                device.status = async::async_device::state::Available;
            }
    }

    clear_all();
    _refresh_period = 1'000'000 / refresh_rate;

    return errno;
}

void hw::LedsDriver::clear_all()
{
    for (uint8_t i=0; i<MULTIPLEX_COLUMS_COUNT; ++i)
    {
        for (uint8_t m=0; m<MCP_COUNT; ++m)
        {
            /**
             * Clear internal buffer
             */
            _mcp_write_buffer[i][m][0] = static_cast<uint8_t>(mcp23017::registers::GPIOA);
            _mcp_write_buffer[i][m][1] = 0; // GPIOA
            _mcp_write_buffer[i][m][2] = 0; // GPIOB
        }
    }

    /* forces the next update() call to write values */
    _elapsed_time = elapsedMicros(_refresh_period);
    _cycle_state = CycleState::READY;
    _current_index = 0;
}

/**
 * 
 */
err::errcode hw::LedsDriver::update()
{
    // if (!_init_ok)
    //     { return err::raise_error(FLEDS, err::errcode::INVALID_CALL, logging::severity::ERROR, "Led driver not initialised"); }

    switch (_cycle_state)
    {
        case CycleState::READY:
            /* Wating for a new cycle */
            if (_elapsed_time < _refresh_period)
                { return err::errcode::OK; }
            if (err::errcode errno = begin_cycle(); errno != err::errcode::OK)
                { return errno; }
            /* else fallthrough UPDATING_GPIOS */

        case CycleState::UPDATING_GPIOS:
            return update_mcps();
            /* No fallthrough here as this async call is supposed to take a lot of time to finish */

        case CycleState::ENDING:
            return end_cycle();
    }
    
    return err::errcode::OK;
}

err::errcode hw::LedsDriver::begin_cycle()
{
    if (_cycle_state != CycleState::READY)
    { 
        LEDS_GPRINTF("INVALID CALL: begin_cycle caled with state=%u", static_cast<uint8_t>(_cycle_state));
        return err::errcode::INVALID_CALL;
    }
    LEDS_GPRINTF("Begin write cycle ... ");

    /* clear state bits */
    // led_err_stack::clear_errors();
    // _updated_mcps = 0x00;

    /* disable all columns and write new address */
    digitalWrite(CATHODE_ENABLE_PIN, LOW);
    digitalWrite(CATHODE_ADDR_PINA0, static_cast<bool>(_current_index & 0x01));
    digitalWrite(CATHODE_ADDR_PINA1, static_cast<bool>(_current_index & 0x02));
    digitalWrite(CATHODE_ADDR_PINA2, static_cast<bool>(_current_index & 0x04));

    static uint8_t tmp_buffer[2] = {0};
    tmp_buffer[0] = static_cast<uint8_t>(mcp23017::registers::GPIOA);
    tmp_buffer[1] = 0xFF;// static_cast<uint8_t>(0x01) << _current_index;

    Master.write_async(mcp23017::MCP_BASE_ADDR+3, tmp_buffer, 2, true);
    // elapsedMillis elapsed;
    // while (elapsed < 10 && !Master.finished())
    //     { /* noting */ }

    /* prepare write calls */
    _targeted_masters = masters_set(nullptr);
    for (mcp_count_type i=0; i<MCP_COUNT; ++i)
    {
        auto& device = MCP_DEVICES[i];
        if (!device)
            { continue; }

        _write_calls[i] = hw::async::async_write{
            .address = device.address,
            .buffer = _mcp_write_buffer[_current_index][i],
            .num_bytes = WRITE_BUFFER_SIZE,
            .send_stop = true
            };
        _write_nodes[i].init(_write_calls +i);

        device.master.push(_write_nodes +i);
        _targeted_masters.add(&device.master);
    }
    SHOW_MASTERS(_targeted_masters);

    /* ready to update GPIOS */
    _cycle_state = CycleState::UPDATING_GPIOS;

    /* update elapsed time at cycle begins to maximise refresh rate precision */
    _elapsed_time = elapsedMillis();

    return err::errcode::OK;
}
err::errcode hw::LedsDriver::update_mcps()
{
    if (_cycle_state != CycleState::UPDATING_GPIOS)
    { 
        LEDS_GPRINTF("INVALID CALL: update_mcps caled with state=%u", static_cast<uint8_t>(_cycle_state));
        return err::errcode::INVALID_CALL;
    }
    LEDS_GPRINTF("Updating drivers ... ");

    bool all_masters_ok = true;
    for (auto master: _targeted_masters.datas)
    {
        if (nullptr == master)
            { continue; }
        LEDS_GPRINT_RAW("\tbus=%u ... ", master->id())

        if (master->next_call() == nullptr)
            { continue; }
        else
            { all_masters_ok = false; }

        I2CError errno = master->update();
        LEDS_GPRINTF("Returned errno: %u ... ", static_cast<uint8_t>(errno));

        if (errno == I2CError::ok)
            { continue; }
        else            
        {
            if (errno == I2CError::master_not_ready)
                { continue; }
            else if (hw::async::i2c_error_is_recoverable(errno))
                { continue; /* nothing to be done, only hopes */ }
            else
                { 
                    LEDS_GPRINT_RAW("Unrecoverable error: ");
                    auto* bad_write = master->next_call();
                    const size_t mcp_index = bad_write - _write_nodes;

                    if (
                        nullptr == bad_write
                        || bad_write < _write_nodes+0
                        || _write_nodes+MCP_COUNT <= bad_write
                        )
                        {
                            LEDS_GPRINT_RAW("ILLEGAL STATE: next_write=%p mcp_index=%lu ... ABORT",
                                bad_write, mcp_index);
                            return err::errcode::INVALID_STATE;
                        }
                    LEDS_GPRINT_RAW("Abort transaction for mcp=%lu", mcp_index);

                    /** Abort transaction for this device */
                    bad_write->pop_self();
                    MCP_DEVICES[mcp_index].status = hw::async::async_device::state::Failed;
                }
        }
    }

    if (all_masters_ok)
        { LEDS_GPRINTF("Cycle done"); _cycle_state = CycleState::ENDING; }
    
    return err::errcode::OK;
}

err::errcode hw::LedsDriver::end_cycle()
{
    if (_cycle_state != CycleState::ENDING)
    { 
        LEDS_GPRINTF("INVALID CALL: end_cycle caled with state=%u", static_cast<uint8_t>(_cycle_state));
        return err::errcode::INVALID_CALL;
    }
    LEDS_GPRINTF("Ending cycle ... ");


    static uint8_t tmp_buffer[2] = {0};
    tmp_buffer[0] = static_cast<uint8_t>(mcp23017::registers::GPIOA);
    tmp_buffer[1] = ~(static_cast<uint8_t>(0x01) << _current_index);

    Master.write_async(mcp23017::MCP_BASE_ADDR+3, tmp_buffer, 2, true);
    elapsedMillis elapsed;
    while (elapsed < 10 && !Master.finished())
        { /* noting */ }

    
    /* enable selected column */
    digitalWrite(hw::CATHODE_ENABLE_PIN, HIGH);
    /* prepare next cycle */
    _current_index = (_current_index+1) % hw::MULTIPLEX_COLUMS_COUNT;
    _cycle_state = CycleState::READY;

    return err::errcode::OK;
}

    // static_assert(MCP_COUNT < (sizeof(uint8_t) * 8));
    // for (uint8_t i=0; i<MCP_COUNT; ++i)
    // {
    //     if (_updated_mcps & (0x01 << i))
    //         { continue; /* MCP already upated */ }

    //     if (!MCP[i].master.finished())
    //     {
    //         auto error = led_error{ .fault_mcp=static_cast<hw::mcp>(i), .errcode=I2CError::ok };
    //         led_err_stack::raise(FLEDS, error, err::errcode::HWERROR, logging::severity::WARNING,
    //             "I2C Master for MCP_%d not finished", i);
    //         continue;
    //     }

    //     if (MCP[i].master.has_error())
    //     {
    //         auto error = led_error{ .fault_mcp=static_cast<hw::mcp>(i), .errcode=MCP[i].master.error() };
    //         led_err_stack::raise(FLEDS, error, err::errcode::HWERROR, logging::severity::ERROR,
    //             "I2C Master for MCP_%u errno:%d", i, error.errcode);
    //         /* no continue here as a new operation must be tried to clear 'has_error' flag */
    //     }

    //     MCP[i].master.write_async(MCP[i].address, _mcp_write_buffer[_current_index][i], WRITE_BUFFER_SIZE, true);
    //     _updated_mcps |= 0x01 << i;
    // }

    // constexpr const uint8_t BITMASK = ((0x01 << MCP_COUNT) - 1);

    // if (static_cast<bool>(_updated_mcps & ~BITMASK))
    //     { err::raise_error(FLEDS, err::errcode::INVALID_STATE, logging::severity::ALERT,
    //             "_updated_mcp=%x has bad bits set, mask=%x", _updated_mcps, BITMASK); }

    // if (_updated_mcps == BITMASK)
    //     { _cycle_state = CycleState::ENDING; }

    // _init_ok = false;

    //     /**
    //      * Asyncronous send IOCON
    //      */
    // elapsedMicros elapsed;
    // Serial.println("Initialising Leds Driver ...");

    // led_err_stack::clear_errors();
    // {
    //     for (uint8_t i=0; i<hw::MCP_COUNT; ++i)
    //     {
    //         // MCP[i].master.set_internal_pullups(InternalPullup::disabled);
    //         // MCP[i].master.begin(bus_speed);

    //     // configure IOCON, do not send stop as we are going to setup IODIRA
    //         MCP[i].master.write_async(MCP[i].address, iocon_buffer, 2, false);
    //     }
    //     Serial.printf("Sent IOCON to all devices, took %lu(us)\n", static_cast<unsigned long>(elapsed));
    //     elapsed = 0;
        
    //     uint8_t mcp_iocon_ok = 0;
    //     elapsedMillis timeout;
    //     while (timeout < timeout_ms)
    //     {
    //         Serial.printf("timeout=%lu(ms)\n", static_cast<unsigned long>(timeout));

    //         for (uint8_t i=0; i<MCP_COUNT; ++i)
    //         {
    //             if (!(mcp_iocon_ok & (1 << i)) && MCP[i].master.finished())
    //             {
    //                 mcp_iocon_ok |= 1 << i;

    //                 if (MCP[i].master.has_error())
    //                 {
    //                     auto error = led_error{ .fault_mcp=static_cast<hw::mcp>(i), .errcode=MCP[i].master.error() };
    //                     return led_err_stack::raise(FLEDS, error, err::errcode::HWERROR, logging::severity::EMERGENCY,
    //                         "Failed write IOCON for MCP_%u, errno:%u", i, static_cast<uint8_t>(error.errcode));
    //                 }
    //                 const size_t bytes_transfered = MCP[i].master.get_bytes_transferred();
    //                 if (bytes_transfered != IOCON_SIZE)
    //                 {
    //                     auto error = led_error{ .fault_mcp=static_cast<hw::mcp>(i), .errcode=I2CError::ok };
    //                     return led_err_stack::raise(FLEDS, error, err::errcode::GENERIC_ERROR, logging::severity::EMERGENCY,
    //                         "Transmitted %u bytes but expected %u", bytes_transfered, IOCON_SIZE);
    //                 }

    //     // configure IODIRs when IOCON is OK
    //                 MCP[i].master.write_async(MCP[i].address, iodir_buffer, 3, true);
    //             }

    //         }
    //     }
    //     Serial.printf("Sent IODIR to all devices, took %lu(us)\n", static_cast<unsigned long>(elapsed));
    //     elapsed = 0;

    //     if (mcp_iocon_ok != (1 << MCP_COUNT) - 1)
    //     {
    //         return err::raise_error(FLEDS, err::errcode::TIMEOUT_ERROR, logging::severity::EMERGENCY,
    //                 "Failed initialise MCP: Timed out, mcp_iocon_ok:%u", mcp_iocon_ok);
    //     }
    // }
    //     /**
    //      * Asynchronous send IODIRA and IODIRB (set pins for output)
    //      */
    // {
    //     uint8_t mcp_iodir_ok = 0;
    //     elapsedMillis timeout;
    //     while (timeout < timeout_ms)
    //     {
    //         for (uint8_t i=0; i<MCP_COUNT; ++i)
    //         {
    //             if (!(mcp_iodir_ok & (1 << i)) && MCP[i].master.finished())
    //             {
    //                 mcp_iodir_ok |= 1 << i;

    //                 if (MCP[i].master.has_error())
    //                 {
    //                     auto error = led_error{ .fault_mcp=static_cast<hw::mcp>(i), .errcode=MCP[i].master.error() };
    //                     return led_err_stack::raise(FLEDS, error, err::errcode::HWERROR, logging::severity::EMERGENCY,
    //                             "Failed write IODIR for MCP_%u, errno:%u", i, static_cast<uint8_t>(error.errcode));
    //                 }
    //                 const size_t bytes_transfered = MCP[i].master.get_bytes_transferred();
    //                 if (bytes_transfered != IODIR_SIZE)
    //                 {
    //                     auto error = led_error{ .fault_mcp=static_cast<hw::mcp>(i), .errcode=I2CError::ok };
    //                     return led_err_stack::raise(FLEDS, error, err::errcode::GENERIC_ERROR, logging::severity::EMERGENCY,
    //                         "Transmitted %u bytes but expected %u", bytes_transfered, IODIR_SIZE);
    //                 }
    //             }
    //         }
    //     }
    // }

    // using i2c_task = async::task<I2CError, mcp23017::device*, bool>;
    // auto task = i2c_task{std::make_tuple(MCP_DEVICES, false), mcp23017::async_setup_iocon, async::dummy_finished, 100};

    // auto task = async::task{
    //     [](bool send_stop) -> auto { return MCP_DEVICES[0].async_setup_iocon(send_stop); }, 
    //     []() -> auto { return MCP_DEVICES[0].master.finished(); },
    //     100
    //     };

//     for (auto& device: MCP_DEVICES)
//     {

//     }

//         /**
//          * Clear internal buffer
//          */
//     _init_ok = true;
//     _current_index = 0;
//     for (uint8_t i=0; i<MULTIPLEX_COLUMS_COUNT; ++i)
//     {
//         for (uint8_t m=0; m<MCP_COUNT; ++m)
//         {
//             _mcp_write_buffer[i][m][0] = static_cast<uint8_t>(mcp23017::registers::GPIOA);
//             _mcp_write_buffer[i][m][1] = 0; // GPIOA
//             _mcp_write_buffer[i][m][2] = 0; // GPIOB
//         }
//     }

//     _refresh_rate = refresh_rate;
//     _elapsed_time = elapsedMillis(_refresh_rate);
//     // forces the next update() call to write values

//     // for (uint8_t i=0; i<MCP_COUNT; ++i)
//     //     { _elapsed_time[i] = elapsedMillis(_refresh_rate); }

//     return err::errcode::OK;
// }

// /**
//  * Set state of an ON-OFF led, change takes effect in few update calls
//  */
// err::errcode write(const ios::monochrome_pad& pad, const bool state);
// err::errcode toggle(const ios::monochrome_pad& pad);
// err::result<bool> read(const ios::monochrome_pad& pad) const;

// /**
//  * Set state of bicolor led
//  */
// err::errcode write(const ios::bicolor_pad& pad, const color color);
// err::errcode toggle(const ios::bicolor_pad& pad);
// err::result<color> read(const ios::bicolor_pad& pad) const;

// /**
//  * Set state of a ledring
//  */
// err::errcode write(const ios::ring_encoder& encoder, const uint16_t state);
// err::errcode toggle(const ios::ring_encoder& encoder);
// err::result<uint16_t> read(const ios::ring_encoder& encoder) const;
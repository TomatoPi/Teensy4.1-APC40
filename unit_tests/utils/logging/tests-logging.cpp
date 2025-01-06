
#include "utils/logging/logging.h"
#include "utils/logging/headers.hxx"
#include "utils/logging/abc.hxx"

#include <array>
#include <cstddef>
#include <iostream>
#include <cassert>

#include <chrono>
#include <ctime>

using namespace logging;
using namespace error;

struct mprintf
{
    template <typename ...Args>
    status_byte operator() (const char* fmt, Args... args) const
    {
        int err = printf(fmt, std::forward<Args>(args)...);
        if (err < 0)
            { return errcode::GENERIC_ERROR | severity::ERROR; }
        else
            { return status_byte{}; }
    }

    status_byte flush() const
        { return status_byte{}; }
};

using mlogger = Logger<mprintf, severity_filter>;

struct vmprintf: logging2::AbstractTransport<>
{
    int transmit_impl(const char* fmt, va_list args) override
        {
            vprintf(fmt, args);
            return 0; // status_byte{};
        }
    int flush() override
        {
            std::flush(std::cout);
            return 0; // status_byte{};
        }
    explicit operator bool() const override
        { return true; }
};

template <>
struct logging2::typeof_timepoint<logging2::DefaultContext>
{
    struct type
    {
        using time_point = std::time_t;

        static time_point now()
            { return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); }
        
        template <typename T>
        static auto hours(T&& t)
            { return std::chrono::hours(std::forward<T>(t)).count(); }
    };
};

int main(int argc, char* const argv[])
{

    for (size_t i=0; i<1; ++i)
    {
    auto now = std::chrono::system_clock::time_point{};
    auto time = now.time_since_epoch();

    raw_header header{ "Logging", errcode::OK | severity::DEBUG, static_cast<unsigned long int>(time.count()) };

    header(mprintf{});
    std::cout << std::endl;

    mprintf printer;
    header(printer);
    std::cout << std::endl;

    severity_filter filter{severity::DEBUG};
    mlogger logger{printer, filter};

    logger(header, "Hello Logging ! argc=%d", argc);
    std::cout << std::endl;

    logger.filter.log_level = severity::NOTICE;
    logger(header, "Discaded message");
    std::cout << "/* 'Discarded message' not shown */" << std::endl;

    header.errbyte = errcode::OK | severity::WARNING;
    logger(header, "Not a discarded message");
    std::cout << std::endl;

    mlogger errLogger{printer, severity::ERROR};

    auto doublelog = tie_logs(logger, errLogger);
    assert(doublelog.is_logging_enabled_for(header));
    doublelog(header, "This message only shows once");
    std::cout << std::endl;

    header.errbyte = errcode::HWERROR | severity::EMERGENCY;
    assert(doublelog.is_logging_enabled_for(header));
    doublelog(header, "This message is shown twice\n");

    header.errbyte = errcode::HWERROR | severity::DEBUG;
    assert(!doublelog.is_logging_enabled_for(header));
    doublelog(header, "This message is not shown");
    std::cout << "/* discarded twice 'This message is not shown' */" << std::endl;
    std::cout << std::endl;
    }

    vmprintf vprinter;
    vprinter("Hello VLogging ! argc%%d=%d\n", argc);
    vprinter("Hello VLogging ! argc%%u=%u\n", argc);
    /* vprinter("Hello VLogging ! argc%%s=%s\n", argc); // won't compile ! %s incompatible with int */
    vprinter.flush();

    logging2::messages::timestamp<> t;
    t.dump(vprinter);

    return EXIT_SUCCESS;
}

#include "utils/logging/logging.h"

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

int main(int argc, char* const argv[])
{
    auto now = std::chrono::system_clock::now();
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
    doublelog(header, "This message only shows once");

    return EXIT_SUCCESS;
}
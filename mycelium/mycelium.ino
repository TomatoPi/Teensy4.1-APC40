/**
 * 
 */

#include "src/utils/logging/logging.h"
#include "Arduino.h"

using namespace logging;
using namespace error;

void setup()
{
    /* %0: load configuration */
    /* %1: configure hardware */

    raw_header header{ "Logging", errcode::OK | severity::DEBUG, static_cast<unsigned long int>(millis()) };
}

void loop()
{

}
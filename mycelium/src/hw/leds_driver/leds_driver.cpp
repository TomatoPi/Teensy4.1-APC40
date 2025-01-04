/**
 * 
 */

#include "leds_driver.hxx"

namespace hw
{
namespace leds_driver
{

auto DriverDefaultSettings::RefreshRate = 50;

#ifdef NDEBUG
auto DriverDefaultSettings::LogLevel = error::severity::INFO;
#else
auto DriverDefaultSettings::LogLevel = error::severity::DEBUG;
#endif

} /* endof namespace leds_driver */
} /* endof namespace hw */
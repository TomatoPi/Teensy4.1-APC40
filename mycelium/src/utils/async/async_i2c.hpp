/**
 * 
 */

#ifndef DEF_ASYNC_I2C_HPP
#define DEF_ASYNC_I2C_HPP

#include <cstdint>

namespace async
{
namespace io
{

template <typename Transport>
class WriteCall
{
public:

    template <size_t N>
    WriteCall(const uint8_t datas[N], uint8_t address, bool send_stop)
        : _buffer{datas}, _num_bytes{N}, _address{address}, _send_stop{send_stop}
        {}

private:
    const uint8_t* _buffer;
    size_t      _num_bytes;
    uint8_t       _address;
    bool        _send_stop;
};

} /* endof namespace i2c */
} /* endof namespace async */

#endif /* DEF_ASYNC_I2C_HPP */
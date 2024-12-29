/**
 * 
 */

#include "logging.hpp"

#include <usb_serial.h>
#include <SD.h>

#define SERIAL_AVAILABLE    static_cast<bool>(Serial)

bool logging::init_logger()
{
    /**
     * Init memory
     */
    memset(_msg_buffer, '\0', LOGGING_BUFFER_SIZE);
    _msg_len = 0;

    /**
     * Creates logs directory
     */
    if (!SD.exists(LOG_FOLDER))
    {
        if (!SD.mkdir(LOG_FOLDER))
        {
            if (SERIAL_AVAILABLE)
                { Serial.println("@init_logger: EMERGENCY: Failed create logging folder"); }
            return false;
        }
    }

    /**
     * Open log file and close it immediatly
     */
    File f = SD.open(LOG_FILE, FILE_WRITE);
    if (!f)
    {
        if (SERIAL_AVAILABLE)
            { Serial.println("@init_logger: EMERGENCY: Can't open logging file"); }
        return false;
    }
    f.close();

    return true;
}

bool logging::flush_to_logfile()
{
    elapsedMicros elapsed;

    if (LOGGING_BUFFER_SIZE < _msg_len)
    {
        if (SERIAL_AVAILABLE)
            { Serial.printf("@flush_to_logfile: INVALID STATE: msg_len=%d bigger than BUFFSIZE=%d\n", _msg_len, LOGGING_BUFFER_SIZE); }
        clear_buffer();
        return false;
    }
    
    File f = SD.open(LOG_FILE, FILE_WRITE);
    if (!f)
    {
        if (SERIAL_AVAILABLE)
            { Serial.println("@flush_to_logfile: EMERGENCY: Can't open logging file"); }
        return false;
    }

    size_t bytes_written = f.write(_msg_buffer, _msg_len);
    if (bytes_written != _msg_len)
    {
        if (SERIAL_AVAILABLE)
            { Serial.printf("@flush_to_logfile: BAD WRITE: bytes_written=%d msg_len=%d", bytes_written, _msg_len); }
        f.close();
        return false;
    }
    f.close();

    if (SERIAL_AVAILABLE && is_logging_enabled_for(severity::DEBUG))
        { Serial.printf("@flush_to_logfile: SUCCESS: bytes_written=%d, time_taken=%lu(us)", bytes_written, static_cast<unsigned long>(elapsed)); }

    return true;
}
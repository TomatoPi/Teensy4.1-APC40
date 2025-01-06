/**
 * 
 */

#include <Arduino.h>
#include <usb_serial.h>

#include "defines.h"

void setup()
{
    /* %0: load configuration */
    /* %1: configure hardware */
    Serial.begin(9600);
    while (!Serial) { /* nothing */ }
    Serial.println(BUILD_DATE);
    Serial.println(BUILD_ID);
}

void loop()
{

}
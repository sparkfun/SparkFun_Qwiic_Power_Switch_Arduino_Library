/*
  This is a library written for the SparkX Qwiic Power Switch.
  Written by: Paul Clark (PaulZC)
  Date: April 23rd, 2020

  It is based extensively on the SparkFun_PCA9536_Arduino_Library:
  Written by Jim Lindblom @ SparkFun Electronics, May 4th, 2018

  The power switch is based on the PCA9536 4-bit I/O digital expander,
  which communicates via an I2C bus. The expander can read or write four
  separate I/O pins. For the Qwiic Power Switch:
  GPIO0 switches the Qwiic power via a P-FET. If GPIO0 is high, power is enabled.
  GPIO1 and GPIO2 are general purpose pins which can be used for input or output.
  GPIO3 can isolate the I2C bus via a PCA9306. If GPIO3 is low, the bus is isolated.

  MIT Licence
*/

#ifndef QWIIC_POWER_SWITCH_LIBRARY_H
#define QWIIC_POWER_SWITCH_LIBRARY_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DEBUG_PCA9536

#include <Wire.h>

//  Valid PCA9536 addresses
typedef enum {
    PCA9536_ADDRESS = 0x41,
    PCA9536_ADDRESS_INVALID = 0xFF
} PCA9536_Address_t;

// PCA9536 registers:
typedef enum {
    PCA9536_REGISTER_INPUT_PORT         = 0x00,
    PCA9536_REGISTER_OUTPUT_PORT        = 0x01,
    PCA9536_REGISTER_POLARITY_INVERSION = 0x02,
    PCA9536_REGISTER_CONFIGURATION      = 0x03,
    PCA9536_REGISTER_INVALID
} PCA9536_REGISTER_t;

// PCA9536 error code returns:
typedef enum {
    PCA9536_ERROR_READ            = -4,
    PCA9536_ERROR_WRITE           = -3,
    PCA9536_ERROR_INVALID_ADDRESS = -2,
    PCA9536_ERROR_UNDEFINED       = -1,
    PCA9536_ERROR_SUCCESS         = 1
} PCA9536_error_t;
const PCA9536_error_t PCA9536_SUCCESS = PCA9536_ERROR_SUCCESS;

// PCA9536 invert/normal values:
typedef enum {
    PCA9536_RETAIN,
    PCA9536_INVERT,
    PCA9536_INVERT_END
} PCA9536_invert_t;

class QWIIC_POWER {
public:
    QWIIC_POWER();

    // Perform initial configuration. Has to be called once.
    boolean begin(TwoWire &wirePort = Wire);

    // Check whether PCA9536 device is connected
    boolean isConnected(void);

    // enable Qwiic power
    PCA9536_error_t powerOn();
    // disable Qwiic power
    PCA9536_error_t powerOff();
    // switch Qwiic power
    PCA9536_error_t switchPower(uint8_t value);

    // enable I2C isolation = I2C bus _is_ isolated
    PCA9536_error_t isolationOn();
    // disable I2C isolation = I2C bus _is not_ isolated
    PCA9536_error_t isolationOff();
    // switch I2C isolation: 0 = I2C is not isolated; 1 = I2C is isolated.
    PCA9536_error_t switchIsolation(uint8_t value);

    // setDebugStream to enable library debug statements
    void setDebugStream(Stream &debugPort = Serial);

    // pinMode can set a pin (0-3) to INPUT or OUTPUT
    PCA9536_error_t pinMode(uint8_t pin, uint8_t mode);

    // digitalWrite and write can be used to set a pin HIGH or LOW
    PCA9536_error_t digitalWrite(uint8_t pin, uint8_t value);
    PCA9536_error_t write(uint8_t pin, uint8_t value);

    // readReg can be used to read the whole input register (4 bits)
    uint8_t readReg();

    // digitalRead and read can be used to read a pin (0-3)
    uint8_t digitalRead(uint8_t pin);
    uint8_t read(uint8_t pin);

    // invert and revert can be used to invert (or not) the I/O logic during a read
    PCA9536_error_t invert(uint8_t pin, PCA9536_invert_t inversion = PCA9536_INVERT);
    PCA9536_error_t revert(uint8_t pin);

private:
    TwoWire *_i2cPort; //The generic connection to user's chosen I2C hardware
    Stream * _debugPort;
    PCA9536_Address_t _deviceAddress;

// I2C Read/Write
    PCA9536_error_t readI2CBuffer(uint8_t * dest, PCA9536_REGISTER_t startRegister, uint16_t len);
    PCA9536_error_t writeI2CBuffer(uint8_t * src, PCA9536_REGISTER_t startRegister, uint16_t len);
    PCA9536_error_t readI2CRegister(uint8_t * dest, PCA9536_REGISTER_t registerAddress);
    PCA9536_error_t writeI2CRegister(uint8_t data, PCA9536_REGISTER_t registerAddress);
};

#endif

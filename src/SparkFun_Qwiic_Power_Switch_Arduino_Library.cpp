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

#include <SparkFun_Qwiic_Power_Switch_Arduino_Library.h>

#ifdef DEBUG_PCA9536
#define PCA9536_DEBUG(x) if (_debugPort != NULL) { _debugPort->print(x);}
#define PCA9536_DEBUGLN(x) if (_debugPort != NULL) { _debugPort->println(x);}
#define STORAGE(x) (x)
#else
#define PCA9536_DEBUG(x)
#define PCA9536_DEBUGLN(x)
#define STORAGE(x) (x)
#endif

QWIIC_POWER::QWIIC_POWER()
{
  _i2cPort = NULL;
  _debugPort = NULL;
  _deviceAddress = PCA9536_ADDRESS_INVALID;
}

boolean QWIIC_POWER::begin(TwoWire &wirePort)
{
  _i2cPort = &wirePort; //Grab which port the user wants us to use
  _deviceAddress = PCA9536_ADDRESS;

  //Check connection
  if (isConnected() == false)
    return (false);

  // Enable Qwiic Power
  pinMode(0, OUTPUT); // Make GPIO0 an output
  write(0, HIGH); // Set the output high

  // Enable I2C
  pinMode(3, OUTPUT); // Make GPIO3 an output
  write(3, HIGH); // Set the output high

  return (true);
}

//Returns true if I2C device ack's
boolean QWIIC_POWER::isConnected()
{
  _i2cPort->beginTransmission((uint8_t)_deviceAddress);
  return ((_i2cPort->endTransmission()) == 0);
}

PCA9536_error_t QWIIC_POWER::powerOn()
{
  // Enable Qwiic Power
  pinMode(0, OUTPUT); // Make GPIO0 an output
  write(0, HIGH); // Set the output high

  // Enable I2C
  pinMode(3, OUTPUT); // Make GPIO3 an output
  write(3, HIGH); // Set the output high

  return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t QWIIC_POWER::powerOff()
{
  // Disable Qwiic Power
  pinMode(0, OUTPUT); // Make GPIO0 an output
  write(0, LOW); // Set the output low

  // Disable I2C
  pinMode(3, OUTPUT); // Make GPIO3 an output
  write(3, LOW); // Set the output low

  return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t QWIIC_POWER::switchPower(uint8_t value)
{
  pinMode(0, OUTPUT); // Make GPIO0 an output
  write(0, value); // Set the output

  pinMode(3, OUTPUT); // Make GPIO3 an output
  write(3, value); // Set the output

  return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t QWIIC_POWER::isolationOn()
{
  // Enable I2C isolation = I2C bus _is_ isolated
  pinMode(3, OUTPUT); // Make GPIO3 an output
  write(3, LOW); // Set the output low

  return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t QWIIC_POWER::isolationOff()
{
  // Disable I2C isolation = I2C bus _is not_ isolated
  pinMode(3, OUTPUT); // Make GPIO3 an output
  write(3, HIGH); // Set the output high

  return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t QWIIC_POWER::switchIsolation(uint8_t value)
{
  pinMode(3, OUTPUT); // Make GPIO3 an output
  if (value == 0) // If value is 0
  {
    write(3, HIGH); // Set the output high to disable isolation
  }
  else
  {
    write(3, LOW); // Set the output low to enable isolation
  }

  return PCA9536_ERROR_SUCCESS;
}

void QWIIC_POWER::setDebugStream(Stream & debugPort)
{
  _debugPort = &debugPort;
}

PCA9536_error_t QWIIC_POWER::pinMode(uint8_t pin, uint8_t mode)
{
  PCA9536_error_t err;
  uint8_t cfgRegister = 0;

  if (pin > 4) return PCA9536_ERROR_UNDEFINED;

  err = readI2CRegister(&cfgRegister, PCA9536_REGISTER_CONFIGURATION);
  if (err != PCA9536_ERROR_SUCCESS)
  {
    return err;
  }
  cfgRegister &= ~(1<<pin); // Clear pin bit
  if (mode == INPUT) // Set the bit if it's being set to INPUT (opposite of Arduino)
  {
    cfgRegister |= (1<<pin);
  }
  return writeI2CRegister(cfgRegister, PCA9536_REGISTER_CONFIGURATION);
}

PCA9536_error_t QWIIC_POWER::write(uint8_t pin, uint8_t value)
{
  PCA9536_error_t err;
  uint8_t outputRegister = 0;

  if (pin > 4) return PCA9536_ERROR_UNDEFINED;

  err = readI2CRegister(&outputRegister, PCA9536_REGISTER_OUTPUT_PORT);
  if (err != PCA9536_ERROR_SUCCESS)
  {
    return err;
  }
  // TODO: Break out of here if it's already set correctly
  outputRegister &= ~(1<<pin); // Clear pin bit
  if (value == HIGH) // Set the bit if it's being set to HIGH (opposite of Arduino)
  {
    outputRegister |= (1<<pin);
  }
  return writeI2CRegister(outputRegister, PCA9536_REGISTER_OUTPUT_PORT);
}

PCA9536_error_t QWIIC_POWER::digitalWrite(uint8_t pin, uint8_t value)
{
  return write(pin, value);
}

uint8_t QWIIC_POWER::readReg()
{
  PCA9536_error_t err;
  uint8_t inputRegister = 0;

  err = readI2CRegister(&inputRegister, PCA9536_REGISTER_INPUT_PORT);
  if (err != PCA9536_ERROR_SUCCESS)
  {
    return err;
  }
  return (inputRegister & (0x0f));
}

uint8_t QWIIC_POWER::read(uint8_t pin)
{
  PCA9536_error_t err;
  uint8_t inputRegister = 0;

  if (pin > 4) return PCA9536_ERROR_UNDEFINED;

  err = readI2CRegister(&inputRegister, PCA9536_REGISTER_INPUT_PORT);
  if (err != PCA9536_ERROR_SUCCESS)
  {
    return err;
  }
  return (inputRegister & (1<<pin)) >> pin;
}

uint8_t QWIIC_POWER::digitalRead(uint8_t pin)
{
  return read(pin);
}

PCA9536_error_t QWIIC_POWER::invert(uint8_t pin, PCA9536_invert_t inversion)
{
  PCA9536_error_t err;
  uint8_t invertRegister = 0;

  if (pin > 4) return PCA9536_ERROR_UNDEFINED;

  err = readI2CRegister(&invertRegister, PCA9536_REGISTER_POLARITY_INVERSION);
  if (err != PCA9536_ERROR_SUCCESS)
  {
    return err;
  }
  // TODO: Break out of here if it's already set correctly
  invertRegister &= ~(1<<pin); // Clear pin bit
  if (inversion == PCA9536_INVERT) // Set the bit if it's being set to inverted
  {
    invertRegister |= (1<<pin);
  }
  return writeI2CRegister(invertRegister, PCA9536_REGISTER_POLARITY_INVERSION);
}

PCA9536_error_t QWIIC_POWER::revert(uint8_t pin)
{
  return invert(pin, PCA9536_RETAIN);
}

PCA9536_error_t QWIIC_POWER::readI2CBuffer(uint8_t * dest, PCA9536_REGISTER_t startRegister, uint16_t len)
{
  PCA9536_DEBUGLN((STORAGE("(readI2CBuffer): read ") + String(len) +
                     STORAGE(" @ 0x") + String(startRegister, HEX)));
  if (_deviceAddress == PCA9536_ADDRESS_INVALID)
  {
    PCA9536_DEBUGLN(STORAGE("    ERR (readI2CBuffer): Invalid address"));
    return PCA9536_ERROR_INVALID_ADDRESS;
  }
  _i2cPort->beginTransmission((uint8_t) _deviceAddress);
  _i2cPort->write(startRegister);
  if (_i2cPort->endTransmission(false) != 0)
  {
    PCA9536_DEBUGLN(STORAGE("    ERR (readI2CBuffer): End transmission"));
    return PCA9536_ERROR_READ;
  }

  _i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)len);
  for (int i = 0; i < len; i++)
  {
    dest[i] = _i2cPort->read();
    PCA9536_DEBUGLN((STORAGE("    ") + String(i) + STORAGE(": 0x") + String(dest[i], HEX)));
  }

  return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t QWIIC_POWER::writeI2CBuffer(uint8_t * src, PCA9536_REGISTER_t startRegister, uint16_t len)
{
  if (_deviceAddress == PCA9536_ADDRESS_INVALID)
  {
    PCA9536_DEBUGLN(STORAGE("ERR (readI2CBuffer): Invalid address"));
    return PCA9536_ERROR_INVALID_ADDRESS;
  }
  _i2cPort->beginTransmission((uint8_t) _deviceAddress);
  _i2cPort->write(startRegister);
  for (int i = 0; i < len; i++)
  {
    _i2cPort->write(src[i]);
  }
  if (_i2cPort->endTransmission(true) != 0)
  {
    return PCA9536_ERROR_WRITE;
  }
  return PCA9536_ERROR_SUCCESS;
}

PCA9536_error_t QWIIC_POWER::readI2CRegister(uint8_t * dest, PCA9536_REGISTER_t registerAddress)
{
  return readI2CBuffer(dest, registerAddress, 1);
}

PCA9536_error_t QWIIC_POWER::writeI2CRegister(uint8_t data, PCA9536_REGISTER_t registerAddress)
{
  return writeI2CBuffer(&data, registerAddress, 1);
}

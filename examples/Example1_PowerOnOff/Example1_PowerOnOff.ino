/*
  Using the Qwiic Power Switch
  By: Paul Clark (PaulZC)
  Date: April 23rd, 2020

  Based extensively on:
  Using the PCA9536 -- Digital Output
  By: Jim Lindblom
  SparkFun Electronics
  Date: May 4, 2018

  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example demonstrates how to use the Qwiic Power Switch and read the additional GPIO pins

  Hardware Connections:
  Attach your Arduino to the Qwiic Power Switch IN
  Plug your Qwiic device into the Qwiic Power Switch OUT
  https://www.sparkfun.com/products/15081
*/

#include <Wire.h>

#include <SparkFun_Qwiic_Power_Switch_Arduino_Library.h> // Click here to get the library: http://librarymanager/All#SparkFun_Qwiic_Power_Switch

QWIIC_POWER mySwitch;

void setup(void)
{
  Serial.begin(115200);
  while (!Serial)
    ; //Wait for user to open terminal
  Serial.println(F("Qwiic Power Switch Example"));

  Wire.begin();

  if (mySwitch.begin() == false) //Connect to the power switch using Wire port
  {
    Serial.println(F("Qwiic Power Switch not detected at default I2C address. Please check wiring. Freezing."));
    while (1)
      ;
  }

  // Configure GPIO1 and GPIO2 as INPUT
  mySwitch.pinMode(1, INPUT);
  mySwitch.pinMode(2, INPUT);

  Serial.println(F("1) Enable power and I2C"));
  Serial.println(F("2) Disable power and I2C"));
  Serial.println(F("3) Enable I2C isolation"));
  Serial.println(F("4) Disable I2C isolation"));
}

void loop()
{
  if (Serial.available())
  {
    byte incoming = Serial.read();

    if (incoming == '1')
    {
      // Switch the power on
      mySwitch.powerOn();
      Serial.println(F("Power is ON. I2C isolation is disabled."));
    }
    else if (incoming == '2')
    {
      // Switch the power off
      mySwitch.powerOff();
      Serial.println(F("Power is OFF. I2C isolation is enabled."));
    }
    else if (incoming == '3')
    {
      // Enable I2C isolation = I2C bus _is_ isolated
      mySwitch.isolationOn();
      Serial.println(F("I2C isolation enabled. I2C is isolated."));
    }
    else if (incoming == '4')
    {
      // Disable I2C isolation = I2C bus _is not_ isolated
      mySwitch.isolationOff();
      Serial.println(F("I2C isolation disabled. I2C is not isolated."));
    }

    // Read and print the GPIO1/GPIO2 state
    Serial.print(F("GPIO1 is: "));
    Serial.println(mySwitch.digitalRead(1));
    Serial.print(F("GPIO2 is: "));
    Serial.println(mySwitch.digitalRead(2));

    // Read any extra Serial bytes (e.g. CR or LF)
    while (Serial.available() > 0)
    {
      Serial.read();
    }
  }
}

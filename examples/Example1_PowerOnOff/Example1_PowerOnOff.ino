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
  Attach the Qwiic Power Switch IN to your Arduino
  Plug your Qwiic device into the Qwiic Power Switch OUT
*/

#include <Wire.h>

#include <Qwiic_Power_Switch_Library.h>

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

  Serial.println(F("1) Enable power"));
  Serial.println(F("2) Disable power"));
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
      Serial.println(F("Power is ON."));
    }
    else if (incoming == '2')
    {
      // Switch the power off
      mySwitch.powerOff();
      Serial.println(F("Power is OFF."));
    }

    // Read and print the GPIO1/GPIO2 state
    Serial.print(F("GPIO1 is: "));
    Serial.println(mySwitch.digitalRead(1));
    Serial.print(F("GPIO2 is: "));
    Serial.println(mySwitch.digitalRead(2));
  }
}
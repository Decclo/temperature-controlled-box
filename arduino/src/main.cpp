/*
 * Heat Regulated Electric Oven 
 * 
 * This code works as a controller for box that is to keep its internal temperature stable.
 * 
 * Author:        Hans Rasmussen
 * E-Mail:        angdeclo@gmail.com
 * Creation Date: 09/10/2020
 * Last Modified: 10/10/2020
 */



// ##########################################
// ##              Includes                ##
// ##########################################
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>



// ##########################################
// ##               Settings               ##
// ##########################################
/*
 * This section defines the settings for the program, such as pin numbers and operation modi.
*/

// Should the code be compiled with additional outputs? 0=off, 1=normal
#define DEBUGMODE 0

// Pin for onewire interface. The sensors are installed in series running without parasitic power.
#define ONE_WIRE_BUS 10
#define TEMPERATURE_PRECISION 12

// Pin on which to run the PWM for the motor controller in charge of the fan. The H-bridge is hard-wired.
#define FAN_PWM 11

// Pin controlling the relay in charge of the heating element.
#define HEAT_RELAY_PIN 8

// ##########################################
// ##         Objects and functions        ##
// ##########################################

// Onewire initialization and sensor setup
OneWire oneWire(10);  // on pin 10 (a 4.7K resistor is necessary)
DallasTemperature sensors(&oneWire);

// Addresses for the sensors
DeviceAddress sensor1 = {0x28, 0x69, 0xA1, 0x69, 0x35, 0x19, 0x01, 0x5D};
DeviceAddress sensor2 = {0x28, 0xD5, 0x55, 0x6B, 0x35, 0x19, 0x01, 0x99};


// Function to read out onewire addresses 
void printAddress(DeviceAddress deviceAddress);

// Function to print the temperature for a onewire device
void printTemperature(DeviceAddress deviceAddress);

// Function to print the address and temperature for a onewire device
void printData(DeviceAddress deviceAddress);


// ##########################################
// ##                Setup                 ##
// ##########################################
/*
 * The setup function takes care of initialization, and runs only once.
*/
void setup()
{
  // Initializations //

  // Serial
  Serial.begin(9600);
  Serial.print("Serial initialized!\n\n");

  // OneWire sensors
  Serial.print("Initializing OneWire sensors...\n");
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

/*
  if (!sensors.getAddress(sensor1, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(sensor2, 1)) Serial.println("Unable to find address for Device 1");
*/
  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(sensor1);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(sensor2);
  Serial.println();

  // set the resolution to 9 bit per device
  sensors.setResolution(sensor1, TEMPERATURE_PRECISION);
  sensors.setResolution(sensor2, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(sensor1), DEC);
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(sensor2), DEC);
  Serial.println();
  Serial.print("OneWire sensors initialized!\n\n");
}



// ##########################################
// ##                 Main                 ##
// ##########################################
/*
 * The loop function subsitudes the while(1){} function.
*/
void loop()
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  printData(sensor1);
  printData(sensor2);
  delay(1000);
}



// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    Serial.print(" ");
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}
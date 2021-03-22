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
#include "ArduinoJson.h"



// ##########################################
// ##               Settings               ##
// ##########################################
/*
 * This section defines the settings for the program, such as pin numbers and operation modi.
*/

// Pin for onewire interface. The sensors are installed in series running without parasitic power.
#define ONE_WIRE_BUS 10
#define TEMPERATURE_PRECISION 12

// Pin on which to run the PWM for the motor controller in charge of the fan. The H-bridge is hard-wired.
#define FAN_PWM 9

// Pin controlling the relay in charge of the heating element.
#define HEAT_RELAY_PIN 11

// PID values
#define PID_KP 1;
#define PID_KI 0.2;
#define PID_KD 0.9;

// PID analogue to digital period time (ms)
#define PID_PERIOD 5000;


// ##########################################
// ##   Objects and Function Declaraions   ##
// ##########################################

// Onewire initialization and sensor setup
OneWire oneWire(10);  // on pin 10 (a 4.7K resistor is necessary)
DallasTemperature sensors(&oneWire);

// Addresses for the sensors
DeviceAddress sensor0 = {0x28, 0x69, 0xA1, 0x69, 0x35, 0x19, 0x01, 0x5D};
DeviceAddress sensor1 = {0x28, 0xD5, 0x55, 0x6B, 0x35, 0x19, 0x01, 0x99};
DeviceAddress sensor2 = {0x28, 0x96, 0x5F, 0xE3, 0x22, 0x20, 0x01, 0x62};

// Function to update the PID controller
double computePID(double input);

// Function that takes over the main loop and gives full control of the device.
void debugMode();

// Function to read out onewire addresses 
void printAddress(DeviceAddress deviceAddress);

// Function to print the temperature for a onewire device
void printTemperature(DeviceAddress deviceAddress);

// Function to print the address and temperature for a onewire device
void printData(DeviceAddress deviceAddress);

// Function which searches the OneWire bus and prints results
uint8_t findDevices(int pin);



// ##########################################
// ##           Global Variables           ##
// ##########################################
/*
 * This section contains the global variables. It would be a good idea to change these global variables to local ones in the future, but they're here now.
*/
uint32_t tick_counter = 0;
bool relay_state = false;

//PID constants
unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double input, output, setPoint;
double cumError, rateError;

// PID analogue to digital output
unsigned long LastPeriod = 0;
const int period = PID_PERIOD;
int dutyCycle = 0;



// ##########################################
// ##                Setup                 ##
// ##########################################
void setup()
{
  // Initializations //

  delay(5000);

  // ==== Serial ====
  Serial.begin(9600);
  Serial.print("Serial initialized!\n\n");


  // ==== OneWire sensors ====
  Serial.print("Initializing OneWire sensors...\n");
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasitic power is: ");
  if (sensors.isParasitePowerMode())
  {
    Serial.println("ON");
  }
  else 
  {
    Serial.println("OFF");
  }

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(sensor0);
  Serial.println();
  Serial.print("Device 0 Description: Inside, low\n");

  Serial.print("Device 1 Address: ");
  printAddress(sensor1);
  Serial.println();
  Serial.print("Device 1 Description: Inside, high\n");

  Serial.print("Device 2 Address: ");
  printAddress(sensor2);
  Serial.println();
  Serial.print("Device 1 Description: Outside, reference\n");

  // set the resolution to 9 bit per device
  sensors.setResolution(sensor0, TEMPERATURE_PRECISION);
  sensors.setResolution(sensor1, TEMPERATURE_PRECISION);
  sensors.setResolution(sensor2, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(sensor0), DEC);
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(sensor1), DEC);
  Serial.println();

  Serial.print("Device 2 Resolution: ");
  Serial.print(sensors.getResolution(sensor2), DEC);
  Serial.println();
  Serial.println("OneWire sensors initialized!");
  Serial.println();

  // ==== Fan Control ====
  // Initialize PWM pin as PWM output
  pinMode(FAN_PWM, OUTPUT);

  // ==== Heat Relay Control ====
  // Initialize IO pin as output
  pinMode(HEAT_RELAY_PIN, OUTPUT);

  Serial.println("For debugMode please press anykey.");
  Serial.println();
  delay(1000);

  analogWrite(FAN_PWM, 220);

  // ==== PID Controller ====
  // Set initial setpoint to 0
  setPoint = 27; 

  // Perform first PID computation and define output for first period
  sensors.requestTemperatures();
  float tempC_S0 = sensors.getTempC(sensor0);
  float tempC_S1 = sensors.getTempC(sensor1);
  float tempC_mean = ((tempC_S0+tempC_S1)/2);

  //dutyCycle = computePID(tempC_mean);
  //digitalWrite(HEAT_RELAY_PIN, HIGH);
  relay_state = true;
  LastPeriod = millis();
}



// ##########################################
// ##                 Main                 ##
// ##########################################
void loop()
{
  if (Serial.available() > 0)
  {
     debugMode();
  }

  // Turn off the relay once we reach the end of the active state of the duty cycle
  if (((millis() - LastPeriod) >= dutyCycle) && relay_state == true)
  {
    //digitalWrite(HEAT_RELAY_PIN, LOW);
    relay_state = false;
  }
  
  // Recompute the PID dutycycle once the period has complete
  if ((millis() - LastPeriod) >= period)
  {
    sensors.requestTemperatures();
    float tempC_S0 = sensors.getTempC(sensor0);
    float tempC_S1 = sensors.getTempC(sensor1);
    float tempC_S2 = sensors.getTempC(sensor2);
    float tempC_mean = ((tempC_S0+tempC_S1)/2);


    int PID_output = computePID(tempC_mean);
    dutyCycle = (period/100) * PID_output;
    //digitalWrite(HEAT_RELAY_PIN, HIGH);
    relay_state = true;
    LastPeriod = millis();

    Serial.print("Current duty cycle: ");
    Serial.println(dutyCycle);
    Serial.println();
  }

  

/*
  // Debugging and logging - Create a JSON document and send it over Serial
  StaticJsonDocument<96> doc;

  doc["tick"] = tick_counter;
  JsonObject sensors_0 = doc["sensors"].createNestedObject();
  sensors_0["sensor00"] = tempC_S0;
  sensors_0["sensor01"] = tempC_S1;
  sensors_0["sensor02"] = tempC_S2;
  doc["sensorMean"] = tempC_mean;
  doc["fan"] = 255;
  doc["heatingElement"] = relay_state;

  serializeJson(doc, Serial);
  Serial.println();
  */

  tick_counter++;
}



// ##########################################
// ##               Functions              ##
// ##########################################

// Function to update the PID controller
double computePID(double input){
  double kp = PID_KP;
  double ki = PID_KI;
  double kd = PID_KD;    

  currentTime = millis();                                 //get current time
  elapsedTime = (double)(currentTime - previousTime);     //compute time elapsed from previous computation

  error = setPoint - input;                               // determine error
  cumError += error * (elapsedTime / 1000);                        // compute integral
  rateError = (error - lastError)/(elapsedTime / 1000);            // compute derivative

  lastError = error;                                      //remember current error
  previousTime = currentTime;                             //remember current time

  double output = kp*error + ki*cumError + kd*rateError;

  // Make sure that the output does not exceed some extreme values
  if (output > 1000)
  {
    output = 1000;
  }
  else if (output < -1000)
  {
    output = -1000;
  }

  // Normailize the output to be in promille
  output = map(output, -1000, 1000, 0, 100);
  
  Serial.print("error: ");
  Serial.println(error);
  Serial.print("cumError: ");
  Serial.println(cumError);
  Serial.print("rateError: ");
  Serial.println(rateError);
  Serial.print("output: ");
  Serial.println(output);

  return output;
}


// Function that takes over the main loop and gives full control of the device.
void debugMode()
{
  int debugmode_enabled = true;
  char rx_byte = 0;

  if (Serial.available() > 0) 
  {
    rx_byte = Serial.read();
  }

  while (debugmode_enabled)
  {
    Serial.println("\nCommands:\n");
    Serial.println("\t1 - Read temperature\n");
    Serial.println("\t2 - Set PWM\n");
    Serial.println("\t3 - Change relay state to on\n");
    Serial.println("\t4 - Change relay state to off\n");
    Serial.println("\t5 - Scan for OneWire sensors\n");
    Serial.println("\t0 - Exit debugMode and return to normal operation\n");
    Serial.println("Choice: ");
    
    while(Serial.available() == 0)
    {
      delay(100);
    }

    if (Serial.available() > 0) 
    {    // is a character available?
      rx_byte = Serial.read();       // get the character
      Serial.print(rx_byte);
      Serial.print("\n");

      // check if a number was received
      if (rx_byte == '1')
      {
        Serial.println("\nReading temperature sensors. Press anykey to return.");
        bool read_temp = 1;
        while (read_temp)
        {
          sensors.requestTemperatures();
          printData(sensor0);
          printData(sensor1);
          printData(sensor2);
          Serial.println();

          char rx_byte_alt = 0;
          if (Serial.available() > 0) 
          {    // is a character available?
            rx_byte_alt = Serial.read();       // get the character
            read_temp = 0;
          }
        }
      }
      else if (rx_byte == '2')
      {
        char rx_byte_alt = 0;
        String rx_str_alt = "";
        bool read_str = 1;

        Serial.print("\nPlease input PWM between 100 to 255: ");

        while (read_str)
        {
          if (Serial.available() > 0) 
          {    // is a character available?
            rx_byte_alt = Serial.read();       // get the character
            
            if (rx_byte_alt != '\n') 
            {
              // a character of the string was received
              rx_str_alt += rx_byte_alt;
              Serial.print(rx_byte_alt);
            }
            else 
            {
              Serial.print("\nSetting pwm to ");
              Serial.println(rx_str_alt.toInt());
              Serial.println();
              Serial.println();
              analogWrite(FAN_PWM, rx_str_alt.toInt());
              read_str = 0;
            }
          }
        }
      }
      else if (rx_byte == '3')
      {
        Serial.println("\nTurning on the relay...");
        digitalWrite(HEAT_RELAY_PIN, HIGH);
      }
      else if (rx_byte == '4')
      {
        Serial.println("\nTurning off the relay...");
        digitalWrite(HEAT_RELAY_PIN, LOW);
      }
      else if (rx_byte == '5')
      {
        findDevices(ONE_WIRE_BUS);
        Serial.println("\n//\n// End oneWireSearch.ino //");
        Serial.println();
      }
      else if (rx_byte == '0')
      {
        debugmode_enabled = false;
      }
      else 
      {
        Serial.println("Command not recognized!");
      }
    }
  }

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


// Function which searches the OneWire bus and prints results
uint8_t findDevices(int pin)
{
  OneWire ow(pin);

  uint8_t address[8];
  uint8_t count = 0;


  if (ow.search(address))
  {
    Serial.print("\nuint8_t pin");
    Serial.print(pin, DEC);
    Serial.println("[][8] = {");
    do {
      count++;
      Serial.println("  {");
      for (uint8_t i = 0; i < 8; i++)
      {
        Serial.print("0x");
        if (address[i] < 0x10) Serial.print("0");
        Serial.print(address[i], HEX);
        if (i < 7) Serial.print(", ");
      }
      Serial.println("  },");
    } while (ow.search(address));

    Serial.println("};");
    Serial.print("// nr devices found: ");
    Serial.println(count);
  }

  return count;
}
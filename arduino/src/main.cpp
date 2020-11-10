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



// ##########################################
// ##              Defines                 ##
// ##########################################
/*
 * This section defines the settings for the program, such as pin numbers and operation modi.
*/

// Should the code be compiled with additional outputs? 0=off, 1=normal
#define DEBUGMODE 0

// Pin for onewire interface. The sensors are installed in series running without parasitic power.
#define ONEWIRE_PIN 10

// Pin on which to run the PWM for the motor controller in charge of the fan. The H-bridge is hard-wired.
#define FAN_PWM 11

// Pin controlling the relay in charge of the heating element.
#define HEAT_RELAY_PIN 8



// ##########################################
// ##                Setup                 ##
// ##########################################
/*
 * The setup function takes care of initialization, and runs only once.
*/
void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}



// ##########################################
// ##                 Main                 ##
// ##########################################
/*
 * The loop function subsitudes the while(1){} function.
*/
void loop()
{
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  // wait for a second
  delay(1000);
  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
   // wait for a second
  delay(1000);
}
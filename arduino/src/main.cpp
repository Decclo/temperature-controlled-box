/*
 * Heat Regulated Electric Oven 
 * 
 * This code works as a controller for box that is to keep its internal temperature stable.
 */



// ##########################################
// ##              Includes                ##
// ##########################################
#include <Arduino.h>



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
 * The loop function subsitudes the while(1)-function.
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
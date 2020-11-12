#include "support_func.h"

#include <Arduino.h>
#include <OneWire.h>

extern OneWire  ds;  // on pin 10 (a 4.7K resistor is necessary)


class OneWire_Customized
{
    public:
        OneWire_Customized()
        {
            uint8_t counter = 0;
            while (ds.search(addr)) 
            {
                uint8_t i;
                Serial.print("Found device with ROM =");
                for( i = 0; i < 8; i++) 
                {
                    Serial.write(' ');
                    Serial.print(addr[i], HEX);
                };
                Serial.print("\n");
                onewire_addr[counter] = addr;
                counter++;
                Serial.print("Converted to storage");
                for( i = 0; i < 8; i++) 
                {
                    Serial.write(' ');
                    Serial.print(onewire_addr[counter][i], HEX);
                };
                Serial.print("\n");
            };
            Serial.println("No more addresses.");
            Serial.println();
            ds.reset_search();
            delay(250);
        };
    
    protected:

    private:
    byte present = 0;
    uint8_t type_s;
    uint8_t onewire_addr[3][8] = { };
    float celsius, fahrenheit;
    
};
# temperature-controlled-box
A control system to manage the temperature in an isolated box.
The box is meant to house a resin printer which needs a constant temperature for best results, and to be able to place said printer ina a non-heated room, such as a garage, to keep potential toxic fumes away from the living area.

The IDE used for writing this code is Visual Studio Code with the PlatformIO add-on. This allows me to write C++ code and deploy it to the Arduino board with ease, and I'm even able to use the simplified Arduino functions and libraries that usually wouldn't be compatible with pure C++. Another nice feature of PlatformIO is its ability to upload code to and monitor the microcontroller using a remote computer, such as a Raspberry Pi which can be inside the box.

### Components needed:
1. An isolated box, in this case with a wooden frame with a double layered plexiglass front panel, removable top, and styrofoam on the inside.
2. [A power supply which covers all our power requirements](https://www.amazon.de/gp/product/B07BLR16PB/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1).
3. [Arduino Nano](https://www.banggood.com/Geekcreit-ATmega328P-Nano-V3-Module-Improved-Version-No-Cable-Development-Board-Geekcreit-for-Arduino-products-that-work-with-official-Arduino-boards-p-959231.html?cur_warehouse=CN&rmmds=search) microcontroller.
4. A heating unit, in this case a [150W 12V DC PTC Fan Heater](https://www.banggood.com/150W-12V-DC-PTC-Fan-Heater-Constant-Temperature-With-Connection-Cable-p-1120251.html?cur_warehouse=CN&rmmds=search) with integrated fan.
5. A motor controller to take care of the fan, such as the [L298N Dual H bridge motor driver](https://www.banggood.com/Wholesale-L298N-Dual-H-Bridge-Stepper-Motor-Driver-Board-p-42826.html?cur_warehouse=CN&rmmds=search), which also features a 5v voltage regulator.
6. A [relay](https://www.banggood.com/3Pcs-5V-Relay-5-12V-TTL-Signal-1-Channel-Module-High-Level-Expansion-Board-p-1178211.html?cur_warehouse=CN&rmmds=search) to turn the heating element on and off.
7. We'll also need some temperature sensors for feedback, my personal favorite being the [DS18B20 OneWire digital temperature probes](https://www.banggood.com/DS18B20-Waterproof-Digital-Temperature-Temp-Sensor-Probe-1M-2M-3M-5M-10M-15M-p-1211828.html?cur_warehouse=CN&ID=45763&rmmds=search).
8. At last, we'll need some assorted cables and soldering skills. A power socket rail would also be neat so we have easy power access for the printer.

### Assembly
The box really depends on your tastes and needs.
![signal-2021-03-24-194901_002](https://user-images.githubusercontent.com/25606661/112367431-30895d80-8cda-11eb-955a-b8b967ac7dbb.jpeg)
![signal-2021-03-24-194901_003](https://user-images.githubusercontent.com/25606661/112367433-3121f400-8cda-11eb-8844-0a34f85c6d95.jpeg)

The power supply is connected in the following way:  
Power supply (12v output)  
|-> L298N motor driver's power in  
| |-> Fan (motor A out)  
| |-> Relay (5v relugated output)  
| |-> DS18B20 temperature sensors (5v relugated output)  
|-> Arduino Nano's VIN  
|-> Relay (switch)  
| |-> Heating unit  

While the positive rail can be wired as stated above, there are no limits for the ground rail. Preferable every component would connect to every other components ground, but since this would be cumbersome, the components which give signals to other components have an additional ground wire along the signal wire. This results in a mesh-like ground rail.

It should also be noted that the 12v input connected to the Arduino Nano's VIN is made with a removable connector, as to be able to change the power source from the power supply and to a computer to be able to upload code to the arduino without risking the power flowing the wrong way through the device and potentially burning the computers USB port. 

**Always make sure the arduino's vin pin is not connected when connecting it to the computer!**

![signal-2021-03-24-194901_001](https://user-images.githubusercontent.com/25606661/112367429-2ff0c700-8cda-11eb-82a0-a78cce96e969.jpeg)

The devices are connected to the Arduino Nano as follows:  
  Pin 10: Onewire (additionally has 4.7kOhm pullup resistor to arduino's 5v rail)  
  Pin 09: Motor driver PWM for fan  
  Pin 11: Digital output for relay controlling heating element  
  
  ### Repository Setup
  1. Download [Microsoft Visual Studio Code](https://code.visualstudio.com/) (and no, Visual Studio Codium does sadly nt work because of PlatformIO dependencies)
  2. Go into 'Extensions' and install the 'PlatformIO' extension.
  3. Once installed, go to 'PlatformIO -> Platforms' and install 'Atmel AVR'
  4. Download this repository if you haven't done so already.
  5. In VSCode, open this repositories 'Arduino' directory. This is important as PlatformIO will look for its files in the project root directory (the arduino directory)
  6. Initialize the required modules by running `git submodule init && git submodule update` in either your local terminal or VSCode's terminal (requires you to be in the repository directory)

And that's it! You should now be able to compile the code by pressing the 'PlatformIO:Build' button on the blue bar at the bottom of VSCode. Board and uplaod preferences can be changed in 'platformio.ini'.









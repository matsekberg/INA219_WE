# INA219_WE
An Arduino library for the INA219 current and power sensor module.

It allows to set various ADC modes, gains and bus voltage ranges. Continuous mode, power down and ADC off mode are supported. Default shunt size is 75mV at 100A, but you can change it. 
It has been enhanced with shunt voltage calibration

I attached a list of public functions and a fritzing scheme for the example sketches.

You find further details here: 

https://wolles-elektronikkiste.de/ina219  (German)

https://wolles-elektronikkiste.de/en/ina219-current-and-power-sensor (English)

Note: Version 1.1.2 corrected a misspelling of "continuous" which might lead to backwards incompatibilities. You might need to change your code.  

Note: Version 2.0.0 removed lot of methods and implemented corrections for input offset errors due to filtering resistors on Vin+ and Vin-.

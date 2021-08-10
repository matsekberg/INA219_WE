/***************************************************************************
* Example sketch for the INA219_WE library
*
* This sketch shows how to use the INA219 module in continuous mode. 
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ina219 (German)
* https://wolles-elektronikkiste.de/en/ina219-current-and-power-sensor (English)
* 
***************************************************************************/
#include <Wire.h>
#include <INA219_WE.h>
#define I2C_ADDRESS 0x40

/* There are several ways to create your INA219 object:
 * INA219_WE ina219 = INA219_WE()              -> uses Wire / I2C Address = 0x40
 * INA219_WE ina219 = INA219_WE(ICM20948_ADDR) -> uses Wire / I2C_ADDRESS
 * INA219_WE ina219 = INA219_WE(&wire2)        -> uses the TwoWire object wire2 / I2C_ADDRESS
 * INA219_WE ina219 = INA219_WE(&wire2, I2C_ADDRESS) -> all together
 * Successfully tested with two I2C busses on an ESP32
 */
INA219_WE ina219 = INA219_WE(I2C_ADDRESS);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if(!ina219.init()){
    Serial.println("INA219 not connected!");
  }

  /* Set ADC Mode for Bus and ShuntVoltage
  * Mode *            * Res / Samples *       * Conversion Time *
  BIT_MODE_9        9 Bit Resolution             84 µs
  BIT_MODE_10       10 Bit Resolution            148 µs  
  BIT_MODE_11       11 Bit Resolution            276 µs
  BIT_MODE_12       12 Bit Resolution            532 µs  (DEFAULT)
  SAMPLE_MODE_2     Mean Value 2 samples         1.06 ms
  SAMPLE_MODE_4     Mean Value 4 samples         2.13 ms
  SAMPLE_MODE_8     Mean Value 8 samples         4.26 ms
  SAMPLE_MODE_16    Mean Value 16 samples        8.51 ms     
  SAMPLE_MODE_32    Mean Value 32 samples        17.02 ms
  SAMPLE_MODE_64    Mean Value 64 samples        34.05 ms
  SAMPLE_MODE_128   Mean Value 128 samples       68.10 ms
  */
  //ina219.setADCMode(SAMPLE_MODE_128); // choose mode and uncomment for change of default
  
  /* Set measure mode
  POWER_DOWN - INA219 switched off
  TRIGGERED  - measurement on demand
  ADC_OFF    - Analog/Digital Converter switched off
  CONTINUOUS  - Continuous measurements (DEFAULT)
  */
  // ina219.setMeasureMode(CONTINUOUS); // choose mode and uncomment for change of default
  
  /* Set PGain
  * Gain *  * Shunt Voltage Range *   * Max Current (if shunt is 0.1 ohms) *
   PG_40       40 mV                    0.4 A
   PG_80       80 mV                    0.8 A
   PG_160      160 mV                   1.6 A
   PG_320      320 mV                   3.2 A (DEFAULT)
  */
  // ina219.setPGain(PG_320); // choose gain and uncomment for change of default
  
  /* Set Bus Voltage Range
   BRNG_16   -> 16 V
   BRNG_32   -> 32 V (DEFAULT)
  */
  // ina219.setBusRange(BRNG_32); // choose range and uncomment for change of default

  Serial.println("INA219 Current Sensor Example Sketch - Continuous");

  /* If the current values delivered by the INA219 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by INA219
  */
  // ina219.setCorrectionFactor(0.98); // insert your correction factor if necessary
  
}

void loop() {


  uint16_t bus_reg = ina219.readRegister(INA219_BUS_REG);
  uint16_t shunt_reg = ina219.readRegister(INA219_SHUNT_REG);

  printf("\n");
  printf("\nShunt reg   = %d", shunt_reg);
  printf("\nBus reg     = %d", bus_reg);
  printf("\nCurrent reg = %d", ina219.readRegister(INA219_CURRENT_REG));
  printf("\nCal reg     = %d", ina219.readRegister(INA219_CAL_REG));
  printf("\nConfig reg  = %04X \n", ina219.readRegister(INA219_CONF_REG));

  float bus_voltage = 0.004 * (bus_reg >> 3);
  float shunt_offset_V = -(SHUNT_VOLTAGE_OFFSET_FACTOR * bus_voltage);
  float shunt_voltage_V = 0.000010 * shunt_reg;
  float current = (shunt_voltage_V + shunt_offset_V) / SHUNT_RES;

  if (current < 0.0) current = 0.0;

  printf("\nShunt offset [V]:  %10.6f  \nShunt Voltage [V]: %10.6f  \nCurrent [A]:       %10.2f  \nBus voltage [V]:   %10.2f",
         shunt_offset_V,
         shunt_voltage_V,
         current,
         bus_voltage);


  printf("\n\n-----------------------------------");

  
  delay(3000);
}

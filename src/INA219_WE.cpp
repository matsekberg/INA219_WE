/*****************************************
* This is a library for the INA219 Current Sensor Module
*
* You'll find an example which should enable you to use the library. 
*
* You are free to use it, change it or build on it. In case you like 
* it, it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
* https://wolles-elektronikkiste.de/en/ina219-current-and-power-sensor (English)
* https://wolles-elektronikkiste.de/ina219 (German)
*
*******************************************/

#include "INA219_WE.h"

INA219_WE::INA219_WE(uint8_t addr){
    _wire = &Wire;
    i2cAddress = addr;   
    setMeasureMode(CONTINUOUS);
}

INA219_WE::INA219_WE(){
    _wire = &Wire;
    i2cAddress = 0x40;   
    setMeasureMode(CONTINUOUS);
}

INA219_WE::INA219_WE(TwoWire *w, uint8_t addr){
    _wire = w;
    i2cAddress = addr; 
    setMeasureMode(CONTINUOUS);
}

INA219_WE::INA219_WE(TwoWire *w){
    _wire = w;
    i2cAddress = 0x40;
    setMeasureMode(CONTINUOUS);
}
    
bool INA219_WE::init(){ 
    if( !reset_INA219() )
    {
        return false;
    }
    setADCMode(SAMPLE_MODE_128);
    setMeasureMode(CONTINUOUS);
    setPGain(PG_40);
    setBusRange(BRNG_16);
    setShuntCharacteristics(0.075, 100);
    setOffsetCorrection(4, 12.96); // 0 ohms resistor
    //setOffsetCorrection(41, 12.96); // 10 ohms resistor
    return true;
}

bool INA219_WE::reset_INA219(){
    byte ack = writeRegister(INA219_CONF_REG, INA219_RST); 
    return ack == 0;
}

void INA219_WE::setADCMode(INA219_ADC_MODE mode){
    deviceADCMode = mode;
    uint16_t currentConfReg = readInaRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x0780);  
    currentConfReg &= ~(0x0078);
    uint16_t adcMask = mode<<3;
    currentConfReg |= adcMask;
    adcMask = mode<<7;
    currentConfReg |= adcMask;
    writeRegister(INA219_CONF_REG, currentConfReg);
}



void INA219_WE::setPGain(INA219_PGAIN gain){
    devicePGain = gain;
    uint16_t currentConfReg = readInaRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x1800);
    currentConfReg |= devicePGain;
    writeRegister(INA219_CONF_REG, currentConfReg);
    
    switch(devicePGain){
        case PG_40:
            calVal = 20480;
            break;
        case PG_80:
            calVal = 10240;
            break;
        case PG_160:
            calVal = 8192;
            break;
        case PG_320:
            calVal = 4096;
            break;
    }
    
    writeRegister(INA219_CAL_REG, calVal);
            
}

void INA219_WE::setBusRange(INA219_BUS_RANGE range){
    deviceBusRange = range;
    uint16_t currentConfReg = readInaRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x2000);
    currentConfReg |= deviceBusRange;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

/** 
 * Set offset correction; zero current voltage offset bit value at bus voltage
 */
void INA219_WE::setOffsetCorrection(uint16_t shuntBits, float atBusVoltage) {
    _shuntBits = shuntBits;
    _atBusVoltage = atBusVoltage;
}

/**
 * Set shunt voltage at specifie current
 */
void INA219_WE::setShuntCharacteristics(float shuntVoltage, float atCurrent) {
    _shuntVoltage = shuntVoltage;
    _atCurrent = atCurrent;
}

/**
 * Set shunt korrection
 */
void INA219_WE::setShuntCorrection(float k) {
    _shunt_k = k;
}

/**
 * Calculate offset correction at the specified voltage
 */
float INA219_WE::getOffsetCorrection() {
    return  -(_shuntBits * 0.000010 / _atBusVoltage * getBusVoltage());
}

float INA219_WE::getBusVoltage() {
    uint16_t bus_reg = readInaRegister(INA219_BUS_REG);
    return 0.004 * (bus_reg >> 3);
}


float INA219_WE::getShuntVoltage() {
    int16_t sv = (int16_t)readInaRegister(INA219_SHUNT_REG);
    return (0.000010 * sv + getOffsetCorrection()) * _shunt_k;
}

//float _shuntVoltage = 0.075;
//float _atCurrent = 100.0;

float INA219_WE::getCurrent() {
    return getShuntVoltage() / (_shuntVoltage / _atCurrent);
}

uint8_t INA219_WE::writeRegister(uint8_t reg, uint16_t val){
  _wire->beginTransmission(i2cAddress);
  uint8_t lVal = val & 255;
  uint8_t hVal = val >> 8;
  _wire->write(reg);
  _wire->write(hVal);
  _wire->write(lVal);
  return _wire->endTransmission();
}
  
uint16_t INA219_WE::readInaRegister(uint8_t reg){
  uint8_t MSByte = 0, LSByte = 0;
  uint16_t regValue = 0;
  _wire->beginTransmission(i2cAddress);
  _wire->write(reg);
  _wire->endTransmission();
  _wire->requestFrom(i2cAddress,(uint8_t)2);
  if(_wire->available()){
    MSByte = _wire->read();
    LSByte = _wire->read();
  }
  regValue = (MSByte<<8) + LSByte;
  return regValue;
}

uint8_t INA219_WE::getAddress(void) {
    return i2cAddress;
}

/************************************************ 
    private functions
*************************************************/


void INA219_WE::setMeasureMode(INA219_MEASURE_MODE mode){
    deviceMeasureMode = mode;
    uint16_t currentConfReg = readInaRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x0007);
    currentConfReg |= deviceMeasureMode;
    writeRegister(INA219_CONF_REG, currentConfReg);
}



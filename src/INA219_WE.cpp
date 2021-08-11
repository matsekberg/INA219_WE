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

INA219_WE::INA219_WE(int addr){
    _wire = &Wire;
    i2cAddress = addr;   
}

INA219_WE::INA219_WE(){
    _wire = &Wire;
    i2cAddress = 0x40;   
}

INA219_WE::INA219_WE(TwoWire *w, int addr){
    _wire = w;
    i2cAddress = addr; 
}

INA219_WE::INA219_WE(TwoWire *w){
    _wire = w;
    i2cAddress = 0x40;
}
    
bool INA219_WE::init(){ 
    if( !reset_INA219() )
    {
        return false;
    }
    setADCMode(BIT_MODE_12);
    setMeasureMode(CONTINUOUS);
    setPGain(PG_320);
    setBusRange(BRNG_32);
    return true;
}

bool INA219_WE::reset_INA219(){
    byte ack = writeRegister(INA219_CONF_REG, INA219_RST); 
    return ack == 0;
}

void INA219_WE::setADCMode(INA219_ADC_MODE mode){
    deviceADCMode = mode;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x0780);  
    currentConfReg &= ~(0x0078);
    uint16_t adcMask = mode<<3;
    currentConfReg |= adcMask;
    adcMask = mode<<7;
    currentConfReg |= adcMask;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

void INA219_WE::setMeasureMode(INA219_MEASURE_MODE mode){
    deviceMeasureMode = mode;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x0007);
    currentConfReg |= deviceMeasureMode;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

void INA219_WE::setPGain(INA219_PGAIN gain){
    devicePGain = gain;
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
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
    uint16_t currentConfReg = readRegister(INA219_CONF_REG);
    currentConfReg &= ~(0x2000);
    currentConfReg |= deviceBusRange;
    writeRegister(INA219_CONF_REG, currentConfReg);
}

void setOffsetCorrection(uint16_t shuntBits, float atBusVoltage) {
    _shuntBits = shuntBits;
    _atBusVoltage = atBusVoltage;
}

void setShuntCharacteristics(float shuntVoltage, float atCurrent) {
    _shuntVoltage = shuntVoltage;
    _atCurrent = atCurrent;
}


float getOffsetCorrection() {
    return  -(_shuntBits * 0.000010 / _atBusVoltage) * getBusVoltage());
}

float getBusVoltage() {
    uint16_t bus_reg = ina219.readRegister(INA219_BUS_REG);
    float bus_voltage = 0.004 * (bus_reg >> 3);
    return bus_voltage;
}


float getShuntVoltage() {
    return 0.000010 * ina219.readRegister(INA219_SHUNT_REG) + getOffsetCorrection();
}

    float _shuntVoltage = 0.075;
    float _atCurrent = 100.0;

float getCurrent() {
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
  
uint16_t INA219_WE::readRegister(uint8_t reg){
  uint8_t MSByte = 0, LSByte = 0;
  uint16_t regValue = 0;
  _wire->beginTransmission(i2cAddress);
  _wire->write(reg);
  _wire->endTransmission();
  _wire->requestFrom(i2cAddress,2);
  if(_wire->available()){
    MSByte = _wire->read();
    LSByte = _wire->read();
  }
  regValue = (MSByte<<8) + LSByte;
  return regValue;
}

/************************************************ 
    private functions
*************************************************/


    



/***************************************************************************
 * ADS1299 SPI Bridge
 * Tested and developed on an Adafruit Huzzah32.
 * 
 * Author: Ian Romano
 ***************************************************************************/

#include "ADS1299.h"
#include "Arduino.h"
#include <SPI.h>

/**
 *  @brief class constructor for SleepSure_ADS1299
 */
SleepSure_ADS1299::SleepSure_ADS1299(int8_t cspin, int8_t drdypin, int8_t startpin, int8_t resetpin, SPIClass *theSPI)
{
    _cs = cspin;
    _drdy = drdypin;
    _mosi = _miso = _sck = -1;
    start = startpin;
    _reset = resetpin;
    _spi = theSPI;
}
SleepSure_ADS1299::SleepSure_ADS1299(int8_t cspin, int8_t drdypin, int8_t mosipin, int8_t misopin, int8_t sckpin, int8_t startpin, int8_t resetpin)
    : _cs(cspin), _drdy(drdypin), _mosi(mosipin), _miso(misopin), _sck(sckpin), start(startpin), _reset(resetpin) {}

SleepSure_ADS1299::~SleepSure_ADS1299() {};

/**
 * @brief Initializes the ADS1299
 * 
 * @return ID check passed
 */
bool SleepSure_ADS1299::begin()
{
    digitalWrite(_cs, HIGH);
    pinMode(_cs, OUTPUT);
    pinMode(_drdy, INPUT);
    digitalWrite(start, LOW);
    digitalWrite(_reset, HIGH);
    pinMode(start, OUTPUT);
    pinMode(_reset, OUTPUT);
    _spi->begin();
    _spi->setFrequency(ADS1299_SPI_FREQ);

    delay(1);
    command(ADS1299_COMMAND_RESET);

    _sensorID = read(ADS1299_REGISTER_ID);
    write(ADS1299_REGISTER_CONFIG3, 0xE0);     //Using Internal Reference, setting PDB_REFBUF = 1
    write(ADS1299_REGISTER_CONFIG1, 0x96);      //Set Device for DR = fmod / 4096
    write(ADS1299_REGISTER_CONFIG2, 0xC0);
    write(ADS1299_REGISTER_MISC1, 0x00);
    //Set all Channels to input short
    write(ADS1299_REGISTER_CH1SET, 0x01);
    write(ADS1299_REGISTER_CH2SET, 0x01);
    write(ADS1299_REGISTER_CH3SET, 0x01);
    write(ADS1299_REGISTER_CH4SET, 0x01);
    write(ADS1299_REGISTER_CH5SET, 0x01);
    write(ADS1299_REGISTER_CH6SET, 0x01);
    write(ADS1299_REGISTER_CH7SET, 0x01);
    write(ADS1299_REGISTER_CH8SET, 0x01);

    command(ADS1299_COMMAND_START);
    delay(10);

    int testChannels[8];
    readChannels(testChannels, 8);

    command(ADS1299_COMMAND_SDATAC);
    write(ADS1299_REGISTER_CONFIG2, 0xD0);
    write(ADS1299_REGISTER_CH1SET, 0x05);
    write(ADS1299_REGISTER_CH2SET, 0x05);
    write(ADS1299_REGISTER_CH3SET, 0x05);
    write(ADS1299_REGISTER_CH4SET, 0x05);
    write(ADS1299_REGISTER_CH5SET, 0x05);
    write(ADS1299_REGISTER_CH6SET, 0x05);
    write(ADS1299_REGISTER_CH7SET, 0x05);
    write(ADS1299_REGISTER_CH8SET, 0x05);
    command(ADS1299_COMMAND_RDATAC);
    readChannels(testChannels, 8);      //Reading Square Waves

    command(ADS1299_COMMAND_SDATAC);
    write(ADS1299_REGISTER_CONFIG3, 0xEC);
    write(ADS1299_REGISTER_CH1SET, 0x68);
    write(ADS1299_REGISTER_CH2SET, 0x68);
    write(ADS1299_REGISTER_CH3SET, 0x68);
    write(ADS1299_REGISTER_CH4SET, 0x68);
    write(ADS1299_REGISTER_CH5SET, 0x68);
    write(ADS1299_REGISTER_CH6SET, 0x68);
    write(ADS1299_REGISTER_CH7SET, 0x68);
    write(ADS1299_REGISTER_CH8SET, 0x68);
    command(ADS1299_COMMAND_RDATAC);

    uint8_t sensorCheck = _sensorID & 0x1F;
    if (sensorCheck == ADS1299_ID || sensorCheck == ADS1299_6_ID  || sensorCheck == ADS1299_4_ID ) { return true; }
    
    delete(testChannels);

    return false;

}
/**
 * @brief Sends a command to the ADS via SPI
 * 
 * @param cmd The command to be sent
 * @return uint8_t response, if any
 */
uint8_t SleepSure_ADS1299::command(uint8_t cmd)
{
    uint8_t value;
    _spi->beginTransaction(SPISettings(ADS1299_SPI_FREQ, MSBFIRST, SPI_MODE1));
    digitalWrite(_cs, LOW);
    _spi->transfer(cmd);
    value = _spi->transfer(0);
    digitalWrite(_cs, HIGH);
    _spi->endTransaction();
    if (cmd == ADS1299_COMMAND_RESET) { delay(1); }
    return value;
}

/**
 *   @brief  Reads an 8-bit value from a register via SPI
 * 
 *   @param reg  The address of the 8-bit register
 *   @returns the 8-bits read from the register
 */
uint8_t SleepSure_ADS1299::read(uint8_t reg){   return readWrite(reg, 0x00, ADS1299_COMMAND_RREG);}

/**
 * @brief Write 8-bits to an 8-bit register
 * 
 * @param reg The address of the 8-bit register
 * @param data The data to be written to the register
 */
void SleepSure_ADS1299::write(uint8_t reg, uint8_t data) {readWrite(reg, data, ADS1299_COMMAND_WREG);}

/**
 * @brief Read or write to or from a register. This should only be called 
 * from the read() or write() function and not called explicitly.
 * 
 * @param reg 
 * @param data 
 * @param cmd 
 * @return uint8_t 
 */
uint8_t SleepSure_ADS1299::readWrite(uint8_t reg, uint8_t data, uint8_t cmd)
{
    uint8_t value;
    _spi->beginTransaction(SPISettings(ADS1299_SPI_FREQ, MSBFIRST, SPI_MODE1));
    digitalWrite(_cs, LOW);
    _spi->transfer(ADS1299_COMMAND_SDATAC);

    uint8_t commandReg = cmd | reg;
    _spi->transfer(commandReg);
    _spi->transfer(0);
    value = _spi->transfer(data);

    _spi->transfer(ADS1299_COMMAND_RDATAC);
    digitalWrite(_cs, HIGH);
    _spi->endTransaction();

    return value;
}

bool SleepSure_ADS1299::readChannels(int *values, int8_t len)
{
    if (!digitalRead(_drdy))
    {
        int packetSize = (len + 1) * 3;      //Packet size is (channels +1) * 3
        uint8_t packet[packetSize];
        _spi->beginTransaction(SPISettings(ADS1299_SPI_FREQ, MSBFIRST, SPI_MODE1));
        digitalWrite(_cs, LOW);
        for (int i=0; i<packetSize; i++)
        {
            packet[i] = _spi->transfer(0x00);
        }
        for (int i=0; i<len; i++)
        {
            int value = 0x00000000;
            for (int j=0; j<3; j++)
            {
                value = (value << 8) | packet[(i + 1) * 3 + j];
            }
            if (value > 0x007FFFFF)
            {
                value |= 0xFF000000;
            }
            values[i] = value;
        }
        digitalWrite(_cs, HIGH);
        _spi->endTransaction();
        return true;
    }
    else{ return false; }
}

/**
 * @brief Public getter for _sensorID
 * 
 * @return uint8_t _sensorID
 */
uint8_t SleepSure_ADS1299::getID(){ return _sensorID;}


/***************************************************************************
 * ADS1299 SPI Bridge
 * This Arduino library is mean to bridge a TI ADS1299 chip with an Esspriff
 * ESP32 microcontroller. It uses SPI to slave the ADS to the ESP32 and
 * allows signals to be recieved.
 * 
 * Tested and developed on an Adafruit Huzzah32.
 * 
 * Author: Ian Romano
 ***************************************************************************/

#ifndef __ADS1299_H__
#define __ADS1299_H__

#include "Arduino.h"
#include <SPI.h>

/*!
 *  @brief Register addresses and commands
 */
enum {

    /*** ADS1299 Registers ***/

    //Read Only ID
    ADS1299_REGISTER_ID = 0x00,
    //Global Settings Across Channels
    ADS1299_REGISTER_CONFIG1 = 0x01,
    ADS1299_REGISTER_CONFIG2 = 0x02,
    ADS1299_REGISTER_CONFIG3 = 0x03,
    //Channel-Specific Settings
    ADS1299_REGISTER_LOFF = 0x04,
    ADS1299_REGISTER_CH1SET = 0x05,
    ADS1299_REGISTER_CH2SET = 0x06,
    ADS1299_REGISTER_CH3SET = 0x07,
    ADS1299_REGISTER_CH4SET = 0x08,
    ADS1299_REGISTER_CH5SET = 0x09,
    ADS1299_REGISTER_CH6SET = 0x0A,
    ADS1299_REGISTER_CH7SET = 0x0B,
    ADS1299_REGISTER_CH8SET = 0x0C,
    ADS1299_REGISTER_BIAS_SENSP = 0x0D,
    ADS1299_REGISTER_BIAS_SENSN = 0x0E,
    ADS1299_REGISTER_LOFF_SENSP = 0x0F,
    ADS1299_REGISTER_LOFF_SENSN = 0x10,
    ADS1299_REGISTER_LOFF_FLIP = 0x11,
    //Lead-Off Status Registers (Read-only)
    ADS1299_REGISTER_LOFF_STATP = 0x12,
    ADS1299_REGISTER_LOFF_STATN = 0x13,
    //GPIO and Other Registers
    ADS1299_REGISTER_GPIO = 0x14,
    ADS1299_REGISTER_MISC1 = 0x15,
    ADS1299_REGISTER_MISC2 = 0x16,
    ADS1299_REGISTER_CONFIG4 = 0x17,

    /*** ADS1299 Commands ***/
    //System Commands
    ADS1299_COMMAND_WAKEUP = 0x02,
    ADS1299_COMMAND_STANDBY = 0x04,
    ADS1299_COMMAND_RESET = 0x06,
    ADS1299_COMMAND_START = 0x08,
    ADS1299_COMMAND_STOP = 0x0A,
    //Data Read Commands
    ADS1299_COMMAND_RDATAC = 0x10,
    ADS1299_COMMAND_SDATAC = 0x11,
    ADS1299_COMMAND_RDATA = 0x12,
    //Register Read Commands
    ADS1299_COMMAND_RREG = 0x20,
    ADS1299_COMMAND_WREG = 0x40,

    /*** ADS1299 IDs ***/
    ADS1299_4_ID = 0x1C,
    ADS1299_6_ID = 0x1D,
    ADS1299_ID = 0x1E,
    ADS1299_SPI_FREQ = 1000000
};

/*
 * SleepSure_ADS1299
 */
class SleepSure_ADS1299
{
    public:
        SleepSure_ADS1299(int8_t cspin, int8_t drdypin, int8_t startpin, int8_t resetpin, SPIClass *theSPI = &SPI);
        SleepSure_ADS1299(int8_t cspin, int8_t drdypin, int8_t mosipin, int8_t misopin, int8_t sckpin, int8_t startpin, int8_t resetpin);
        ~SleepSure_ADS1299();

        bool begin();
        uint8_t command(uint8_t);
        uint8_t read(uint8_t reg);
        void write(uint8_t reg, uint8_t data);
        bool readChannels(int *values, int8_t len);
        uint8_t spixfer(uint8_t x);
        uint8_t getID();
        
        int8_t _cs;
        int8_t _drdy;
        int8_t _mosi;
        int8_t _miso;
        int8_t _sck;
        int8_t start;
        int8_t _reset;

        SPIClass *_spi;

    private:
        uint8_t _sensorID;

        uint8_t readWrite(uint8_t reg, uint8_t data, uint8_t cmd);
};

#endif

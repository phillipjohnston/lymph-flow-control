/*
* SPI Device Class
* Created by Phillip Johnston
* 31 May 2012
*
* This class was created to allow common function calls
* to be encapsulated in a base SPIDevice Class
*
*/

#ifndef SPI_DEVICE_H
#define SPI_DEVICE_H

#include <stdint.h>

/********************
* Define Statements *
********************/
//#define SPIDEV_DEBUG

/********************
* Class Declaration * 
********************/
class SPIDevice
{
public:
    SPIDevice();
    ~SPIDevice();
    void setCSPin(uint8_t cs_pin);
    virtual void setupSPI();
    uint8_t getChipSelectPin();
    uint32_t getLastTransmissionResult(void);

protected:
    uint8_t CSpin;
    inline void _enableChipSelect();
    inline void _disableChipSelect();
    void _send(uint8_t a, uint8_t b, uint8_t c);
    void _send(uint8_t a, uint8_t b);
    uint32_t _transfer(uint8_t a, uint8_t b, uint8_t c);
    uint32_t _transfer(uint8_t a, uint8_t b);
};

#endif //SPI_DEVICE_H

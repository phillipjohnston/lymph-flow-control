/*
* SPI Device Class
* Created by Phillip Johnston
* 31 May 2012
*
* This class was created to allow common function calls
* to be encapsulated in a base SPIDevice Class
*
*/

#include "SPIDevice.h"
#include "SPI.h"

/********************
* Define Statements *
********************/
//#define SPIDEV_DEBUG

#ifdef SPIDEV_DEBUG
    #define debugf(msg) Serial.print msg
#else
    #define debugf(msg)
#endif

/***********************
* Function Definitions *
***********************/
SPIDevice::SPIDevice()
{
    //Nothing to construct
}

SPIDevice::~SPIDevice()
{
    //Nothing to destruct
}

void SPIDevice::setupSPI()
{
    //We're gonna default to SPI_MODE1 for now.
    //TODO:  Make this more robust/generic
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE1);
    SPI.setClockDivider(SPI_CLOCK_DIV16);
}

void SPIDevice::setCSPin(uint8_t cs_pin)
{
    CSpin = cs_pin;
    pinMode(CSpin, OUTPUT);
    _disableChipSelect();
}

uint32_t SPIDevice::getLastTransmissionResult()
{
    return _transfer(0x18, 0, 0);
}

inline void SPIDevice::_enableChipSelect()
{ 
    debugf(("Enable Chip Select Function enetered.\n"));
    digitalWrite(CSpin, LOW);
}

inline void SPIDevice::_disableChipSelect()
{
    debugf(("Disable Chip Select Function enetered.\n"));
    digitalWrite(CSpin, HIGH);
}

uint32_t SPIDevice::_transfer(uint8_t a, uint8_t b, uint8_t c)
{
    debugf(("Sending the following message over SPI:"));
    debugf(("\nMSB:  "));
    debugf((a, HEX));
    debugf(("\nMID:  "));
    debugf((b, HEX));
    debugf(("\nLSB:  "));
    debugf((c, HEX));
    debugf(("\n"));        
        
    _enableChipSelect();

    uint8_t a_ret = SPI.transfer(a);
    uint8_t b_ret = SPI.transfer(b);
    uint8_t c_ret = SPI.transfer(c);
    
    _disableChipSelect();
    
    //Store the returned values as a single uint32_t
    uint32_t ret_val = (uint32_t) a_ret;
    ret_val = ((ret_val << 8) | ((uint32_t) b_ret));
    ret_val = ((ret_val << 8) | ((uint32_t) c_ret));
  
    return ret_val;
}

void SPIDevice::_send(uint8_t a, uint8_t b, uint8_t c)
{           
    debugf(("Sending the following message over SPI:"));
    debugf(("\nMSB:  "));
    debugf((a, HEX));
    debugf(("\nMID:  "));
    debugf((b, HEX));
    debugf(("\nLSB:  "));
    debugf((c, HEX));
    debugf(("\n"));     

    _enableChipSelect();

    SPI.transfer(a);
    SPI.transfer(b);
    SPI.transfer(c);

    _disableChipSelect();
}

void SPIDevice::_send(uint8_t a, uint8_t b)
{           
    debugf(("Sending the following message over SPI:"));
    debugf(("\nMSB:  "));
    debugf((a, HEX));
    debugf(("\nLSB:  "));
    debugf((b, HEX));  

    _enableChipSelect();

    SPI.transfer(a);
    SPI.transfer(b);

    _disableChipSelect();
}

uint8_t SPIDevice::getChipSelectPin()
{
    return CSpin;
}

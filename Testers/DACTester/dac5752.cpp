/*
* DAC 5752 Library
* Written by Phillip Johnston
* 3 March 2012
*/

#include "dac5752.h"
#include "SPI.h"

/********************
* Define Statements *
********************/
#ifdef DAC_DEBUG
    #define debugf(msg) Serial.print msg
#else
    #define debugf(msg)
#endif

/***********************
* Function Definitions *
***********************/
DACClass::DACClass()
{
    debugf(("Initializing DAC object...\n"));
}

DACClass::~DACClass()
{
    //SPI.end(); //Release SPI stuff
}

void DACClass::setupSPI()
{
    //SPI Settings for DAC5752
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE2);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
}

uint32_t DACClass::getPowerControl()
{
    _send(RW | POWER_CONTROL, 0, 0);
    return getLastTransmissionResult();
}

void DACClass::setPowerControl(uint8_t channels)
{
    _send(POWER_CONTROL, 0, channels & 15);
     delayMicroseconds(10); //Datasheet specifies a delay of 10 microseconds befoer issuing another command.
}

uint32_t DACClass::getControl()
{
     _send( RW | CONTROL_SET, 0, 0 );
     return getLastTransmissionResult();
}

void DACClass::setControl()
{
    _send(CONTROL_SET,0, CTRL_THERMAL_SHUTDOWN | CTRL_CLAMP_EN);
}

void DACClass::setValue(uint8_t address, uint16_t val)
{
    _send(address,(uint8_t)(val >> 8) & 0xFF,(uint8_t)(val) & 0xFF);
}

void DACClass::setOutputRange(uint8_t address, uint8_t voltage_range)
{
    _send(OUTPUT_RANGE_SEL | address, 0, voltage_range);
}

void DACClass::disableSDO()
{
  _send(CONTROL_SET, 0, CTRL_THERMAL_SHUTDOWN | CTRL_CLAMP_EN | CTRL_SDO_DISABLE);
}

void DACClass::enableSDO()
{
  setControl();
}

/*
void DACClass::powerDownDAC(uint8_t channels)
{
    //To ensure we don't get stuck at some odd place sometimes, we're gonna:
    //Set voltage to zero.
    //Turn off DAC
    if(channels == POWER_DAC_A)
    {
        setValue(DAC_A, 0x0);


    } 
    else if (channels == POWER_DAC_B)
    {

    }
    else if(channels == POWER_DACA_ALL
    {


    }
}
*/

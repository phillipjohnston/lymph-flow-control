/*
* DAC 5752 Library
* Written by Phillip Johnston
* 3 March 2012
*/
#ifndef _DAC_5752_H
#define _DAC_5752_H

#include <stdint.h>
#include "SPIDevice.h"

/*******************
* Type Definitions *
*******************/
typedef enum outputRanges
{
    UNIPOLAR_5V = 0,
    UNIPOLAR_10V,
    BIPOLAR_5V,
    BIPOLAR_10V
} dac_output_range_t;

/********************
* Define Statements *
********************/
//Debugging Definitions
//#define DAC_DEBUG

//Register Definitions
#define RW 128 //BIT 7*, Bit 6 is "zero"
#define REG2 32 //Bit 5
#define REG1 16 //Bit 4
#define REG0 8 //Bit 3
#define __A2 (1 << 2) //Bit 2
#define __A1 (1 << 1) //Bit 1
#define __A0 (1 << 0) //Bit 0
	/* NOTE:
	A2 .. A0 were renamed with two "__" in front, because leaving them as "A2",
	"A1", and "A0" caused them to be valued at 16, 15, and 14 respectively.
	This causes communication with the control register and DAC B to fail.*/

// Power Up Definitions
#define POWER_DAC_A 1
#define POWER_DAC_B (1 << 2)
#define POWER_DAC_ALL POWER_DAC_A | POWER_DAC_B

//Channel Definitions
#define DAC_A 0
#define DAC_B __A1
#define DAC_ALL __A2

//Control Definitions
#define OUTPUT_RANGE_SEL REG0
#define CONTROL (REG0 | REG1)
#define CONTROL_SET (CONTROL | __A0)
#define POWER_CONTROL REG1

#define CTRL_SDO_DISABLE 1
#define CTRL_CLAMP_EN 4
#define CTRL_THERMAL_SHUTDOWN 8

/********************
* Class Declaration * 
********************/
class DACClass : public SPIDevice
{
    public:
        DACClass();
        ~DACClass();

        void setupSPI(); //Differs from SPI Mode 1 used in SPIDevice
        void setOutputRange(uint8_t address, uint8_t voltage_range);
        uint32_t getOutputRange(uint8_t address);
        void setControl();
        void setPowerControl(uint8_t channels);
        void setValue(uint8_t address, uint16_t value);
        uint32_t getPowerControl();
        uint32_t getControl();
        void powerDownDAC(uint8_t channels);
        void enableSDO();
        void disableSDO();
};

#endif

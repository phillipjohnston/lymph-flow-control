/*
* DAC 5752 Library
* Written by Phillip Johnston
* 3 March 2012
*/
#ifndef _DAC_5752_H
#define _DAC_5752_H

#include <stdint.h>
#include <stdbool.h>

/*
* Type Definitions
*/

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

//Register Definitions
#define RW 128 //BIT 7*
#define REG2 32 //Bit 5
#define REG1 16 //Bit 4
#define REG0 8 //Bit 3
#define A2 4 //Bit 2
#define A1 2 //Bit 1
#define A0 1 //Bit 0

// Power Up Definitions
#define PUA 1
#define PUB (1 << 2)
#define POWER_UP_ALL PUA | PUB


//Channel Definitions
#define ADDRESS_A 0
#define ADDRESS_B A1
#define ADDRESS_ALL A2


//Control Definitions
#define OUTPUT_RANGE_SEL REG0
#define CONTROL (REG0 | REG1)
#define CONTROL_SET (CONTROL | A0)
#define POWER_CONTROL REG1
#define CTRL_CLAMP_EN 4
#define CTRL_THERMAL_SHUTDOWN 8


/*******************
* Class Definition * 
*******************/
class DACClass
{
   public:
	  DACClass();
	  ~DACClass();
      void setupSPI(uint8_t cs_pin);
      void init(void);
	  void setOutputRange(uint8_t address, uint8_t voltage_range);
	  uint32_t getOutputRange(uint8_t address);
	  void setControl();
	  void setPowerControl(uint8_t channels);
	  void setValue(uint8_t address, uint16_t value);
	  inline uint32_t getLastTransmissionResult(void);
	  uint32_t getPowerControl();
	  uint32_t getControl();
	  uint8_t getDACChipSelectPin();
	  
   private: 
	uint8_t dacCS;
	inline void _enableChipSelect();
	inline void _disableChipSelect();
	void _send(uint8_t a, uint8_t b, uint8_t c);
	uint32_t _transfer(uint8_t a, uint8_t b, uint8_t c);

};

#endif

/*
* DAC 5752 Library
* Written by Phillip Johnston
* 3 March 2012
*/

#include "dac5752.h"
#include "SPI.h"

#ifdef DAC_DEBUG
  #define debugf(msg) Serial.print msg
#else
  #define debugf(msg)
#endif

/*
* Thoughts and Notes:
*		Implement checking that everything is set correctly after we try tos et it
*				eg output range
*
*		For some reason, we can't work with DAC B.  Also, we can't use the ADDRESS_ALL
*		DAC A doesn't work if ADDRESS_ALL is used.
*
*		Verify SPI is responding for DAC B
*/

DACClass::DACClass()
{
  #ifdef DAC_DEBUG
    //Serial.begin(115200);
    Serial.println("Initializing DAC object...");
  #endif
}

DACClass::~DACClass()
{
	SPI.end(); //Release SPI stuff
}

void DACClass::setupSPI(uint8_t cs_pin)
{
   /*Initialization of SPI constants. The data sheet says MSB_FIRST, and the DAC
   * can take up to a 30 MHz clock. Now, the Uno32 clock is 80 MHz, which would
   * suggest that SPI_CLOCK_DIV4=20 MHz (within the limit), but oscilloscope
   * testing suggests that the division is based off of the Arduino's 16 MHz
   * clock, which means SPI_CLOCK_DIV4=4 MHz. This is still fine, though. The
   * SPI_MODE2 is a point of contention. The timing diagrams on the datasheet
   * seem to suggest (to me) that it should be either SPI_MODE1 or SPI_MODE3.
   * One Analog Devices rep told me that I should use SPI_MODE3, but the latest
   * one suggested SPI_MODE2. As the DAC still hasn't gotten working, this
   * continues to be matter up in the air. Also note that experimentation
   * suggested that you need to setup the SPI parameters after a call to
   * SPI.begin()*/


  /* This is the chip selection pin for SPI, we're setting it high to ensure
   * that no stray signals get sent to the chip*/
   
  dacCS = cs_pin;
  pinMode(dacCS, OUTPUT);
  _disableChipSelect();

  //Initialize the SPI library things
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE2);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
}

inline uint32_t DACClass::getLastTransmissionResult()
{
	return _transfer(0x18, 0, 0);
}

inline void DACClass::_enableChipSelect()
{
	digitalWrite(dacCS, LOW);
}

inline void DACClass::_disableChipSelect()
{
	digitalWrite(dacCS, HIGH);
}

uint32_t DACClass::_transfer(uint8_t a, uint8_t b, uint8_t c)
{
  
        debugf(("Sending the following message to the DAC:"));
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

void DACClass::_send(uint8_t a, uint8_t b, uint8_t c)
{
  
  debugf(("Sending the following message to the DAC:"));
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

uint8_t DACClass::getDACChipSelectPin()
{
	return dacCS;
}

void DACClass::setOutputRange(uint8_t address, uint8_t voltage_range)
{
    _send(OUTPUT_RANGE_SEL | address, 0, voltage_range);
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

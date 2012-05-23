
<!-- saved from url=(0072)https://raw.github.com/TheWorldJoker/lymph-flow-control/master/Servo.cpp -->
<html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"></head><body><pre style="word-wrap: break-word; white-space: pre-wrap;">/*
 * Servo.cpp - Library for interacting with a
 * 	     Parker MX80L linear stage
 * 
 * Created by Jeff Kornuta, September 18, 2011.
 * Modified by Phillip Johnston, 22 May 2012
 * Released into the public domain.
 *
*/

#include "WProgram.h"
/*
  
  EXTERNAL FUNCTIONS FOR MAKE BENEFIT GLORIOUS CODE
  
*/
#include "Servo.h"
#include "HardwareSerial.h"
#include "dac5752.h"
#include "SPI.h"
#include "math.h"

/******** QUADRATURE DECODER OP CODES AND CONFIGURATION ********/
// LS7366R OP-Codes
const byte CLEAR_COUNTER = 8;  // 00 001 000
const byte CLEAR_STATUS = 48;  // 00 110 000
const byte READ_COUNTER = 96;  // 01 100 000
const byte READ_STATUS = 112;  // 01 110 000
const byte READ_MDR0 = 72;     // 01 001 000
const byte READ_MDR1 = 80;     // 01 010 000
const byte WRITE_MDR0 = 136;   // 10 001 000
const byte WRITE_MDR1 = 144;   // 10 010 000

// configuration vars for the LS7366R
const byte MDR0_Config = 3;    // 0 0 00 00 11
const byte MDR1_Config = 0;    // 00000000
/***************************************************************/

/***************
* Declarations *
***************/
DACClass DAC;

Servo::Servo(int chipSelect, int dacSelect)
{
   // initialize chip select pin on LS7366R
   pinMode(chipSelect, OUTPUT);
   
   //Initialize DAC Object
   DAC.setupSPI(dacSelect); //Set pin 8 as the DAC CS Pin

   // do some object-oriented junk
   _chipSelect = chipSelect;
   digitalWrite(_chipSelect, HIGH);
}


// read position of motor (via LS7366R)
double Servo::position(void)
{
	//Make sure that the result is an unsigned char!! Otherwise the double
	//casting screws stuff up
   unsigned char result[4] = {0, 0, 0, 0};
   long count = 0;
   unsigned char positionRead[1] = {READ_COUNTER};
   _chipTalk(positionRead, 1, result, 4, _chipSelect);

   // turn 4 byte array into one looong var
   count = result[0];
   for (int i = 1; i &lt; 4; i++)
   {
      count = count &lt;&lt; 8;
      count = count | result[i];
   }

   // convert count to position (mm)!
   double mmPos = (double)count/2000.0;   // 0.5um/count

   return mmPos;
}


// apply +-10 V (double) signal to motor
/*void Servo::move(double volts)
{
************************************
  digitalWrite(chipSelectPin, LOW);
  
  SPI.transfer(dacRegTop);
  SPI.transfer(dacRegMid);
  SPI.transfer(dacRegBot);

  digitalWrite(chipSelectPin, HIGH);
  Serial.println("Set DAC A to 10 V");
  **************************************
   // determine if voltage is +, -, or zero
*   if (volts == 0.0)
   {
       // make sure motor is stopped
       digitalWrite(_applyArray[0], LOW);
       digitalWrite(_applyArray[1], LOW);
   }
   else if (volts &lt; 0.0)  
   {
       // make sure motor is moving backward
       digitalWrite(_applyArray[0], LOW);
       digitalWrite(_applyArray[1], HIGH);
   }
   else
   {
       // make sure motor is moving forward!
       digitalWrite(_applyArray[0], HIGH);
       digitalWrite(_applyArray[1], LOW);
   }*
   
   // write particular value to PWM pin; make sure it's a valid byte!
   double val = 0;
   // can't be less than -10 or more than 10
   if (volts &gt; 10.0) { val = 10.0; }
   else if (volts &lt; -10.0) { val = -10.0; }
   else { val = volts; }
   //2.5 is the 2.5V REFIN, 4 is just a set gain
   //The 32767 comes from the max bit value being 32767, which is one less than
   //the 32768 which would ensure perfect 10V... buuut... this should be fine
   val = val/4/2.5*32767;
   // typecast to a byte
   int valInt = (int)(val);
   _chipTell
}*/


// initialize the motor, encoder and DAC chips
void Servo::init(void)
{
   SPI.begin();
   SPI.setBitOrder(MSBFIRST);
   //NOTE: SPI_MODE1 with DAC A on only sometimes produced results
   SPI.setDataMode(SPI_MODE1);
   SPI.setClockDivider(SPI_CLOCK_DIV16);
   // start SPI library for chip communication
   delayMicroseconds(1);

   // configure LS7366R chip
  // _ls7366rConfig();
   delayMicroseconds(200);
   _dacConfig();

   // make sure motor isn't moving!
   /*digitalWrite(_applyArray[0], LOW);
   digitalWrite(_applyArray[1], LOW);
   digitalWrite(_applyArray[3], HIGH);*/

}

/* 
 * ************************************************** *
 *						      *                       *
 * "PRIVATE" FUNCTIONS FOR MAKE BENEFIT GLORIOUS CODE *
 *						      *                       *
 * ************************************************** *
*/

// configure LS7366R upon boot-up
void Servo::_ls7366rConfig(void)
{
  unsigned char MDR0_Val[1];// = {2};
  unsigned char MDR1_Val[1];// = {0};
  
  // setup MDR1 register
  byte MDR1_setup[2] = {WRITE_MDR1, MDR1_Config};
  _chipTell(MDR1_setup, 2, _chipSelect);
  delayMicroseconds(200);
  
  // setup MDR0 register
  byte MDR0_setup[2] = {WRITE_MDR0, MDR0_Config};
  _chipTell(MDR0_setup, 2, _chipSelect);
  delayMicroseconds(200);
  
  // print serially whether the registers were correctly written to (DEBUG)
  unsigned char mdr1Read[1] = {READ_MDR1};
  _chipTalk(mdr1Read, 1, MDR1_Val, 1, _chipSelect);
  delayMicroseconds(200);
  unsigned char mdr0Read[1] = {READ_MDR0};
  _chipTalk(mdr0Read, 1, MDR0_Val, 1, _chipSelect);
  
  // are the registers correct? if not, stall indefinitely
  if (MDR0_Config == MDR0_Val[0] &amp;&amp; MDR1_Config == MDR1_Val[0])
  {
    Serial.println("  &gt;&gt; LS7366R successfully configured.\n");
  }
  else
  {
	  Serial.println("LS7366R MDR0 vals:");
	  Serial.println(MDR0_Config, BIN);
	  Serial.println(MDR0_Val[0], BIN);
	  Serial.println("LS7366R MDR1 vals:");
	  Serial.println(MDR1_Config, BIN);
	  Serial.println(MDR1_Val[0], BIN);
    Serial.println("\n  LS7366R CONFIGURATION FAILURE. Please restart.");
    while (1) { }
  }
}

/*Configure the DAC--currently only turns on DAC A*/
void Servo::_dacConfig(void)
{

  //TODO:  Turn on and check both dacs
  DAC.setOutputRange((uint8_t) ADDRESS_ALL, (uint32_t) UNIPOLAR_5V);
  delayMicroseconds(1);  
  DAC.setPowerControl(PUA); //Power up DAC A
  uint32_t dacCheckPower = DAC.getPowerControl();
  
  //TODO:  Check what the power control is returning, make sure DAC A/B are on

  //TODO:  If the checking worked out, print this stuff..
  Serial.println("  &gt;&gt; DAC power control successfully configured.\n");

  Serial.println("Turned on DAC A.");
}


/*Tell stuff to the chips: the data will be an array of bytes to be sent to the
 * chips with SPI communications, while the size is the number of bytes in said
 * array. The chipSelectPin tells which chip to send this data to*/
void Servo::_chipTell(byte data[], int size, int chipSelectPin)
{
  // take chip select low to let device know we're talking
  int i;
  digitalWrite(chipSelectPin, LOW);
  
  // send bytes to chipSelectPin chip--bytes sent are set up pre-function call,
  // but probably start off with some sort of op code and continue from there
  for (i=0; i&lt;size; i++)
  {
	  SPI.transfer(data[i]);
  }
  
  // take chip select high to officailly end conversation
  digitalWrite(chipSelectPin, HIGH); 
}

void Servo::_chipTalk(unsigned char readCmd[], int inSize, unsigned char *dataRead, int outSize, int chipSelectPin)
{
  // take chip select low to let device know we're talking
  int i;
  Serial.println("WRITING:");
  digitalWrite(chipSelectPin, LOW);
  
  // send whatever bytes to initiate read--size will be stored in inSize and
  // that's how we know how many to send
  /*for (i=0;i&lt;inSize;i++)
  {
	  Serial.println(readCmd[i], BIN);
	  SPI.transfer(readCmd[i]);
  }*/
  SPI.transfer(readCmd[0]);
  SPI.transfer(readCmd[1]);
  SPI.transfer(readCmd[2]);


  digitalWrite(chipSelectPin, HIGH);

  delayMicroseconds(10);

	Serial.println("Press that button again!");
	delay(2000);
  digitalWrite(chipSelectPin, LOW);
  
  // here we just grab the data and store it in dataRead, we'll let the function
  // caller decide how much data needs to be written depending on what read code
  // was sent, and this will be stored in outSize
/*  for (i=0; i&lt;outSize; i++)
  {
	  // here we're going to assume that all chips giving readback data don't
	  // really care what's being sent to them concurrently, but this may have
	  // to change
	  dataRead[i] = SPI.transfer(0x180000);
}*/
	  dataRead[0] = SPI.transfer(0x18);
	  dataRead[1] = SPI.transfer(0x00);
	  dataRead[2] = SPI.transfer(0x00);
  
  // take chip select high to officially end conversation
  digitalWrite(chipSelectPin, HIGH);
}



</pre></body></html>
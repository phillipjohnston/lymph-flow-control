/*
 * Servo.cpp - Library for interacting with a
 * 	     Parker MX80L linear stage
 * 
 * Created by Jeff Kornuta, September 18, 2011.
 * Released into the public domain.
 *
*/

#include "WProgram.h"
/*
  
  EXTERNAL FUNCTIONS FOR MAKE BENEFIT GLORIOUS CODE
  
*/
#include "Servo.h"
#include "HardwareSerial.h"
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

/*************** DAC OP CODES AND CONFIGURATION ****************/
/*Output range 24 bits*/
const byte OUT_RANGE_TOP = 12; //0 0 001 100 -- output range for both DACs
const byte OUT_RANGE_MID = 0; //0000 0000 -- don't care
const byte OUT_RANGE_BOT = 0; //00000 000 -- +5 V

/*Power on DAC A*/
const byte POWER_CONT_TOP = 16; //0 0 010 000 -- power control
const byte POWER_CONT_MID = 0; //00000 000 -- don't care for write
const byte POWER_CONT_BOT = 1; //0000 00 001 -- turn on DAC A


/*Set DAC A Voltage to 5 V*/
const byte DAC_REG_TOP = 0; //0 0 000 000 -- DAC register A
const byte DAC_REG_MID = 255; //1111 1111 -- top of 32767
const byte DAC_REG_BOT = 0; //1111 1111 -- bottom of 32767

/*Load DAC registers?*/
const byte CONT_REG_TOP = 25; //0 0 011 001 -- change settings
const byte CONT_REG_MID = 0; //0000 0000 -- don't care
const byte CONT_REG_BOT = 12; //0000 1000 -- TSD enable and clamp disable
const byte CONT_REG_TOP2 = 25; //0 0 011 001 -- CLR function
const byte CONT_REG_MID2 = 0; //0000 0000 -- don't care
const byte CONT_REG_BOT2 = 2; //0000 0010 -- don't care
/***************************************************************/


Servo::Servo(int chipSelect, int dacSelect)
{
   // initialize chip select pin on LS7366R
   pinMode(chipSelect, OUTPUT);
   
   //Initialize DAC Object
   pinMode(dacSelect, OUTPUT);


   // do some object-oriented junk
   _chipSelect = chipSelect;
   digitalWrite(_chipSelect, HIGH);
   _dacSelect = dacSelect;
   digitalWrite(_dacSelect, HIGH);

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
   for (int i = 1; i < 4; i++)
   {
      count = count << 8;
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
   else if (volts < 0.0)  
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
   if (volts > 10.0) { val = 10.0; }
   else if (volts < -10.0) { val = -10.0; }
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
  if (MDR0_Config == MDR0_Val[0] && MDR1_Config == MDR1_Val[0])
  {
    Serial.println("  >> LS7366R successfully configured.\n");
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
  /*SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV128);*/
  int i;
  int die = 0;

  /*Set the output range for both DACs to +5V*/    
/*  byte dacOutRange[3] = {OUT_RANGE_TOP, OUT_RANGE_MID, OUT_RANGE_BOT};
  _chipTell(dacOutRange, 3, _dacSelect);
//  dacOutRange[0] = dacOutRange[0] | 128;
  delayMicroseconds(200);

  unsigned char dacCheckOutCmd[3] = {OUT_RANGE_TOP | 128, OUT_RANGE_MID, OUT_RANGE_BOT};
  unsigned char dacCheckOut[3];
  _chipTalk(dacCheckOutCmd, 3, dacCheckOut, 3, _dacSelect);
  for (i=0;i<3;i++)
  {
	  Serial.print("DAC values at ");
	  Serial.print(i);
	  Serial.println(":");
	  Serial.println(dacOutRange[i], BIN);
	  Serial.println(dacCheckOut[i], BIN);
	  if (dacCheckOut[i] != dacOutRange[i])
	  {
		  die = 1;
	  }
  }*/

  /*if (die)
  {
		  Serial.println("\n  DAC OUTPUT RANGE CONFIGURATION FAILURE. Please restart.");
		  while (1) { }
  }*/

  /*Serial.println("  >> DAC output range successfully configured.\n");

  Serial.println("Set output range to +5V");

  delay(10);*/

  /*Set TSD and current clamp*/
 /* byte dacContReg[3] = {CONT_REG_TOP, CONT_REG_MID, CONT_REG_BOT};
  _chipTell(dacContReg, 3, _dacSelect);
  dacContReg[0] = dacContReg[0] | 128;
  delayMicroseconds(200);

  unsigned char dacCheckContCmd[3] = {CONT_REG_TOP | 128, CONT_REG_MID, CONT_REG_BOT};
  unsigned char dacCheckCont[3];
  _chipTalk(dacCheckContCmd, 3, dacCheckCont, 3, _dacSelect);
  for (i=0;i<3;i++)
  {
	  Serial.print("DAC values at ");
	  Serial.print(i);
	  Serial.println(":");
	  Serial.println(dacContReg[i], BIN);
	  Serial.println(dacCheckCont[i], BIN);
	  if (dacCheckCont[i] != dacContReg[i])
	  {
		  die = 1;
	  }
  }*/

  /*if (die)
  {
		  Serial.println("\n  DAC CONTROL REGISTER CONFIGURATION FAILURE. Please restart.");
		  while (1) { }
  }*/

  /*Turn on DAC A*/
  Serial.println("Acquire!!");
  delay(2000);
  byte dacPowerCont[3] = {POWER_CONT_TOP, POWER_CONT_MID, POWER_CONT_BOT};
  _chipTell(dacPowerCont, 3, _dacSelect);
  dacPowerCont[0] = dacPowerCont[0] | 128;
  delayMicroseconds(200);


  unsigned char dacCheckPowerCmd[3] = {POWER_CONT_TOP | 128, POWER_CONT_MID, POWER_CONT_BOT};
  unsigned char dacCheckPower[3];
  Serial.println("Press Single Seq!!!!");
  delay(1000);
  _chipTalk(dacCheckPowerCmd, 3, dacCheckPower, 3, _dacSelect);
  for (i=0;i<3;i++)
  {
	  Serial.print("DAC values at ");
	  Serial.print(i);
	  Serial.println(":");
	  Serial.println(dacPowerCont[i], BIN);
	  Serial.println(dacCheckPower[i], BIN);
	  if (dacCheckPower[i] != dacPowerCont[i])
	  {
		  die = 1;
	  }
  }
  
  /*if (die)
  {
	  Serial.println("\n  DAC POWER CONTROL CONFIGURATION FAILURE. Please restart.");
	  while (1) { }
  }*/

  Serial.println("  >> DAC power control successfully configured.\n");

  Serial.println("Turned on DAC A.");

  delay(10);

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
  for (i=0; i<size; i++)
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
  /*for (i=0;i<inSize;i++)
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
/*  for (i=0; i<outSize; i++)
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




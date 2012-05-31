/*
 * Servo.cpp - Library for interacting with a
 * 	     Parker MX80L linear stage
 * 
 * Created by Jeff Kornuta, September 18, 2011.
 * Modified by Phillip Johnston, 22 May 2012
 * Released into the public domain.
 *
*/

#include "WProgram.h"
#include "Servo.h"
#include "LS7366R.h"
#include "HardwareSerial.h"
#include "dac5752.h"
#include "SPI.h"
#include "math.h"

/**********************
* Object Declarations *
**********************/
DACClass DAC;
LS7366RClass QD;

/****************************
* Configuration Definitions *
****************************/
// configuration vars for the LS7366R
#define MDR0_CONFIG 3    // 0 0 00 00 11
#define MDR1_CONFIG 0    // 00000000


/***********************
* Function Definitions *
***********************/
Servo::Servo(int chipSelect, int dacSelect)
{
   // initialize SPI for LS7366R
   QD.setCSPin(chipSelect);
   
   //Initialize SPI for DAC
   DAC.setCSPin(dacSelect);
}

// read position of motor (via LS7366R)
double Servo::position(void)
{
	QD.setupSPI(); //Ensure SPI is configured for this device
	return QD.readPosition();
}

// initialize the motor, encoder and DAC chips
void Servo::init(void)
{
   SPI.begin();
   _ls7366rConfig();
   _dacConfig();
}

void Servo::_ls7366rConfig(void)
{
  uint8_t MDR0_Val;
  uint8_t MDR1_Val;
  
  QD.setupSPI(); //Ensure SPI is configured for this device
  
  QD.setMDR1Reg(MDR1_CONFIG);
  //delayMicroseconds(200);
  
  QD.setMDR0Reg(MDR0_CONFIG);
  //delayMicroseconds(200);
  
  //Now we will get the register vals to make sure everything is kosher
  MDR1_Val = QD.getMDR1Reg();  
  //delayMicroseconds(200);
  MDR0_Val = QD.getMDR0Reg();
  
  if (MDR0_CONFIG == MDR0_Val && MDR1_CONFIG == MDR1_Val)
  {
    Serial.println("  >> LS7366R Quadrature Decoder successfully configured.\n");
  }
  else
  {
	  Serial.println("LS7366R MDR0 vals:");
	  Serial.println(MDR0_CONFIG, BIN);
	  Serial.println(MDR0_Val, BIN);
	  Serial.println("LS7366R MDR1 vals:");
	  Serial.println(MDR1_CONFIG, BIN);
	  Serial.println(MDR1_Val, BIN);
      Serial.println("\n*** LS7366R CONFIGURATION FAILURE! ***");
      while (1) { }
  }
}

void Servo::_dacConfig(void)
{
  DAC.setupSPI(); //Ensure SPI is configured for this device

  //Turn on and check both dacs
  DAC.setOutputRange((uint8_t) DAC_ALL, (uint32_t) UNIPOLAR_5V);
  DAC.setPowerControl(POWER_DAC_ALL); //Power up DAC A
  
  //Check the value of the power control register.
  uint32_t dacCheckPower = DAC.getPowerControl();
  
  if( (dacCheckPower & 0xF) == POWER_DAC_ALL)
  {
	Serial.println("DAC power control successfully configured.\n");
    Serial.println("Turned on DAC A & DAC B.");
  }
  else
  {
    Serial.println("DAC5752 Power Control Reg Values:");
	Serial.print("Config Value: ");
	Serial.print(POWER_DAC_ALL, BIN);
	Serial.print("\nRegister Value: ");
	Serial.print(dacCheckPower & 0xF, BIN);
    Serial.println("\n\n*** DAC5752 CONFIGURATION FAILUE! ***");
	while(1) {} //Wait indefinitely in error loop
  }
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

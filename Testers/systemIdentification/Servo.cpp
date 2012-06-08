/*
 * Servo.cpp - Library for interacting with a
 *          Parker MX80L linear stage
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
        while (1);
    }
}

void Servo::_dacConfig(void)
{
    DAC.setupSPI(); //Ensure SPI is configured for this device

    //Turn on and check both dacs
    DAC.setOutputRange((uint8_t) DAC_ALL, (uint32_t) BIPOLAR_5V);
    DAC.setPowerControl(POWER_DAC_A); //Power up DAC A

    //Check the value of the power control register.
    uint32_t dacCheckPower = DAC.getPowerControl();

    if( (dacCheckPower & 0xF) == POWER_DAC_A)
    {
        Serial.println("DAC power control successfully configured.\n");
        Serial.println("Turned on DAC A & DAC B.");
    }
    else
    {
        Serial.println("DAC5752 Power Control Reg Values:");
        Serial.print("Config Value: ");
        Serial.print(POWER_DAC_A, BIN);
        Serial.print("\nRegister Value: ");
        Serial.print(dacCheckPower & 0xF, BIN);
        Serial.println("\n\n*** DAC5752 CONFIGURATION FAILUE! ***");
        while(1) {} //Wait indefinitely in error loop
    }
}

// apply +-10 V (double) signal to motor
void Servo::move(double volts)
{
    //For testing, we're gonna do +-5V. 
    //Get ratio between volts and 5.0
    float scale = volts / 5.0;
    
    int16_t value = (int16_t) (scale * (float) 0x7FFF); //Scale * value for 5V.
    DAC.setValue(DAC_A, value); //Set the DAC output
}
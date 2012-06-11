/*
 * HSC.cpp - Library for interacting with a
 *           Honeywell HSC Series pressure sensor
 *           Range: -1 to 1 psig
 *           I2C Address: 0x28
 *
 * Usage: HSC pSensor (channel)
 *      The argument channel is an integer 1 - 4 to choose the 
 *      multiplexer channel the sensor is connected to (only 1 and 2
 *      should be connected). A value of 0 specifies that the sensor
 *      is connected directly (can be used for debugging).
 *
 * Created by Jeff Kornuta, June 8, 2012.
 * Released into the public domain.
 *
 */

#include "WProgram.h"
#include "HSC.h"
#include "math.h"
#include "Wire.h"

/****************************
* Configuration Definitions *
****************************/
// Sensor address for writing and reading
#define pressAddr 0x28

// I2C multiplexer address
#define mpxAddr 0x70

/***********************
* Function Definitions *
***********************/
HSC::HSC(int sensorChannel)
{
    // Set protected variable for setting multiplexer channel
    selChan = sensorChannel;
}

// Return pressure from sensor in cmH2O
double HSC::pressure(void)
{
    // Are we really connected through the multiplexer?
    // If so, open up the appropriate multiplexer channel
    if ( selChan != 0 )
    {
        Wire.beginTransmission(mpxAddr);
        Wire.send(selChan);
        uint8_t error = Wire.endTransmission();

        // Let us know if there was a transmission error
        if ( error != 0 )
        {
            Serial.print("Multiplexer transmission error: ");
            Serial.println(error, DEC);
        }
    }

    // Grab pressure value  
    Wire.requestFrom((int) pressAddr, 2);
    uint8_t pressHB = Wire.receive();
    uint8_t pressLB = Wire.receive();
  
    // Turn bytes into one integer
    uint16_t pressure = (uint16_t) pressHB;
    pressure = ((pressure << 8) | ((uint16_t) pressLB));
  
    // Convert to psig then cmH2O, after finding output % (see datasheet)
    double output = ((double) pressure)/16383.0;
    double psig = 2.5*(output - 0.1) - 1.0;
    double cmH2O = 70.30696*psig;

    return cmH2O;
}



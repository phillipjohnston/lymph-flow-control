/*
 * HSC.h - Library for interacting with a
 *              Honeywell HSC Series pressure sensor
 *              Range: -1 to 1 psig
 *              I2C Address: 0x28
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

#ifndef HSC_H
#define HSC_H

/********************
* Class Declaration * 
********************/
class HSC 
{
    public:
        HSC(int sensorChannel);
        double pressure(void);

    protected:
        uint8_t selChan;
};

#endif // HSC_H

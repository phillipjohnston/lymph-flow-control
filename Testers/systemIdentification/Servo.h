/*
 * Servo.h - Library for interacting with a
 * 	     Parker MX80L linear stage
 * 
 * Created by Jeff Kornuta, September 18, 2011.
 * Modified by Phillip Johnston, 25 May 2012.
 * Released into the public domain.
 *
*/

#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

/********************
* Class Declaration * 
********************/
class Servo
{
    public:
        Servo(uint16_t quadDecCS, uint16_t dacCS, uint8_t dac);
        void init(void);
        double position(void);
        void move(double volts);

    private:
        void _ls7366rConfig(void);
        void _dacConfig(uint8_t power_setting);
        uint8_t selected_dac;
};

#endif //SERVO_H

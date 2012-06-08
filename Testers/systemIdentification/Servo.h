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

/********************
* Class Declaration * 
********************/
class Servo
{
    public:
        Servo(int chipSelect, int dacSelect);
        void init(void);
        double position(void);
        void move(double volts);

    private:
        void _ls7366rConfig(void);
        void _dacConfig(void);
};

#endif //SERVO_H

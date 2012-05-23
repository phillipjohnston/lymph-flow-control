
<!-- saved from url=(0070)https://raw.github.com/TheWorldJoker/lymph-flow-control/master/Servo.h -->
<html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"></head><body><pre style="word-wrap: break-word; white-space: pre-wrap;">/*
 * Servo.h - Library for interacting with a
 * 	     Parker MX80L linear stage
 * 
 * Created by Jeff Kornuta, September 18, 2011.
 * Released into the public domain.
 *
 *
 * USAGE:
 *  Initialize instance of class with
 *
 *      Servo motor1(chipSelect, applyArray);
 *
 *  where chipSelect is the slave select pin for the LS7366R and
 *  applyArray is a four-byte array containing the pin numbers for
 *
 *      AIN1, AIN2, APWM, ASTBY
 *
 *  for the Toshiba TB6552 H-bridge controller.
 *
 *  Once an instance has been created, initialize the motor with
 *
 *      motor1.init();
 *
 *  The position double (in mm) may be returned with
 *
 *      position = motor1.position();
 *
 *  A voltage double (+- 10 V) may be applied to the motor with
 *
 *      motor1.apply(volts);
 *
 *
*/

#ifndef Servo_h
#define Servo_h

#include "WConstants.h"

class Servo
{
   public:
      Servo(int chipSelect, int dacSelect);
      void init(void);
      double position(void);
      void move(double volts);
   private:
      int _chipSelect;
      int _dacSelect;
      /*char _applyArray[4];*/
      void _chipTalk(byte readCmd[], int inSize, unsigned char *dataRead, int outSize, int chipSelectPin);
      void _chipTell(byte data[], int size, int chipSelectPin);
      void _ls7366rConfig(void);
      void _dacConfig(void);
};

#endif
</pre></body></html>
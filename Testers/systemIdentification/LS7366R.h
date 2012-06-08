/*
* LS7366R Class
* Initial functionality created by Jeff Kornuta
* Packaged into class and expanded by Phillip Johnston
*
* 25 May 2012
*
* LS7366R.h
*/

#ifndef _LS7366R_H
#define _LS7366R_H

#include <stdint.h>
#include "SPIDevice.h"

/********************
* Define Statements *
********************/
// LS7366R OP-Codes
#define CLEAR_COUNTER 8
#define CLEAR_STATUS 48
#define READ_COUNTER 96
#define READ_STATUS 122
#define READ_MDR0 72
#define READ_MDR1 80
#define WRITE_MDR0 136
#define WRITE_MDR1 144

/********************
* Class Declaration * 
********************/
class LS7366RClass : public SPIDevice
{
    public:
        LS7366RClass();
        ~LS7366RClass();
        void setupSPI();
        double readPosition();
        void setMDR0Reg(uint8_t config_val);
        void setMDR1Reg(uint8_t config_val);
        uint8_t getMDR0Reg();
        uint8_t getMDR1Reg();
};
#endif
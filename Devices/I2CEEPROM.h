/*
  I2CEEPROM.h - Library for writing to an external EEPROM
  module (24LC64).
  Created by Zachary Danielak, 10 Feb 2012.
  Use restricted to research applications within the Laboratory of 
  Lymphatic Biology and Bioengineering (LLBB) of the Georgia Institute 
  of Technology.
  */
#ifndef I2CEEPROM_h
#define I2CEEPROM_h

#include "WConstants.h"
#include "WProgram.h"
#include <Wire.h>

class I2CEEPROM
{
    public:
        I2CEEPROM();
        int getFreq();
        int getDuration();
        int getSamples();
        int getMotorVal(int i);
        void eepromwrite();
        void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data );    
        byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress );
    private:
        int len;
        char writeBytes;
        String somedata;
        char in;
        int i;
        int addr;
        byte b;
        int deviceaddress;
        unsigned int eeaddress;
        byte data;
        int rdata;
        int outs;
        String toChar;
        char toParse;
        int j;
        int vals[];
        int x;
};

#endif

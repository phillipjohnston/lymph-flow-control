/*
  I2CEEPROM - Library for writing to an external EEPROM
 module (24LC64).
 Created by Zachary Danielak, 10 Feb 2012.
 Use restricted to research applications within the Laboratory of 
 Lymphatic Biology and Bioengineering (LLBB) of the Georgia Institute 
 of Technology.
 Updated until: 23 April 2012
 */

#include "WProgram.h"
#include <Wire.h> //I2C library
#include "I2CEEPROM.h"
#include "HardwareSerial.h"

I2CEEPROM::I2CEEPROM()
{
    Wire.begin(); // initialise the EEPROM connection
    Serial.begin(115200); // initialise USB connection
}

/*
 * This is the intitialization function responsible for recieving experimental parameters and motor movement data.
 * The first int recieved is the frequency with which the Arduino will update motor position.
 * The second int recieved is the duration of the experiment in minutes.
 * The remaining data, until the character 'z', is the motor movement data.
 *
 * In order to loop through these values and continually print to the motor,
 * 	it is recommended that you do so through the use of the provided 
 * 	getter methods from a loop in Arduino rather than modifying this
 * 	code directly (unless otherwise needed).
 * */
int x = 0;

void I2CEEPROM::eepromwrite()
{
    while(Serial.available() == 0){ 
        // Both LEDs go high, wait for user input (USB)
        pinMode(43, OUTPUT);	  // Pin 43 is exclusively reserved for LED modulation, which is why I use it for debugging
        digitalWrite(43, HIGH);
    }
    digitalWrite(43, LOW);	  // LEDs go low, indicates that user input has been recieved
    char in = Serial.read();	
    int decPlace = 1;
    String curNum = "";
    int vals[1050]; 		// Stores motor movement data - I don't anticipate a sample period of over 1000 samples
    int freq = 0;   		// init motor update requency variable
    int min = 0;			// init duration variable

    
    //This loop reads in serial data, parsing each int by a linefeed, //and terminating the loop when it recieves 'z'.                                                               
    while(in != 'z'){
        if(Serial.available() > 0 ){

        if(in != 10)
        {
            // LF not recieved, so this char must be part of an int
            curNum += in;
            decPlace++;
        }
        if(in == 10){		// LF recieved, parse char array for int
           char toInt[decPlace];
           curNum.toCharArray(toInt, decPlace);
               vals[x] = atoi(toInt);
           if(x == 0){
            freq = vals[x];	// assigns the first int to be the frequency
            }
            if(x == 1){
                min = vals[x];  // assigns the second int to be the duration
            }
           x++;
           curNum = "";
           decPlace = 1;
        }
        in = Serial.read();
        }
    }
    delay(100);
    int i = 0;		// vals counter
    int ii = 0;  	// eeprom counter (needed separately because each int has to be deconstructed into 2 bytes)
    int big;		// big-endian half of int
    int low;		// little-endian half of int

    digitalWrite(43, HIGH);

    //This loop writes the data to the EEPROM.     
    while(i<(x)){  
        big = (int)(vals[i] >> 8); // These lines split the recieved int into two bytes to write to EEPROM
        low = (int)(vals[i] & 0xFF);
        i2c_eeprom_write_byte(0x50, ii, big); // write 1st half of int to EEPROM
        delay(8);
        i2c_eeprom_write_byte(0x50, ii+1, low); // write 2nd half of int to EEPROM 
        ii += 2;
        i++;
        delay(8); // The 8ms delays seem to be necessary to prevent overflow errors
    }
	
    // Toggled LEDs indicate EEPROM writing is soon to begin
    digitalWrite(43, LOW);

    delay(10);
    int addr=0; //first addresss in EEPROM
    int addr2=1;
    byte b = i2c_eeprom_read_byte(0x50, 0); // access the first address from the memory
    byte c = i2c_eeprom_read_byte(0x50, 1);
    String toChars = "";
    int q =0;
    int j = 1; // Counter to represent length of a number
    int quasint = 0; // Sort of a semi error check, quasint is the sum of abs vals. If using this for debugging, be sure to print it out
  
    /*
    This loop reads the ints back from EEPROM and reconstructs a two-byte word from two separate bytes. For
    diagnostic purposes, we Serial.println(reconstructed) to see each int that the EEPROM read. For the 
    purposes of the experiment (once debugging is done), this should be commented out.
    Note, quasint is a sort of "checksum" that could be implemented rather than verbosely printing out each
    integer. In order for it to work, though, you'd need to do the same thing in LabVIEW, and have it compare the
    values. quasint is just the sum  of absolute values of the ints. It's handy for telling you if an int was
    omitted from a read/write operation, however, to see WHICH int was omitted, verbose printing is necessary.
    */
    while(addr<ii){
        int tempB = (b<<8);
        int tempC = (c&0xFF);
        int reconstructed = tempB + tempC;	 // Add together the two bytes to form an int
        if(reconstructed > 32766){		// This if statement ensures proper 2's compliment form because
            reconstructed = reconstructed - 65536; // 		the int loses its sign when deconstructed (so a negative number would appear to be above 2^15)
            quasint += reconstructed*(-1);
        }else{
            quasint += reconstructed;
        }
        Serial.println(reconstructed);	// --Only necessaryfor debugging. If no print statements needed, adjust read-in count in LabVIEW
        delay(15);			// This delay seems to only be necessary to prevent read errors
        addr +=2;			// 		when printing ints back to LabVIEW for error checking
        addr2 += 2;
        b = i2c_eeprom_read_byte(0x50, addr);
        c = i2c_eeprom_read_byte(0x50, addr2);
    } 
	// LEDs low, indicate done initializing
    digitalWrite(43, HIGH);

    //Serial.println(quasint); -- This is the semi "checksum"
    //Serial.println("                                                                                          ");
    // The above is just extra blank space, since LabVIEW only reads in a set number of characters, so
    //			 this provides some buffer space in case we didn't print out enough
    Serial.end();
    Wire.endTransmission();
}


int I2CEEPROM::getFreq()
{// Getter for motor update frequency variable
	Wire.begin();
	byte b = i2c_eeprom_read_byte(0x50, 0);
	byte c = i2c_eeprom_read_byte(0x50, 1);
	int tempB = (b<<8);
	int tempC = (c&0xFF);
	int reconstructed = tempB + tempC;
    Wire.endTransmission();	
	return reconstructed;
}

int I2CEEPROM::getDuration()
{  // Getter for experiment duration variable
	Wire.begin();
	byte b = i2c_eeprom_read_byte(0x50, 2);
	byte c = i2c_eeprom_read_byte(0x50, 3);
	int tempB = (b<<8);
	int tempC = (c&0xFF);
	int reconstructed = tempB + tempC;
 	Wire.endTransmission();	
	return reconstructed;
}

int I2CEEPROM::getSamples()
{  // Getter for the number of samples needed to read
    int samples = x-2;
    return samples;
}

int I2CEEPROM::getMotorVal(int i)
{ // Getter for an individual motor value
    byte b = i2c_eeprom_read_byte(0x50, i+4);
    byte c = i2c_eeprom_read_byte(0x50, i+5);
    int tempB = (b<<8);
    int tempC = (c&0xFF);
    int reconstructed = tempB + tempC;       
    if(reconstructed > 32766)
    {		
        reconstructed = reconstructed - 65536;
    }
    return reconstructed;
}

/*
 * This function, and the one below, are from open-source code available under Arduino.cc
 * They simplify I2C communication by making use of the Wire library
 * deviceaddress specifies the physical EEPROM module
 * eeaddress specifies the byte location in memory you're reading from or writing to
 * data is a single byte you're writing.
 *
 * They are flawless. Do not change them.
 * */
void I2CEEPROM::i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data ) 
{
    int rdata = data;
    Wire.beginTransmission(deviceaddress);
    Wire.send((int)(eeaddress >> 8)); // MSB
    Wire.send((int)(eeaddress & 0xFF)); // LSB
    Wire.send(rdata);
    Wire.endTransmission();
}

byte I2CEEPROM::i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress ) 
{
    byte rdata = 0xFF;
    Wire.beginTransmission(deviceaddress);
    Wire.send((int)(eeaddress >> 8)); // MSB
    Wire.send((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,1);
    if (Wire.available())
    {    
        rdata = Wire.receive();
    }
    return rdata;
}




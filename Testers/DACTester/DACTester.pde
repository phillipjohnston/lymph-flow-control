//#include <SPI.h>
#include <math.h>
#include "dac5752.h"
#include <stdint.h>

#include "SPI.h"

/*Power on DAC A*/
/*The following variables are the three bytes needed to power on DAC A. The
 * first variable selects the power control register (middle 010), the middle
 * byte is a don't care byte, and the bottom byte selects DAC A to be turned on
 * (first bit high; if we want DAC B on, we set third bit high; and if we want
 * both on, we set first and third bit high)*/

/*
* Function Prototypes
*/
void serialInit();

/*
* Declarations
*/
DACClass DAC;

/*
* Setup()
*/
void setup()
{
   serialInit();
   
   DAC.setupSPI(8); //Set pin 8 as the DAC CS Pin
   Serial.println("DAC Initialized.");
  
   Serial.println("Setting DAC Control Registers.");
   DAC.setControl(); //Enable thermal shutdown and clamping
   
   Serial.println("Setting DAC Ouput Range.");
   DAC.setOutputRange((uint8_t) ADDRESS_B, (uint32_t) UNIPOLAR_5V);
  
  delayMicroseconds(1);

  /* As with the output register, three bytes are being set here to give the DAC
   * its expected 24bits, and in doing so turn on DAC A*/
   
   Serial.println("Setting DAC Power Control");
  DAC.setPowerControl(PUB); //Power up DAC A
  delayMicroseconds(10);
  
  delayMicroseconds(1);
  
  /* After sending the initial readback instruction, the next instruction send
   * should clock out the data. It's suggested that NOP instruction (0x180000)
   * be sent for this readback, though I assume processing can be done in
   * parallel and if a more important instruction needs to concurrently be sent,
   * that can be done. However, until we get the DAC working, we'll stay on the
   * suggested side. One byte gets clocked back per byte send, making three
   * total bytes returned*/
   Serial.println("Reading DAC Power Control.");
   uint32_t dacCheckPower = DAC.getPowerControl();
   

   
   //TODO:  Check that the stuff was set sucessfully.  Mask and compare with
   // the returned data
   
   
   //DAC.setValue(ADDRESS_A, 0xFFFF); //Set equal to the highest output voltage: close to 5V in this case
   //Serial.println("Setting DAC A to 5 V");
   delayMicroseconds(10);
   Serial.println("Setting DAC to 5V");
   DAC.setValue(ADDRESS_B, 0xFFFF);
   
}
 
 /* The loop() is empty because all the relevant setup and output initialization is
 * being done once in the setup() function. That being said, it could
 * conceivable hold code that change the DAC outputs or turn them on or off
 * depending on what instruction is sent to the Uno32. Evaluation of this sort
 * of interaction will have to be researched once the DAC.actually.WORKS! :(*/
void loop()
{

}

void serialInit()
{
  /* Begin serial communications--mostly for debugging so we can see what's
   * going on*/
  Serial.begin(115200);
  Serial.println("Initializing serial communication...");
}

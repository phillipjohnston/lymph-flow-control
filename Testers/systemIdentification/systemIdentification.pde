/*
* System Identification Tester
*
* Created by Phillip Johnston
* 8 June 2012
*
* This program will output a chirp on the DAC
* ranging from -5V to 5V @ frequencies 0.1 Hz to ~100Hz.
*
* This program will also make readings of the quadrature
* decoder's position
*
* Output will be to serial terminal.
*/

#if defined(__PIC32MX__)
    #include <p32xxxx.h>    /* this gives all the CPU/hardware definitions */
    #include <plib.h>       /* this gives the i/o definitions */
#endif

#include "Servo.h"
#include "SPI.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "dac5752.h"

/********************
* Define Statements *
********************/
#define POINTS_PER_CYCLE 200
#define POINTS_UNTIL_FREQ_INCR 200
#define CLOCK_FREQ 40000000
#define TIMER_PRESCALE 2
#define PI 3.14159265
//#define PRESCALED_TIMER_FREQ CLOCK_FREQ / TIMER_PRESCALE
#define PRESCALED_TIMER_FREQ 20000000

//#define SYS_IDENT_DEBUG

#ifdef SYS_IDENT_DEBUG
#define debugf(msg) Serial.print msg
#else
#define debugf(msg) 
#endif

/************************
* Function Declarations *
************************/
void configureTimer2();
void printTMRVals();

/************************
* Variable Declarations *
************************/
Servo servo (9, 8, DAC_A); //QD cs = 9, Dac cs = 8
bool apply_value = false;
double delta_t;
double v_app;
double multiplier;
double position;
uint8_t waveform_counter; //Counting the number of samples per cycle.
uint8_t freq_change_counter;
uint8_t f_index = 0;

/******************
* Frequency Table *
******************/
/*
const float frequencies[] = {0.1, 0.2, 0.5, 0.75, 1.0, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 
                           7.5, 10, 11, 12, 13, 14, 15, 17.5, 20, 25, 30, 40, 50, 60,
                           70, 80, 90, 100};
                           */
const float frequencies[] = {1.0, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 
                           7.5, 10, 11, 12, 13, 14, 15, 17.5, 20, 25, 30, 40, 50, 60,
                           70, 80, 90, 100};
                           
//#define NUM_FREQUENCIES 31
#define NUM_FREQUENCIES 27

/***********************
* Function Definitions *
***********************/

void setup() 
{
    Serial.begin(115200);
    
    servo.init(); //Set up SPI and configure QD and DAC
    
    configureTimer2(frequencies[f_index]);
    
    Serial.println("====Begining System Test====");
}

void loop()
{
    if(apply_value)
    {
        //This gives us a signed scaling value in the range of [-1, 1] continuous
        multiplier = sin(2 * PI * waveform_counter / POINTS_PER_CYCLE); 
        v_app = 10.0 * multiplier;
        if(v_app < 0)
        {
           v_app *= -1; 
        }
        servo.move(v_app); //Apply a new voltage
        position = servo.position(); //Get the motor position
        
        Serial.print("\nV_App: ");
        Serial.print(v_app);
        Serial.print(", pos: ");
        Serial.print(position);
        Serial.print(", dt: ");
        Serial.print(delta_t, 8);
        
        //House keeping 
        freq_change_counter++;
        waveform_counter++;
        
        if(waveform_counter == POINTS_PER_CYCLE)
        {
            debugf(("Reset Waveform Counter\n"));
            waveform_counter = 0; //I don't want to worry about weird data due to an overflow
        }
        
        if(freq_change_counter == POINTS_UNTIL_FREQ_INCR)
        {
          
            debugf(("Moving to a new frequency\n"));
            freq_change_counter = 0;
            f_index++;
            
            if(f_index >= NUM_FREQUENCIES)
            {
                CloseTimer2();
                Serial.println("\n\n\n====END OF SYSTEM TEST====");
                while(1); //Hang up here
            }
            
            configureTimer2(frequencies[f_index]);//++f_index]);
        }
        
        apply_value = false;

        //ConfigIntTimer23((T23_INT_ON | T23_INT_PRIOR_3));        
    }
    
        //printTMRVals();
    
}

void configureTimer2(float freq)
{
    uint32_t t_period;  //For the PIC32 PR2 register
    //CloseTimer23();
    
    T4CON = 0x0;
    T5CON = 0x0;
    
    //Using the desired frequency, clock frequency,
    //and number of vals per cycle, we can get the
    //Timer period.
    t_period = (uint32_t) floor((float) PRESCALED_TIMER_FREQ / (freq * (float) POINTS_PER_CYCLE)); 
    
    delta_t = (float) t_period * (1.0 / (float) PRESCALED_TIMER_FREQ);
    
    debugf(("The new timer period is: "));
    debugf((t_period));
    debugf(("\n"));
    
    //Setup Timer2 (16 bit) to be our interrupt source
    //OpenTimer23( T23_ON | T23_PS_1_4 | T23_SOURCE_INT, t_period);
    //OpenTimer23(T2_ON & T2_GATE_OFF & T2_PS_1_4 & T2_32BIT_MODE_ON & T2_IDLE_STOP & T2_SOURCE_INT, t_period); 
    //ConfigIntTimer23((T23_INT_ON | T23_INT_PRIOR_3));

    
    //T4CONSET = 0x18; //Prescaler 1:2, internal peripheral source
    T4CONSET = 0x28; //Prescaler 1:4, internal source
    
    TMR4 = 0;
    PR4 = t_period;
     //#error "Put a debug statement here so you can see the new PR values and t_period values each time."
    
    IPC5SET = 0x5; //Priority level 1, sub-priority level 1
    IFS0CLR = 0x00100000;
    IEC0SET = 0x00100000;
    
    printTMRVals();
    
    T4CONSET = 0x8000; //Start timer 45

}

#ifdef __cplusplus
extern "C" {
#endif
void __ISR(_TIMER_5_VECTOR,IPL3AUTO) write_handler( void)
{
    apply_value = true;
    debugf(("Interrupt Entered\n"));
    IFS0CLR = 0x00100000;  // Clear interrupt flag
    printTMRVals();
}

#ifdef __cplusplus
}
#endif


void printTMRVals()
{
    debugf(("TMR5: "));
    debugf((TMR5));
    debugf(("\nTMR4: "));
    debugf((TMR4));
    debugf(("\n")); 
}

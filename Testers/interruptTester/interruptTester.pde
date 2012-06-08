/* For AVR */
#if defined(__AVR__)
    #include <avr/io.h>
#endif

/* For PIC32 */
#if defined(__PIC32MX__)
    #include <p32xxxx.h>    /* this gives all the CPU/hardware definitions */
    #include <plib.h>       /* this gives the i/o definitions */
#endif
int LED43 = 43;
int LED13 = 13;
boolean off = true;
boolean off2 = true;
int check=0;
int timerTime;

void setup() {
  Serial.begin(9600);
  //100 Hz
  pinMode(LED13, OUTPUT);
    pinMode(LED43, OUTPUT);
    digitalWrite(LED13, LOW);
   digitalWrite(LED43, LOW);
    // init Timer2 mode and period (PR2)
   // Fpb = SYS_FREQ = 80Mhz (From configuration in bootloader code)
   // Timer Prescale = 8
   // PR2 = 0x9C3F = 39,999
   // interrupts every 4 ms
   // 4 ms = (PR2 + 1) * TMR Prescale / Fpb = (39999 + 1) * 8 / 80000000
//   CloseTimer2();
//   OpenTimer2( T2_ON | T2_PS_1_8 | T2_SOURCE_INT, 0x9C3F);
//   ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_7);
  timerTime=39999; //Variable that will contain the speed of the
                //interrupt--specifications indicate that the rate
                //of the interrupt will be ~50-500Hz, which means
                //2-20ms. This input should be in seconds
int rate = 80000000*timerTime/256-1;//Note the 256 comes from T2_PS_1_256 ratio
OpenTimer2( T2_ON | T2_PS_1_256 | T2_SOURCE_INT, rate);
ConfigIntTimer2((T2_INT_ON | T2_INT_PRIOR_3));
//OpenTimer3( T3_ON | T3_PS_1_256 | T3_SOURCE_INT, 0xFFFF);
//ConfigIntTimer3((T3_INT_ON | T3_INT_PRIOR_3));
/* mT2SetIntPriority( 4);    // set Timer2 Interrupt Priority
/*   mT2ClearIntFlag();       // clear interrupt flag
   mT2IntEnable(1);      // enable timer2 interrupts*/
}
int x = 0;
void loop(){
  x++;
  String timeString;
  timeString="";
  if (Serial.available())
  {
    int len = 0;
    while (Serial.available())
    {
      len++;
      char input = Serial.read();
      timeString += input;
    }
    Serial.println("LENGTH!");
    Serial.println(len);
    char timerString[len];
//  Serial.println(x);
    timeString.toCharArray(timerString, len);
    timerTime = atoi(timerString);
    Serial.println("Timer time!");
    Serial.println(timerTime);
//    Serial.println(timerTime);
    if (timerTime>10000)
    {
      int rate = 80000000*timerTime/256-1;//Note the 256 comes from T2_PS_1_256 ratio
//      OpenTimer2( T2_ON | T2_PS_1_256 | T2_SOURCE_INT, rate);
//      ConfigIntTimer2((T2_INT_ON | T2_INT_PRIOR_3));
      Serial.println("I heard ya!");
      WritePeriod2(rate);
    }
  }
}
#ifdef __cplusplus
extern "C" {
#endif


void __ISR(_TIMER_3_VECTOR,IPL3AUTO) comms_handler( void)
{
  if (off)
  {
//    digitalWrite(LED43, HIGH);
    digitalWrite(LED13, LOW);
    off = false;
  }
  else
  {
//    digitalWrite(LED43, LOW);
    digitalWrite(LED13, HIGH);
    off = true;
  }
  mT3ClearIntFlag();  // Clear interrupt flag
}

void __ISR(_TIMER_2_VECTOR,IPL3AUTO) comms2_handler( void)
{
  if (check > 3)
  {
    digitalWrite(LED43, HIGH);
    digitalWrite(LED13, HIGH);
    check = 0;
  }
  else
  {
    digitalWrite(LED43, LOW);
    digitalWrite(LED13, LOW);
    check++;
  }
  mT2ClearIntFlag();  // Clear interrupt flag
}

#ifdef __cplusplus
}
#endif

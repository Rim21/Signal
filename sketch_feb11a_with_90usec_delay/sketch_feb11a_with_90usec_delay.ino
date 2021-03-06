/* 3 Channel Signal Calibration

This item is rolled over from feb_09a&c, feb_08a which is
also based on feb_05a.

Motor does not run.

 include the pinchangeint library to access 3 x pin interrupts
*/
#include <PinChangeInt.h>
// include the elapsedMillis library to access 3 x micros timers
#include <elapsedMillis.h>
// include the digitalIOPerformance library for Leo Board and to
// access fast digital write and digital read. Pseudo equivalent
// to direct port manipulation. Must use const for pins.
#include <digitalIOPerformance.h> 

// Assign Leo interrupts
const byte phaseOin = 8;
const byte phaseGin = 9;
const byte phaseYin = 10;

// Assign channel out pins
const byte phaseOout = 2; //chosen as it is on PORTD & nonPWM
const byte phaseGout = 4; //corresponds to PIND - 4
const byte phaseYout = 12; //corresponds to PIND - 6

// Setup signal time constants for calcs

volatile unsigned long phaseOintervalin;

volatile unsigned long phaseGintervalin;

volatile unsigned long phaseYintervalin;

//Setup calibration & debounce constants
//const int debounce = 100;// not used as yet
float calPercent = .99;

// Setup the 3 timers to be used for calibration
elapsedMicros timerO;
elapsedMicros timerG;
elapsedMicros timerY;

// Setup the 3 timers to be used for interval
elapsedMicros timerOint;
elapsedMicros timerGint;
elapsedMicros timerYint;

// These bit flags are set in FlagsShared to indicate which
// channels have new signals
const byte phaseOflag = 1;
const byte phaseGflag = 2;
const byte phaseYflag = 4;

// holds the update flags defined above

volatile uint8_t FlagsShared;

  unsigned long phaseOinterval;

  unsigned long phaseGinterval;

  unsigned long phaseYinterval;
  
  unsigned long phaseOcal;

  unsigned long phaseGcal;

  unsigned long phaseYcal;
  
  static uint8_t UpdateFlags;
  
void setup()
{
  //delay(2500);//let the external circuit settle so as to set the o/p pins to reflect the i/ps

  pinMode(phaseOout, OUTPUT);    
  pinMode(phaseOin, INPUT);  
  pinMode(phaseGout, OUTPUT);    
  pinMode(phaseGin, INPUT);
  pinMode(phaseYout, OUTPUT);    
  pinMode(phaseYin, INPUT);
  
  digitalWrite (phaseOin, HIGH); // turn on the internal pull-up resistor
  digitalWrite (phaseGin, HIGH);
  digitalWrite (phaseYin, HIGH); 

    if (digitalRead(phaseOin)==HIGH){
    digitalWrite(phaseOout,HIGH);
  }else{
    digitalWrite(phaseOout,LOW);
  }
  //digitalWrite(phaseOout, digitalRead(phaseOin)); //gets rid of the multi pulses on i/p

  //PhaseG pin setup                      

    if (digitalRead(phaseGin)==HIGH){
    digitalWrite(phaseGout,HIGH);
  }else{
    digitalWrite(phaseGout,LOW);
  }
  //digitalWrite(phaseGout, digitalRead(phaseGin));
  
  //PhaseY pin setup                      

    if (digitalRead(phaseYin)==HIGH){
    digitalWrite(phaseYout,HIGH);
  }else{
    digitalWrite(phaseYout,LOW);
  }
  //digitalWrite(phaseYout, digitalRead(phaseYin));//doesn't use the digitalIOPer
  //macro if written this way as the state is a variable number
  
  // use the PinChangeInt library to attach the interrupts
  // Pins chosen make this interchangeable with an Uno and Leo
  PCintPort::attachInterrupt(phaseOin, calcPhaseO,CHANGE); 
  PCintPort::attachInterrupt(phaseGin, calcPhaseG,CHANGE); 
  PCintPort::attachInterrupt(phaseYin, calcPhaseY,CHANGE);

  //Set the Timers to 0 and start them as the last activity in Setup
  timerOint = 0;
  timerGint = 0;
  timerYint = 0; 
  timerO = 0;
  timerG = 0;
  timerY = 0; 
}

void loop()
{

  // check shared update flags to see if any channels have a change to the signal
  
  if(FlagsShared)
  {
    noInterrupts();
    
    // turn interrupts off - take local copies of the shared variables
    
    UpdateFlags = FlagsShared;
    
    
    if(UpdateFlags & phaseOflag)
    {
      phaseOinterval = phaseOintervalin;
      phaseOcal = (phaseOinterval); //timer period to wait
      
    }

    if(UpdateFlags & phaseGflag)
    {
      phaseGinterval = phaseGintervalin;
      phaseGcal = (phaseGinterval);
    }

    if(UpdateFlags & phaseYflag)
    {
      phaseYinterval = phaseYintervalin;
      phaseYcal = (phaseYinterval);
    }
                
    // clear shared copy of updated flags
    
    FlagsShared = 0;
    
    interrupts(); // we now have local copies of the inputs

  }
        
    if(timerO > phaseOcal)
    {
      timerO -= phaseOcal; //reset timer and maintain sync
      if (digitalRead(phaseOin)==HIGH){
         digitalWrite(phaseOout,HIGH);
         }else{
         digitalWrite(phaseOout,LOW);
        }      
    }
    

    if(timerG > phaseGcal)
    {
      timerG -= phaseGcal; //reset the timer
      if (digitalRead(phaseGin)==HIGH){
         digitalWrite(phaseGout,HIGH);
         }else{
         digitalWrite(phaseGout,LOW);
        }            
    }
  

    if(timerY > phaseYcal)
    {
      timerY -= phaseYcal; //reset the timer
      if (digitalRead(phaseYin)==HIGH){
         digitalWrite(phaseYout,HIGH);
         }else{
         digitalWrite(phaseYout,LOW);
        }            
    }
 
  UpdateFlags = 0; 
}

//  interrupt service routines

void calcPhaseO()
{
      phaseOintervalin=timerOint; // calc the interval since last read
      timerOint-=timerOint; // reset the time

  FlagsShared |= phaseOflag;
}

void calcPhaseG()
{
      phaseGintervalin=timerGint;
      timerGint-=timerGint;

  FlagsShared |= phaseGflag;
}

void calcPhaseY()
{
      phaseYintervalin=timerYint;
      timerYint-=timerYint;

  FlagsShared |= phaseYflag;
}

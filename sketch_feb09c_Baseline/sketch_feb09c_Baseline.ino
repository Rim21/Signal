/* 3 Channel Signal Calibration

This item is the baseline to feb_09b. Excludes the DigitalIOPerformance.

Result = same as for feb_09b - no performance gain?

Include the pinchangeint library to access 3 x pin interrupts
*/
#include <PinChangeInt.h>
// include the elapsedMillis library to access 3 x micros timers
#include <elapsedMillis.h>

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
float calPercent = .90;

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
  delay(2500);//let the external circuit settle so as to set the o/p pins to reflect the i/ps

  pinMode(phaseOout, OUTPUT);    
  pinMode(phaseOin, INPUT);  
  digitalWrite (phaseOin, HIGH); // turn on the internal pull-up resistor
    if (digitalRead(phaseOin)==HIGH){
    digitalWrite(phaseOout,HIGH);
  }else{
    digitalWrite(phaseOout,LOW);
  }
  //digitalWrite(phaseOout, digitalRead(phaseOin)); //gets rid of the multi pulses on i/p

  //PhaseG pin setup                      
  pinMode(phaseGout, OUTPUT);    
  pinMode(phaseGin, INPUT);
  digitalWrite (phaseGin, HIGH);
    if (digitalRead(phaseGin)==HIGH){
    digitalWrite(phaseGout,HIGH);
  }else{
    digitalWrite(phaseGout,LOW);
  }
  //digitalWrite(phaseGout, digitalRead(phaseGin));
  
  //PhaseY pin setup                      
  pinMode(phaseYout, OUTPUT);    
  pinMode(phaseYin, INPUT);
  digitalWrite (phaseYin, HIGH); 
    if (digitalRead(phaseYin)==HIGH){
    digitalWrite(phaseYout,HIGH);
  }else{
    digitalWrite(phaseYout,LOW);
  }
  //digitalWrite(phaseYout, digitalRead(phaseYin));
  
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
      phaseOcal = (phaseOinterval/calPercent); //timer period to wait
      
    }

    if(UpdateFlags & phaseGflag)
    {
      phaseGinterval = phaseGintervalin;
      phaseGcal = (phaseGinterval/calPercent);
    }

    if(UpdateFlags & phaseYflag)
    {
      phaseYinterval = phaseYintervalin;
      phaseYcal = (phaseYinterval/calPercent);
    }
                
    // clear shared copy of updated flags
    
    FlagsShared = 0;
    
    interrupts(); // we now have local copies of the inputs

  }
        
    if(timerO > phaseOcal)
    {
      if (digitalRead(phaseOin)==HIGH){
         digitalWrite(phaseOout,HIGH);
         }else{
         digitalWrite(phaseOout,LOW);
        }      
//      digitalWrite(phaseOout, digitalRead(phaseOin));
      timerO -= phaseOcal; //reset timer and maintain sync
    }
    

    if(timerG > phaseGcal)
    {
      if (digitalRead(phaseGin)==HIGH){
         digitalWrite(phaseGout,HIGH);
         }else{
         digitalWrite(phaseGout,LOW);
        }      
//      digitalWrite(phaseGout, digitalRead(phaseGin));
      timerG -= phaseGcal; //reset the timer
    }
  

    if(timerY > phaseYcal)
    {
      if (digitalRead(phaseYin)==HIGH){
         digitalWrite(phaseYout,HIGH);
         }else{
         digitalWrite(phaseYout,LOW);
        }      
//      digitalWrite(phaseYout, digitalRead(phaseYin));
      timerY -= phaseYcal; //reset the timer
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

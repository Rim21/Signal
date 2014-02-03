// 3 Channel Signal Calibration

// include the pinchangeint library
#include <PinChangeInt.h>

// Assign channel in pins
const byte phaseOin = 8;
const byte phaseGin = 9;
const byte phaseYin = 10;

// Assign channel out pins
const byte phaseOout = 5;
const byte phaseGout = 6;
const byte phaseYout = 7;

// Setup signal outputs expected by external processor
volatile uint8_t phaseOStateIn;
volatile uint8_t phaseGStateIn;
volatile uint8_t phaseYStateIn;

// Setup signal time constants for calcs
unsigned long phaseOstart = 0;
volatile unsigned long phaseOend = 0;
volatile unsigned long phaseOintervalin = 0;

//Setup calibration & debounce constants
const int debounce = 500;
float calPercent = .85;

// These bit flags are set in FlagsShared to indicate which
// channels have new signals
const byte phaseOflag = 1;
const byte phaseGflag = 2;
const byte phaseYflag = 4;

// holds the update flags defined above
volatile uint8_t FlagsShared;

void setup()
{

  //Speed pin setup                      
  pinMode(phaseOout, OUTPUT);    
  pinMode(phaseOin, INPUT);
  digitalWrite (phaseOin, HIGH);
  
  //PhaseO pin setup                      
  pinMode(phaseGout, OUTPUT);    
  pinMode(phaseGin, INPUT);
  digitalWrite (phaseGin, HIGH);

  //PhaseG pin setup                      
  pinMode(phaseYout, OUTPUT);    
  pinMode(phaseYin, INPUT);
  digitalWrite (phaseYin, HIGH);

  // use the PinChangeInt library to attach the interrupts
  // Pins chosen make this interchangeable with an Uno and Leo
  PCintPort::attachInterrupt(phaseOin, calcPhaseO,CHANGE); 
  PCintPort::attachInterrupt(phaseGin, calcPhaseG,CHANGE); 
  PCintPort::attachInterrupt(phaseYin, calcPhaseY,CHANGE); 
}

void loop()
{
  // create local variables to hold a local copies of the channel inputs

  static uint8_t phaseOstate;
  
  unsigned long phaseOinterval;
  
  unsigned long phaseOdelay;

  unsigned long phaseOlast;
  
  unsigned long phaseOmicros;
  
  static uint8_t phaseGstate;

  static uint8_t phaseYstate;

  // local copy of update flags
  
  static uint8_t UpdateFlags;

  // check shared update flags to see if any channels have a change in the signal
  
  if(FlagsShared)
  {
    noInterrupts();
    
    // turn interrupts off quickly to take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    
    UpdateFlags = FlagsShared;
    
    
    if(UpdateFlags & phaseOflag)
    {
      phaseOstate = phaseOStateIn;
      phaseOinterval = phaseOintervalin;
      phaseOlast = phaseOend;
    }

    if(UpdateFlags & phaseGflag)
    {
      phaseGstate = phaseGStateIn;
    }

    if(UpdateFlags & phaseYflag)
    {
      phaseYstate = phaseYStateIn;
    }
                
    // clear shared copy of updated flags
    
    FlagsShared = 0;
    
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on

  }
 
 phaseOmicros=micros();
 if(phaseOmicros-phaseOend<1000000)
 {
  if(phaseOmicros-phaseOlast>(phaseOinterval/calPercent))
  {
   phaseOlast=phaseOmicros;
   digitalWrite(phaseOout, phaseOstate);
  }
 } 






/*
  if(UpdateFlags & phaseOflag)
  {
    if (phaseOstate==LOW)
    {
    phaseOdelay=((phaseOinterval/calPercent)-phaseOinterval);
    delayMicroseconds(phaseOdelay);
    digitalWrite(phaseOout, phaseOstate);
    }
    else
    {
    phaseOdelay=((phaseOinterval/calPercent)-phaseOinterval);
    delayMicroseconds(phaseOdelay);
    digitalWrite(phaseOout, phaseOstate);
    }
  }
*/

  if(UpdateFlags & phaseGflag)
  {
    digitalWrite(phaseGout, phaseGstate);
  }



  if(UpdateFlags & phaseYflag)
  {
    digitalWrite(phaseYout, phaseYstate);
  }
        
  UpdateFlags = 0;
}


//  interrupt service routines

void calcPhaseO()
{
  phaseOStateIn = digitalRead(phaseOin);
  phaseOstart = micros();
  if(phaseOStateIn==LOW)
  {
  if ((phaseOstart-phaseOend)>debounce)
    {
      phaseOintervalin=phaseOstart-phaseOend;
      phaseOend=phaseOstart;
    }
  }
  FlagsShared |= phaseOflag;
}

void calcPhaseG()
{
  phaseGStateIn = digitalRead(phaseGin);
  FlagsShared |= phaseGflag;
}

void calcPhaseY()
{
  phaseYStateIn = digitalRead(phaseYin);
  FlagsShared |= phaseYflag;
}

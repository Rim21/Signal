/* 3 Channel Signal Calibration

This item is rolled over from feb_08c, feb_08a which is also based on
feb_05a.

On the scope the o/p is greater than the i/p.

Moved the cal calculation to the no-interrupts.

Removed PIND for digitalFastWrite.

Result is o/ps not yet working

 include the pinchangeint library to access 3 x pin interrupts
*/
#include <PinChangeInt.h>
// include the elapsedMillis library to access 3 x micros timers
#include <elapsedMillis.h>
#include <digitalWriteFast.h>

// Assign Leo interrupts
const byte phaseOin = 8;
const byte phaseGin = 9;
const byte phaseYin = 10;

// Assign channel out pins
const byte phaseOout = 2; //corresponds to PIND - 1
const byte phaseGout = 4; //corresponds to PIND - 4
const byte phaseYout = 12; //corresponds to PIND - 6

// Setup signal outputs expected by external processor

volatile boolean phaseOStateIn;
volatile boolean phaseGStateIn;
volatile boolean phaseYStateIn;

// Setup signal time constants for calcs

volatile unsigned long phaseOintervalin;

volatile unsigned long phaseGintervalin;

volatile unsigned long phaseYintervalin;

//Setup calibration & debounce constants
//const int debounce = 100;
float calPercent = .80;

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

//  create local variables to hold a local copies of the channel inputs

  static boolean phaseOstate;
  
  static boolean phaseGstate;

  static boolean phaseYstate;
  
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

  pinModeFast(2, OUTPUT);    
  pinModeFast(8, INPUT);  
  digitalWriteFast (8, HIGH); // turn on the internal pull-up resistor
  phaseOStateIn = digitalReadFast(phaseOin); // the intent here is to read and write the
//existing state of the signal at the beginning and then let the Loop() function alter it
//from there
 if(phaseOStateIn == 0)
  {
    digitalWriteFast(2, LOW);
  }else{
    digitalWriteFast(2, HIGH);
  }


  //PhaseG pin setup                      
  pinModeFast(4, OUTPUT);    
  pinModeFast(9, INPUT);
  digitalWriteFast (9, HIGH);
  phaseGStateIn = digitalReadFast(9);
  if(phaseGStateIn == 0)
  {
    digitalWriteFast(4, LOW);
  }else{
  digitalWriteFast(4, HIGH);
  }

  
  //PhaseY pin setup                      
  pinModeFast(12, OUTPUT);    
  pinModeFast(10, INPUT);
  digitalWriteFast (10, HIGH); 
  phaseYStateIn = digitalReadFast(phaseYin);
 if(phaseYStateIn == 0)
  {
    digitalWriteFast(12, LOW);
  }else{
    digitalWriteFast(12, HIGH);
  }
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

  // check shared update flags to see if any channels have a change in the signal
  
  if(FlagsShared)
  {
    noInterrupts();
    
    // turn interrupts off - take local copies of the shared variables
    
    UpdateFlags = FlagsShared;
    
    
    if(UpdateFlags & phaseOflag)
    {
      phaseOstate = phaseOStateIn;
      phaseOinterval = phaseOintervalin;
      phaseOcal = (phaseOinterval/calPercent); //moved the calculation to this part of the loop()
      
    }

    if(UpdateFlags & phaseGflag)
    {
      phaseGstate = phaseGStateIn;
      phaseGinterval = phaseGintervalin;
      phaseGcal = (phaseGinterval/calPercent);
    }

    if(UpdateFlags & phaseYflag)
    {
      phaseYstate = phaseYStateIn;
      phaseYinterval = phaseYintervalin;
      phaseYcal = (phaseYinterval/calPercent);
    }
                
    // clear shared copy of updated flags
    
    FlagsShared = 0;
    
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on

  }


        
    if(timerO > phaseOcal)
    {
       if(phaseOstate == 0)
        {
          digitalWriteFast(2, LOW);
        }else{
          digitalWriteFast(2, HIGH);
        }
      timerO -= phaseOcal; //reset timer by subtracting calInterval - maintains sync with other phases
      //digitalWriteFast(phaseOout, phaseOstate);
      //PIND=bit(1);
    }
    

    if(timerG > phaseGcal)
    {
       if(phaseGstate == 0)
        {
          digitalWriteFast(4, LOW);
        }else{
          digitalWriteFast(4, HIGH);
        }
      timerG -= phaseGcal; //reset the timer
      //PIND=bit(4);
    }
  

    if(timerY > phaseYcal)
    {
       if(phaseYstate == LOW)
        {
          digitalWriteFast(12, LOW);
        }else{
          digitalWriteFast(12, HIGH);
        }
      timerY -= phaseYcal; //reset the timer
      //digitalWriteFast(phaseYout, phaseYstate);
      //PIND=bit(6);
    }
 

  UpdateFlags = 0; 
}

//  interrupt service routines

void calcPhaseO()
{
      phaseOintervalin=timerOint; // calc the interval since last read
      phaseOStateIn=digitalReadFast(8);
      timerOint=0; // reset the time

  FlagsShared |= phaseOflag;
}

void calcPhaseG()
{
      phaseGintervalin=timerGint;
      phaseGStateIn=digitalReadFast(9);
      timerGint=0;

  FlagsShared |= phaseGflag;
}

void calcPhaseY()
{
      phaseYintervalin=timerYint;
      phaseYStateIn=digitalReadFast(10);
      timerYint=0;

  FlagsShared |= phaseYflag;
}

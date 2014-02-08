/* 3 Channel Signal Calibration
re-baseline from feb_06a_PINx

ISR timer code added
result1 = motor works

Calc moved to no-iterrupts part of loop()
result2 = motor works

Changed calPercent to .95. Changed the Loop() 'if' from flags
to timer
result3 = 05

Reversed code back to result2, added timer reset to
setup() & loop, added delayMicroseonds of 600/900 & 1900 and baselined

result4 = motor runs but has a noise that is OK but
appears to be getting slightly out of sync. For delay of 1900 I got
a very fast motor noise and an increase in speed to 10.6kmh but
also got an occassional 05

Changed the delayMicroseconds to a variable. Added the variable
calculation to the no-interrupts. Changed to cal of .75/.85/.90.

result5 = motor runs rough @ .85, just @ .90 and not @ .75


// include the pinchangeint library*/
#include <PinChangeInt.h>

// include the elapsedMillis library to access 3 x micros timers
#include <elapsedMillis.h>

// Assign channel in pins
const byte phaseOin = 8;
const byte phaseGin = 9;
const byte phaseYin = 10;

// Assign channel out pins
const byte phaseOout = 2;
const byte phaseGout = 4;
const byte phaseYout = 12;

// Setup signal outputs expected by external processor
volatile uint8_t phaseOStateIn;
volatile uint8_t phaseGStateIn;
volatile uint8_t phaseYStateIn;

//Setup calibration & debounce constants
const int debounce = 100;
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

  // create local variables to hold a local copies of the channel inputs

  static uint8_t phaseOstate;

  static uint8_t phaseGstate;

  static uint8_t phaseYstate;
  
volatile unsigned long phaseOintervalin;

volatile unsigned long phaseGintervalin;

volatile unsigned long phaseYintervalin;

  unsigned long phaseOinterval;

  unsigned long phaseGinterval;

  unsigned long phaseYinterval;
  
  unsigned long phaseOcal;

  unsigned long phaseGcal;

  unsigned long phaseYcal;
  
  unsigned long phaseOdelay;
  
  unsigned long phaseGdelay;
  
  unsigned long phaseYdelay;

  // local copy of update flags
  
  static uint8_t UpdateFlags;

void setup()
{
  delay(2500);
    Serial.begin(115200);
  
  //Speed pin setup                      
  pinMode(phaseOout, OUTPUT);    
  pinMode(phaseOin, INPUT);
  digitalWrite (phaseOin, HIGH);
    phaseOStateIn = digitalRead(phaseOin); // the intent here is to read and write the
//existing state of the signal at the beginning and then let the Loop() function alter it
//from there
  digitalWrite(phaseOout, phaseOStateIn);
  
  //PhaseO pin setup                      
  pinMode(phaseGout, OUTPUT);    
  pinMode(phaseGin, INPUT);
  digitalWrite (phaseGin, HIGH);
    phaseGStateIn = digitalRead(phaseGin);
  digitalWrite(phaseGout, phaseGStateIn);

  //PhaseG pin setup                      
  pinMode(phaseYout, OUTPUT);    
  pinMode(phaseYin, INPUT);
  digitalWrite (phaseYin, HIGH);
    phaseYStateIn = digitalRead(phaseYin);
  digitalWrite(phaseYout, phaseYStateIn);

  // use the PinChangeInt library to attach the interrupts
  // Pins chosen make this interchangeable with an Uno and Leo
  PCintPort::attachInterrupt(phaseOin, calcPhaseO,CHANGE); 
  PCintPort::attachInterrupt(phaseGin, calcPhaseG,CHANGE); 
  PCintPort::attachInterrupt(phaseYin, calcPhaseY,CHANGE);
 
   //Set the Timers to 0 and start them as the last activity in Setup
  timerOint = 0;
  timerGint = 0;
  timerYint = 0; 
//  timerO = 0;
//  timerG = 0;
//  timerY = 0;  
}

void loop()
{


  // check shared update flags to see if any channels have a change in the signal
  
  if(FlagsShared)
  {
    noInterrupts();
    
    // turn interrupts off quickly to take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    
    UpdateFlags = FlagsShared;
    
    
    if(UpdateFlags & phaseOflag)
    {
      phaseOinterval = phaseOintervalin;
      phaseOcal = (phaseOinterval/calPercent);
      phaseOdelay = phaseOcal-phaseOinterval;
    }

    if(UpdateFlags & phaseGflag)
    {
      phaseGinterval = phaseGintervalin;
      phaseGcal = (phaseGinterval/calPercent);
      phaseGdelay = phaseGcal-phaseGinterval;
    }

    if(UpdateFlags & phaseYflag)
    {
      phaseYinterval = phaseYintervalin;
      phaseYcal = (phaseYinterval/calPercent);
      phaseYdelay = phaseYcal-phaseYinterval;
    }
                
    // clear shared copy of updated flags
    
    FlagsShared = 0;
    
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on

  }
  

  if(UpdateFlags & phaseOflag)
  {
//    digitalWrite(phaseOout, phaseOstate);
//      timerO = 0;
    delayMicroseconds(phaseOdelay);
    PIND=bit(1);
  }



  if(UpdateFlags & phaseGflag)
  {
//    digitalWrite(phaseGout, phaseGstate);
//      timerG = 0;
    delayMicroseconds(phaseGdelay);    
    PIND=bit(4);
  }



  if(UpdateFlags & phaseYflag)
  {
//    digitalWrite(phaseYout, phaseYstate);
//      timerY = 0;
    delayMicroseconds(phaseYdelay);      
    PIND=bit(6);
  }

//      Serial.println();
//      Serial.print(UpdateFlags);        

  UpdateFlags = 0;

/*            Serial.println();
      Serial.print("O");
      Serial.print(",");     
      Serial.print(phaseOinterval);
      Serial.print(",");
      Serial.print(phaseOcal);
      Serial.print(",");
      Serial.print(phaseOdelay);

            Serial.println();
      Serial.print("G");
      Serial.print(",");     
      Serial.print(phaseGinterval);
      Serial.print(",");
      Serial.print(phaseGcal);
      Serial.print(",");
      Serial.print(phaseGdelay);

            Serial.println();
      Serial.print("Y");
      Serial.print(",");     
      Serial.print(phaseYinterval);
      Serial.print(",");
      Serial.print(phaseYcal);
      Serial.print(",");
      Serial.print(phaseYdelay);
*/
      
}


//  interrupt service routines

void calcPhaseO()
{
//  phaseOStateIn = digitalRead(phaseOin);
      phaseOintervalin=timerOint; // calc the interval since last read
      timerOint=0; // save the time that the pin read was taken
  FlagsShared |= phaseOflag;
}

void calcPhaseG()
{
//  phaseGStateIn = digitalRead(phaseGin);
      phaseGintervalin=timerGint;
      timerGint=0;
  FlagsShared |= phaseGflag;
}

void calcPhaseY()
{
//  phaseYStateIn = digitalRead(phaseYin);
      phaseYintervalin=timerYint;
      timerYint=0;
  FlagsShared |= phaseYflag;
}

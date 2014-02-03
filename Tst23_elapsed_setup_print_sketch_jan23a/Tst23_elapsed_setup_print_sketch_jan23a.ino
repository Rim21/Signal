// 3 Channel Signal Calibration

// include the pinchangeint library to access 3 x pin interrupts
#include <PinChangeInt.h>
// include the elapsedMillis library to access 3 x micros timers
#include <elapsedMillis.h>

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
volatile unsigned long phaseOstart = 0;
volatile unsigned long phaseOend = 0;
volatile unsigned long phaseOintervalin = 0;
boolean phaseOoutput = false;

// Setup the 3 timers to be used
elapsedMicros timerO;
elapsedMicros timerG;
elapsedMicros timerY;

volatile unsigned long phaseGstart = 0;
volatile unsigned long phaseGend = 0;
volatile unsigned long phaseGintervalin = 0;
boolean phaseGoutput = false;

volatile unsigned long phaseYstart = 0;
volatile unsigned long phaseYend = 0;
volatile unsigned long phaseYintervalin = 0;
boolean phaseYoutput = false;
    
//Setup calibration & debounce constants
const int debounce = 500; //in microseconds
float calPercent = .85; //for 8.5 to 9.1 should give 6.0 to 6.5

// These bit flags are set in FlagsShared to indicate which
// channels have new signals
const byte phaseOflag = 1;
const byte phaseGflag = 2;
const byte phaseYflag = 4;

// holds the update flags defined above
volatile uint8_t FlagsShared;

void setup()
{

  Serial.begin(115200);

  Serial.println("multiChannels");
  
  //PhaseO pin setup                      
  pinMode(phaseOout, OUTPUT);    
  pinMode(phaseOin, INPUT);  
  digitalWrite (phaseOin, HIGH); // turn on the internal pull-up resistor
  phaseOStateIn = digitalRead(phaseOin); // the intent here is to read and write the
//existing state of the signal at the beginning and then let the Loop() function alter it
//from there
  digitalWrite(phaseOout, phaseOStateIn);
  phaseOstart = 0;
  phaseOend = 0;
  phaseOintervalin = 0;
  
  //PhaseG pin setup                      
  pinMode(phaseGout, OUTPUT);    
  pinMode(phaseGin, INPUT);
  digitalWrite (phaseGin, HIGH);
  phaseGStateIn = digitalRead(phaseGin);
  digitalWrite(phaseGout, phaseGStateIn);
  
  //PhaseY pin setup                      
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
  timerO = 0;
  timerG = 0;
  timerY = 0; 
}

void loop()
{
  // create local variables to hold a local copies of the channel inputs

  static uint8_t phaseOstate;
  
  unsigned long phaseOinterval;

  unsigned long phaseGinterval;

  unsigned long phaseYinterval;
  
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
      phaseOoutput = true;
    }

    if(UpdateFlags & phaseGflag)
    {
      phaseGstate = phaseGStateIn;
      phaseGinterval = phaseGintervalin;
      phaseGoutput = true;
    }

    if(UpdateFlags & phaseYflag)
    {
      phaseYstate = phaseYStateIn;
      phaseYinterval = phaseYintervalin;
      phaseYoutput = true;
    }
                
    // clear shared copy of updated flags
    
    FlagsShared = 0;
    
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on

  }

if(phaseOoutput = true)
  {
      Serial.println();
      Serial.print("O");
      Serial.print(",");     
      Serial.print(phaseOinterval);
      Serial.print(",");
      Serial.print(timerO);
 //     Serial.print(",");
    
    if(timerO >= (phaseOinterval/calPercent))
    {
      timerO -= (phaseOinterval/calPercent); //reset timer by subtracting calInterval
      phaseOinterval=(phaseOinterval/calPercent);
      phaseOoutput = false;
      Serial.print(",");
      Serial.print(timerO);
      digitalWrite(phaseOout, phaseOstate);
    }
  }


if(phaseGoutput = true)
  {
 /*     Serial.println();
      Serial.print("G");
      Serial.print(",");     
      Serial.print(phaseGinterval);
      Serial.print(",");
      Serial.print(timerG);
      Serial.print(",");
  */  
    if(timerG >= (phaseGinterval/calPercent))
    {
      timerG -= (phaseGinterval/calPercent); //reset the timer
      phaseGoutput = false;
      digitalWrite(phaseGout, phaseGstate);
    }
  }
  

if(phaseYoutput = true)
  {
 /*   Serial.println();
      Serial.print("Y");
      Serial.print(",");     
      Serial.print(phaseYinterval);
      Serial.print(",");
      Serial.print(timerY);
      Serial.print(",");
 */   
    if(timerY >= (phaseYinterval/calPercent))
    {
      timerY -= (phaseYinterval/calPercent); //reset the timer
      phaseYoutput = false;
      digitalWrite(phaseYout, phaseYstate);
    }
  }
        
  UpdateFlags = 0;
}

//  interrupt service routines

void calcPhaseO()
{
  phaseOstart = micros(); // get the current time
  phaseOStateIn = digitalRead(phaseOin); // get the current pin state

      phaseOintervalin=phaseOstart-phaseOend; // calc the interval since last read
      phaseOend=phaseOstart; // save the time that the pin read was taken

  FlagsShared |= phaseOflag;
}

void calcPhaseG()
{
  phaseGStateIn = digitalRead(phaseGin);
  phaseGstart = micros();

      phaseGintervalin=phaseGstart-phaseGend;
      phaseGend=phaseGstart;

  FlagsShared |= phaseGflag;
}

void calcPhaseY()
{
  phaseYStateIn = digitalRead(phaseYin);
  phaseYstart = micros();

      phaseYintervalin=phaseYstart-phaseYend;
      phaseYend=phaseYstart;

  FlagsShared |= phaseYflag;
}

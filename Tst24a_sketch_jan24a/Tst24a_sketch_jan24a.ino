// 3 Channel Signal Calibration
// added in print
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
volatile unsigned long phaseOstart = 0;
volatile unsigned long phaseOend = 0;
volatile unsigned long phaseOintervalin = 0;
unsigned long phaseOprevious = 0;
boolean phaseOoutput = false;

volatile unsigned long phaseGstart = 0;
volatile unsigned long phaseGend = 0;
volatile unsigned long phaseGintervalin = 0;
unsigned long phaseGprevious = 0;
boolean phaseGoutput = false;

volatile unsigned long phaseYstart = 0;
volatile unsigned long phaseYend = 0;
volatile unsigned long phaseYintervalin = 0;
unsigned long phaseYprevious = 0;
boolean phaseYoutput = false;
  
unsigned long currentTime;
    
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

  Serial.begin(115200);

  Serial.println("multiChannels");
  
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

  unsigned long phaseGinterval;

  unsigned long phaseYinterval;

  unsigned long phaseOlast;

  unsigned long phaseGlast;

  unsigned long phaseYlast;
  
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
      phaseOlast = phaseOstart;
      phaseOoutput = true;
    }

    if(UpdateFlags & phaseGflag)
    {
      phaseGstate = phaseGStateIn;
      phaseGinterval = phaseGintervalin;
      phaseGlast = phaseGstart;
      phaseGoutput = true;
    }

    if(UpdateFlags & phaseYflag)
    {
      phaseYstate = phaseYStateIn;
      phaseYinterval = phaseYintervalin;
      phaseYlast = phaseYstart;
      phaseYoutput = true;
    }
                
    // clear shared copy of updated flags
    
    FlagsShared = 0;
    
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on

  }

  currentTime = micros();
  
  if(phaseOoutput == true)
  {
    if(currentTime-phaseOprevious > (phaseOinterval/calPercent))
    {
      phaseOprevious = currentTime;
      Serial.println();
      Serial.print("O");
      Serial.print(",");     
      Serial.print(phaseOinterval);
      Serial.print(",");
      Serial.print(phaseOprevious);
      digitalWrite(phaseOout, phaseOstate);
      phaseOoutput=false;
    }
  }

  
  if(phaseGoutput == true)
  {
    if(currentTime-phaseGprevious > (phaseGinterval/calPercent))
    {
      phaseGprevious = currentTime;
      Serial.println();
      Serial.print("G");
      Serial.print(",");     
      Serial.print(phaseGinterval);
      Serial.print(",");
      Serial.print(phaseGprevious);
      digitalWrite(phaseGout, phaseGstate);
      phaseGoutput=false;
    }
  }

 
  if(phaseYoutput == true)
  {
    if(currentTime-phaseYprevious > (phaseYinterval/calPercent))
    {
      phaseYprevious = currentTime;
      Serial.println();
      Serial.print("Y");
      Serial.print(",");     
      Serial.print(phaseYinterval);
      Serial.print(",");
      Serial.print(phaseYprevious);
      digitalWrite(phaseYout, phaseYstate);
      phaseYoutput=false;
    }
  }

        
  UpdateFlags = 0;
}

//  interrupt service routines

void calcPhaseO()
{
  phaseOStateIn = digitalRead(phaseOin);
  phaseOstart = micros();

  if ((phaseOstart-phaseOend)>debounce)
    {
      phaseOintervalin=phaseOstart-phaseOend;
      phaseOend=phaseOstart;
    }
  FlagsShared |= phaseOflag;
}

void calcPhaseG()
{
  phaseGStateIn = digitalRead(phaseGin);
  phaseGstart = micros();

  if ((phaseGstart-phaseGend)>debounce)
    {
      phaseGintervalin=phaseGstart-phaseGend;
      phaseGend=phaseGstart;
    }
  FlagsShared |= phaseGflag;
}

void calcPhaseY()
{
  phaseYStateIn = digitalRead(phaseYin);
  phaseYstart = micros();

  if ((phaseYstart-phaseYend)>debounce)
    {
      phaseYintervalin=phaseYstart-phaseYend;
      phaseYend=phaseYstart;
    }
  FlagsShared |= phaseYflag;
}

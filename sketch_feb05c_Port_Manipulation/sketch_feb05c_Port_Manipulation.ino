/* 3 Channel Signal Calibration

This item is rolled over from feb_05a to assist with trying different
code adjustments searching for the most efficent Loop() and/or ISR()

Included elapsedMicros for Calibration & ISR
changed back to PinChnageInterrupts
Looking at direct port manipulation to toggle outputs - Loop() only
setup() digitalWrite using a single line of code
Remove the digital read to ISR as a consequence


Results tst1 =


 include the pinchangeint library to access 3 x pin interrupts
*/
#include <PinChangeInt.h>
// include the elapsedMillis library to access 3 x micros timers
#include <elapsedMillis.h>

// Assign Leo interrupts
const byte phaseOin = 8;
const byte phaseGin = 9;
const byte phaseYin = 10;

// Assign channel out pins
const byte phaseOout = 2;
const byte phaseGout = 4;
const byte phaseYout = 12;

// Setup signal outputs expected by external processor
//volatile uint8_t phaseOStateIn;
//volatile uint8_t phaseGStateIn;
//volatile uint8_t phaseYStateIn;

// Setup signal time constants for calcs

volatile unsigned long phaseOintervalin = 0;

volatile unsigned long phaseGintervalin = 0;

volatile unsigned long phaseYintervalin = 0;

//Setup calibration & debounce constants
const int debounce = 100;
float calPercent = .85;

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

//  static uint8_t phaseOstate;
  
//  static uint8_t phaseGstate;

//  static uint8_t phaseYstate;
  
  unsigned long phaseOinterval;

  unsigned long phaseGinterval;

  unsigned long phaseYinterval;
  
  unsigned long phaseOcal;

  unsigned long phaseGcal;

  unsigned long phaseYcal;

  // local copy of update flags
  
  static uint8_t UpdateFlags;
  
void setup()
{

  Serial.begin(115200);

  //PhaseO pin setup                      
  pinMode(phaseOout, OUTPUT);    
  pinMode(phaseOin, INPUT);  
  digitalWrite (phaseOin, HIGH); // turn on the internal pull-up resistor
//phaseOStateIn = digitalRead(phaseOin); // the intent here is to read and write the
//existing state of the signal at the beginning and then let the Loop() function alter it
//from there
  digitalWrite(phaseOout, digitalRead(phaseOin));

  //PhaseG pin setup                      
  pinMode(phaseGout, OUTPUT);    
  pinMode(phaseGin, INPUT);
  digitalWrite (phaseGin, HIGH);
//phaseGStateIn = digitalRead(phaseGin);
  digitalWrite(phaseGout, digitalRead(phaseGin));
  
  //PhaseY pin setup                      
  pinMode(phaseYout, OUTPUT);    
  pinMode(phaseYin, INPUT);
  digitalWrite (phaseYin, HIGH); 
//phaseYStateIn = digitalRead(phaseYin);
  digitalWrite(phaseYout, digitalRead(phaseYin));
  
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
    
    // turn interrupts off quickly to take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    
    UpdateFlags = FlagsShared;
    
    
    if(UpdateFlags & phaseOflag)
    {
//      phaseOstate = phaseOStateIn;
      phaseOinterval = phaseOintervalin;
    }

    if(UpdateFlags & phaseGflag)
    {
//      phaseGstate = phaseGStateIn;
      phaseGinterval = phaseGintervalin;
    }

    if(UpdateFlags & phaseYflag)
    {
//      phaseYstate = phaseYStateIn;
      phaseYinterval = phaseYintervalin;
    }
                
    // clear shared copy of updated flags
    
    FlagsShared = 0;
    
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on

  }


    phaseOcal = (phaseOinterval/calPercent);
        
    if(timerO > phaseOcal)
    {
      timerO = 0; //reset timer by subtracting calInterval
      PIND = bit (1);
      //digitalWrite(phaseOout, phaseOstate);
    }


    phaseGcal = (phaseGinterval/calPercent);

    if(timerG > phaseGcal)
    {
      timerG = 0; //reset the timer
      PIND = bit (4);
      //digitalWrite(phaseGout, phaseGstate);
    }
  

    phaseYcal = (phaseYinterval/calPercent);

    if(timerY > phaseYcal)
    {
      timerY = 0; //reset the timer
      PIND = bit (6);
      //digitalWrite(phaseYout, phaseYstate);
    }
        
  UpdateFlags = 0;
     Serial.println();
      Serial.print(UpdateFlags);
            Serial.println();
      Serial.print("O");
      Serial.print(",");     
      Serial.print(phaseOinterval);
      Serial.print(",");
      Serial.print(phaseOcal);
      Serial.print(",");
      Serial.print(timerO);
      Serial.print(",");
      Serial.print(phaseOin);
            Serial.println();
      Serial.print("G");
      Serial.print(",");     
      Serial.print(phaseGinterval);
      Serial.print(",");
      Serial.print(phaseGcal);
      Serial.print(",");
      Serial.print(timerG);
      Serial.print(",");
      Serial.print(phaseGin);
            Serial.println();
      Serial.print("Y");
      Serial.print(",");     
      Serial.print(phaseYinterval);
      Serial.print(",");
      Serial.print(phaseYcal);
      Serial.print(",");
      Serial.print(timerY);
      Serial.print(",");
      Serial.print(phaseYin);
}

//  interrupt service routines

void calcPhaseO()
{
//phaseOStateIn = digitalRead(phaseOin); // get the current pin state

      phaseOintervalin=timerOint; // calc the interval since last read
      timerOint=0; // save the time that the pin read was taken

  FlagsShared |= phaseOflag;
}

void calcPhaseG()
{
//phaseGStateIn = digitalRead(phaseGin);

      phaseGintervalin=timerGint;
      timerGint=0;

  FlagsShared |= phaseGflag;
}

void calcPhaseY()
{
//phaseYStateIn = digitalRead(phaseYin);

      phaseYintervalin=timerYint;
      timerYint=0;

  FlagsShared |= phaseYflag;
}

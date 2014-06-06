/* 3 Channel Signal Calibration

This item is rolled over from feb_09a&c, feb_08a which is
also based on feb_05a.

On the scope the o/p is greater than the i/p by the calPercent. Fixed (but has now returned)
the every 5th missed interrupt using the digitalIOPerformance macro and an 'if' compare
statement for passing through the i/p state to the o/p.

The remaining problem is that the o/p signals are not correctly synced resulting
in a sparodic motor and code 05. Review of the timing code is required.

Motor does not run.

This code fixes the output oscillation after the motor
has stopped.

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
byte stateOin = LOW;
byte stateGin = LOW;
byte stateYin = LOW;
byte stateO = LOW;
byte stateG = LOW;
byte stateY = LOW;

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

unsigned long phaseOinterval;

unsigned long phaseGinterval;

unsigned long phaseYinterval;

unsigned long phaseOcal;

unsigned long phaseGcal;

unsigned long phaseYcal;

static uint8_t UpdateFlags;

void setup()
{

  pinMode(phaseOout, OUTPUT);
  pinMode(phaseOin, INPUT);
  digitalWrite (phaseOin, HIGH); // turn on the internal pull-up resistor
  digitalWrite (phaseOin, HIGH); // turn on the internal pull-up resistor
  if (digitalRead(phaseOin) == HIGH) {
    stateOin == HIGH;
    digitalWrite(phaseOout, HIGH);
  } else {
    stateOin == LOW;
    digitalWrite(phaseOout, LOW);
  }

  //PhaseG pin setup
  pinMode(phaseGout, OUTPUT);
  pinMode(phaseGin, INPUT);
  digitalWrite (phaseGin, HIGH);
  digitalWrite (phaseGin, HIGH);
  if (digitalRead(phaseGin) == HIGH) {
    digitalWrite(phaseGout, HIGH);
  } else {
    digitalWrite(phaseGout, LOW);
  }


  //PhaseY pin setup
  pinMode(phaseYout, OUTPUT);
  pinMode(phaseYin, INPUT);
  digitalWrite (phaseYin, HIGH);
  digitalWrite (phaseYin, HIGH);
  if (digitalRead(phaseYin) == HIGH) {
    digitalWrite(phaseYout, HIGH);
  } else {
    digitalWrite(phaseYout, LOW);
  }


  // use the PinChangeInt library to attach the interrupts
  // Pins chosen make this interchangeable with an Uno and Leo
  PCintPort::attachInterrupt(phaseOin, calcPhaseO, CHANGE);
  PCintPort::attachInterrupt(phaseGin, calcPhaseG, CHANGE);
  PCintPort::attachInterrupt(phaseYin, calcPhaseY, CHANGE);

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

  if (FlagsShared)
  {
    noInterrupts();

    // turn interrupts off - take local copies of the shared variables

    UpdateFlags = FlagsShared;


    if (UpdateFlags & phaseOflag)
    {
      phaseOinterval = phaseOintervalin;
      phaseOcal = (phaseOinterval / calPercent); //timer period to wait
      stateO = stateOin;
    }

    if (UpdateFlags & phaseGflag)
    {
      phaseGinterval = phaseGintervalin;
      phaseGcal = (phaseGinterval / calPercent);
    }

    if (UpdateFlags & phaseYflag)
    {
      phaseYinterval = phaseYintervalin;
      phaseYcal = (phaseYinterval / calPercent);
    }

    // clear shared copy of updated flags

    FlagsShared = 0;

    interrupts(); // we now have local copies of the inputs

  }


  //    PIND = bit(1);


  if (stateO == HIGH) {
    if (timerO > phaseOcal)
    {
      delay(50);
      digitalWrite(phaseOout, HIGH);
      //timerO -= phaseOcal; //reset timer and maintain sync
    }
  } else {
    digitalWrite(phaseOout, LOW);
  }


  if (timerG > phaseGcal)
  {
    PIND = bit(4);
    /*    timerG -= phaseGcal; //reset the timer
    if (digitalRead(phaseGin) == HIGH) {
      digitalWrite(phaseGout, HIGH);
    } else {
      digitalWrite(phaseGout, LOW);
    }*/
  }


  if (timerY > phaseYcal)
  {
    PIND = bit(6);
    /*    timerY -= phaseYcal; //reset the timer
        if (digitalRead(phaseYin) == HIGH) {
          digitalWrite(phaseYout, HIGH);
        } else {
          digitalWrite(phaseYout, LOW);
        }*/
  }

  UpdateFlags = 0;
}

//  interrupt service routines

void calcPhaseO()
{
  phaseOintervalin = timerOint; // calc the interval since last read
  timerOint -= timerOint; // reset the time
  stateOin = digitalRead(phaseOin);
  FlagsShared |= phaseOflag;
}

void calcPhaseG()
{
  phaseGintervalin = timerGint;
  timerGint -= timerGint;

  FlagsShared |= phaseGflag;
}

void calcPhaseY()
{
  phaseYintervalin = timerYint;
  timerYint -= timerYint;

  FlagsShared |= phaseYflag;
}

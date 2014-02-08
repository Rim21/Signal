// MultiChannels
//
// rcarduino.blogspot.com
//
// A simple approach for reading three RC Channels using pin change interrupts
//
// See related posts - 
// http://rcarduino.blogspot.co.uk/2012/01/how-to-read-rc-receiver-with.html
// http://rcarduino.blogspot.co.uk/2012/03/need-more-interrupts-to-read-more.html
// http://rcarduino.blogspot.co.uk/2012/01/can-i-control-more-than-x-servos-with.html
//
// rcarduino.blogspot.com
//

// include the pinchangeint library - see the links in the related topics section above for details
#include <PinChangeInt.h>

// Assign your channel in pins
#define SPEED_IN 8
#define PHASEO_IN 9
#define PHASEG_IN 10

// Assign your channel out pins
#define SPEED_OUT 5
#define PHASEO_OUT 6
#define PHASEG_OUT 7

// Setup signal outputs expected by Electronic Speed Controller
volatile uint8_t speedStateIn;
volatile uint8_t phaseOStateIn;
volatile uint8_t phaseGStateIn;
const int debounce = 100;
float calFactor = 1;
volatile boolean started;
volatile unsigned long speedStart;
volatile unsigned long speedEnd;

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define SPEED_FLAG 1
#define PHASEO_FLAG 2
#define PHASEG_FLAG 4

// holds the update flags defined above
volatile uint8_t FlagsShared;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the 
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals
volatile uint16_t IntervalInShared;
volatile uint16_t IntervalPOInShared;
volatile uint16_t IntervalPGInShared;

// These are used to record the rising edge of a pulse in the calcInput functions
// They do not need to be volatile as they are only used in the ISR. If we wanted
// to refer to these in loop and the ISR then they would need to be declared volatile

volatile uint32_t currentMicros;
volatile uint32_t previousMicros;

volatile uint32_t phaseOStart;
volatile uint32_t currentPOMicros;
volatile uint32_t previousPOMicros;

volatile uint32_t phaseGStart;
volatile uint32_t currentPGMicros;
volatile uint32_t previousPGMicros;

void setup()
{

  //Speed pin setup                      
  pinMode(SPEED_OUT, OUTPUT);    
  pinMode(SPEED_IN, INPUT);
  digitalWrite (SPEED_IN, HIGH);
  
  //PhaseO pin setup                      
  pinMode(PHASEO_OUT, OUTPUT);    
  pinMode(PHASEO_IN, INPUT);
  digitalWrite (PHASEO_IN, HIGH);

  //PhaseG pin setup                      
  pinMode(PHASEG_OUT, OUTPUT);    
  pinMode(PHASEG_IN, INPUT);
  digitalWrite (PHASEG_IN, HIGH);

  // using the PinChangeInt library, attach the interrupts
  // used to read the channels
  PCintPort::attachInterrupt(SPEED_IN, calcSpeed,CHANGE); 
  PCintPort::attachInterrupt(PHASEO_IN, calcPhaseO,CHANGE); 
  PCintPort::attachInterrupt(PHASEG_IN, calcPhaseG,CHANGE); 
}

void loop()
{
  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained 
  // between calls to loop.

  static uint16_t modInterval;
  static uint8_t speedState;
  static uint16_t spStart;
  static uint16_t spEnd;
  static uint16_t speedDelay;
  
  static uint16_t modPOInterval;
  static uint8_t phaseOState;

  static uint16_t modPGInterval;
  static uint8_t phaseGState;


  // local copy of update flags
  static uint8_t UpdateFlags;

  // check shared update flags to see if any channels have a new signal
  if(FlagsShared)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    
    UpdateFlags = FlagsShared;
    
    // in the current code, the shared values are always populated
    // so we could copy them without testing the flags
    // however in the future this could change, so lets
    // only copy when the flags tell us we can.
    
    if(UpdateFlags & SPEED_FLAG)
    {
      spStart = speedStart;
      spEnd = speedEnd;
      speedState = speedStateIn;
    }

    if(UpdateFlags & PHASEO_FLAG)
    {
      modPOInterval = IntervalPOInShared;
      phaseOState = phaseOStateIn;
    }

    if(UpdateFlags & PHASEG_FLAG)
    {
      modPGInterval = IntervalPGInShared;
      phaseGState = phaseGStateIn;
    }
                
    // clear shared copy of updated flags as we have already taken the updates
    // we still have a local copy if we need to use it in bUpdateFlags
    FlagsShared = 0;
    
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on
    // as soon as interrupts are back on, we can no longer use the shared copies, the interrupt
    // service routines own these and could update them at any time. During the update, the 
    // shared copies may contain junk. Luckily we have our local copies to work with :-)
  }
  
  // do any processing from here onwards
  // only use the local values unAuxIn, unThrottleIn and unSteeringIn, the shared
  // variables unAuxInShared, unThrottleInShared, unSteeringInShared are always owned by 
  // the interrupt routines and should not be used in loop
  
  // the following code provides simple pass through 
  // this is a good initial test, the Arduino will pass through
  // receiver input as if the Arduino is not there.
  // This should be used to confirm the circuit and power
  // before attempting any custom processing in a project.
  
  // we are checking to see if the channel value has changed, this is indicated  
  // by the flags. For the simple pass through we don't really need this check,
  // but for a more complex project where a new signal requires significant processing
  // this allows us to only calculate new values when we have new inputs, rather than
  // on every cycle.
  if(UpdateFlags & SPEED_FLAG)
  {
      modInterval = (spEnd-spStart);
      speedDelay = ((modInterval/calFactor)-modInterval);
      delayMicroseconds(speedDelay);                                              
      digitalWrite(SPEED_OUT, speedState);
      speedEnd=0; 
  }

  if(UpdateFlags & PHASEO_FLAG)
  {
    currentPOMicros=micros();
    if(currentPOMicros-phaseOStart<1000000)
  {
   if (currentPOMicros - previousPOMicros>((modPOInterval)/calFactor))          
       {previousPOMicros = currentPOMicros;                                                    
        //if (speedState == LOW) speedState = HIGH;
        //else speedState = HIGH;                                              
        digitalWrite(PHASEO_OUT, phaseOState);
        
        }
     }        
  }

  if(UpdateFlags & PHASEG_FLAG)
  {
    currentPGMicros=micros();
    if(currentPGMicros-phaseGStart<1000000)
  {
   if (currentPGMicros - previousPGMicros>((modPGInterval)/calFactor))          
       {previousPGMicros = currentPGMicros;                                                    
        //if (speedState == LOW) speedState = HIGH;
        //else speedState = HIGH;                                              
        digitalWrite(PHASEG_OUT, phaseGState);
        
        }
     }        
  }
        
  UpdateFlags = 0;
}


// simple interrupt service routine

void calcSpeed()
{
  speedStateIn = digitalRead(SPEED_IN);
  
  if (started)  
   { speedEnd = micros ();
    FlagsShared |= SPEED_FLAG; } 
  else 
    speedStart = micros ();
  started = !started;
}

void calcPhaseO()
{
  
  phaseOStart = micros();
  phaseOStateIn = digitalRead(PHASEO_IN);
  // if the pin is high, its a rising edge of the signal pulse, so lets record its value
  //if(digitalRead(SPEED_IN) == HIGH)
  //{ 
    
    //FlagsShared |= SPEED_FLAG;    
  //}
  //else
  //{
    // else it must be a falling edge, so lets get the time and subtract the time of the rising edge
    // this gives use the time between the rising and falling edges i.e. the pulse duration.
    IntervalPOInShared = (uint16_t)(micros() - phaseOStart);
    // use set the throttle flag to indicate that a new throttle signal has been received
    FlagsShared |= PHASEO_FLAG;
  //}
}

void calcPhaseG()
{
  
  phaseGStart = micros();
  phaseGStateIn = digitalRead(PHASEG_IN);
  // if the pin is high, its a rising edge of the signal pulse, so lets record its value
  //if(digitalRead(SPEED_IN) == HIGH)
  //{ 
    
    //FlagsShared |= SPEED_FLAG;    
  //}
  //else
  //{
    // else it must be a falling edge, so lets get the time and subtract the time of the rising edge
    // this gives use the time between the rising and falling edges i.e. the pulse duration.
    IntervalPGInShared = (uint16_t)(micros() - phaseGStart);
    // use set the throttle flag to indicate that a new throttle signal has been received
    FlagsShared |= PHASEG_FLAG;
  //}
}

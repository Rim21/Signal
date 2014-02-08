// eBike SpeedTuner by Rim21 using extracts from - Bike computer by Chris Purola and Annikken Andee Tut6
//This sketch is for a Leo board as it uses a different external interrupt to a Uno

#include <PinChangeInt.h>

// Sensor setup
const int debounce = 250; //equates to 500 microseconds;
const int speedPin = 7;                    
const int sensorPin = 9;                                     
int pulseState = HIGH;

// set surrounding pin numbers:
const int aPin = 0;
const int bPin = 1;
const int cPin = 2;
const int dPin = 4;
const int ePin = 5;
const int fPin = 6;

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define CHANNEL1_FLAG 1
// holds the update flags defined above
volatile uint8_t bUpdateFlagsShared;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals
volatile uint16_t unChannel1InShared;

//Calibration variables                                        
volatile unsigned long currentMicros = 0; 
volatile unsigned long previousMicros= 0;  
volatile unsigned long currentSpeed = 0;  
volatile unsigned long previousSpeed = 0;
volatile unsigned long interval = 0;                 
unsigned long modInterval = 0;                
float calFactor = 1;   // decrease to slow down speedometer                           
                         // calFactor of 1 makes no change to speedometer



void setup()
{
                        
  pinMode(speedPin, OUTPUT);    
  pinMode(sensorPin, INPUT);

  //set surrounding pins as inputs
  pinMode(aPin, INPUT);
  pinMode(bPin, INPUT);
  pinMode(cPin, INPUT);
  pinMode(dPin, INPUT);
  pinMode(ePin, INPUT);
  pinMode(fPin, INPUT);
  
  digitalWrite (sensorPin, HIGH);
  
  // turn on surround pull-up resistance to pins to avoid crosstalk
  digitalWrite(aPin, HIGH);
  digitalWrite(bPin, HIGH);
  digitalWrite(cPin, HIGH);
  digitalWrite(dPin, HIGH);
  digitalWrite(ePin, HIGH);
  digitalWrite(fPin, HIGH);
  
  PCintPort::attachInterrupt(sensorPin, sense,FALLING);        
}


void loop()
{
  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained
  // between calls to loop.
 // static uint16_t unChannel1In;
  
  uint8_t bUpdateFlags = 0;
  // check shared update flags to see if any channels have a new signal
  // for nicely formatted serial output use this
  //if(bUpdateFlagsShared == 0xFF)
  // for more responsive projects update any channels whenever a new signal is available using this
  if(bUpdateFlagsShared)
  {
  noInterrupts();                                                 
  //modInterval=interval;

  // take a local copy of which channels were updated in case we need to use this in the rest of loop
  bUpdateFlags = bUpdateFlagsShared;
   
  // in the current code, the shared values are always populated
  // so we could copy them without testing the flags
  // however in the future this could change, so lets
  // only copy when the flags tell us we can.
   

  if(bUpdateFlags & CHANNEL1_FLAG)
  {
      //unChannel1In = unChannel1InShared;
      modInterval=interval;
  }
  
  // clear shared copy of updated flags as we have already taken the updates
  // we still have a local copy if we need to use it in bUpdateFlags
  bUpdateFlagsShared = 0;
   

  interrupts();
  }
 
  // do any processing from here onwards
  // only use the local values unAuxIn, unThrottleIn and unSteeringIn, the shared
  // variables unAuxInShared, unThrottleInShared, unSteeringInShared are always owned by
  // the interrupt routines and should not be used in loop
  
  if(bUpdateFlags & CHANNEL1_FLAG)
  {
  currentMicros = micros();                             
  if (currentMicros-previousSpeed<1000000) 
  {
   if (currentMicros - previousMicros>((modInterval/2)/calFactor))          
       {previousMicros = currentMicros;                                                    
        if (pulseState == LOW) pulseState = HIGH;
        else pulseState = LOW;                                              
        digitalWrite(speedPin, pulseState);
        
        }
      }        
   }
}
 
void sense()
{
  currentSpeed=micros();                   
  
  if (digitalRead(sensorPin)==LOW) 
     {
              
       if ((currentSpeed - previousSpeed) > debounce)  
           {
            interval = currentSpeed - previousSpeed;        
            previousSpeed=currentSpeed;
            bUpdateFlagsShared |= CHANNEL1_FLAG;
            }
      }
}

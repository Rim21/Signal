// eBike Speed-Tuner by Rim21 using extracts from - Bike computer by Chris Purola, Annikken Andee Tut6 and RCArduino
// This sketch works for a Leo board using Pin Change Interrupts. It needs to be checked for Uno compaibility
// The pins for the interrupts chosen should also work for Uno
// Additional information and code snippets sourced from RCArduino
// RCArduino MultiChannel Loop back and servo ESC control for upto 8 RC channels
//
// rcarduino.blogspot.com
//
// A simple approach for reading three RC Channels using pin change interrupts
//
// See related posts -
// http://rcarduino.blogspot.co.uk/2012/01/how-to-read-rc-receiver-with.html
// http://rcarduino.blogspot.co.uk/2012/03/need-more-interrupts-to-read-more.html
// http://rcarduino.blogspot.co.uk/2012/01/can-i-control-more-than-x-servos-with.html

// include the pinchangeint library - see the links in the related topics section above for details

#include <PinChangeInt.h>

// Sensor setup
const int debounce = 50; //equates to 50 microseconds;
const int speedPin = 7;                    
const int sensorPin = 9;                                     
int pulseState = HIGH;
int valS = LOW;

// PhaseO setup - corresponds to the colour of the wire - Orange
const int POdebounce = 50; //Leave this ready if a new debounce figure is required
//Note it equates to 250 microseconds;
const int phaseOpin = 6;                    
const int phaseOspin = 8;                                     
int pulsePOstate = LOW;// may not be needed
int valO = LOW;

// PhaseG setup - corresponds to the colour of the wire - Green
const int PGdebounce = 50; //Leave this ready if a new debounce figure is required
//Note it equates to 250 microseconds;
const int phaseGpin = 5;                    
const int phaseGspin = 10;                                     
int pulsePGstate = HIGH;// may not be needed
int valG = LOW;

// PhaseY setup - corresponds to the colour of the wire - Yellow
const int PYdebounce = 50; //Leave this ready if a new debounce figure is required
//Note it equates to 250 microseconds;
const int phaseYpin = 4;                    
const int phaseYspin = 11;                                     
int pulsePYstate = LOW;// may not be needed
int valY = LOW;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals


//Calibration Speed variables                                        
volatile unsigned long currentMicros = 0; 
volatile unsigned long previousMicros= 0;  
volatile unsigned long currentSpeed = 0;  
volatile unsigned long previousSpeed = 0;
volatile unsigned long interval = 0;
unsigned long modInterval = 0;

//Calibration Phase O variables                                        
volatile unsigned long currentPOmicros = 0; 
volatile unsigned long previousPOmicros= 0;  
volatile unsigned long currentPOspeed = 0;  
volatile unsigned long previousPOspeed = 0;
volatile unsigned long intervalPO = 0;
unsigned long modPOinterval = 0;        

//Calibration Phase G variables                                        
volatile unsigned long currentPGmicros = 0; 
volatile unsigned long previousPGmicros= 0;  
volatile unsigned long currentPGspeed = 0;  
volatile unsigned long previousPGspeed = 0;
volatile unsigned long intervalPG = 0;                 
unsigned long modPGinterval = 0;        

//Calibration Phase Y variables                                        
volatile unsigned long currentPYmicros = 0; 
volatile unsigned long previousPYmicros= 0;  
volatile unsigned long currentPYspeed = 0;  
volatile unsigned long previousPYspeed = 0;
volatile unsigned long intervalPY = 0;
unsigned long modPYinterval = 0;        

float calFactor = 1;   // decrease to slow down speedometer                           
                       // calFactor of 1 makes no change to speedometer



void setup()
{
  //Speed pin setup                      
  pinMode(speedPin, OUTPUT);    
  pinMode(sensorPin, INPUT);
  digitalWrite (sensorPin, HIGH);  
  
  //Phase O pin setup                      
  pinMode(phaseOpin, OUTPUT);    
  pinMode(phaseOspin, INPUT);
  digitalWrite (phaseOspin, HIGH);

  
  //Phase G pin setup                      
  pinMode(phaseGpin, OUTPUT);    
  pinMode(phaseGspin, INPUT);
  digitalWrite (phaseGspin, HIGH);
  
  //Phase Y pin setup                      
  pinMode(phaseYpin, OUTPUT);    
  pinMode(phaseYspin, INPUT);
  digitalWrite (phaseYspin, HIGH);
 
  //Use Pin/Port Interrupts
  PCintPort::attachInterrupt(sensorPin, speedSense,FALLING);
  PCintPort::attachInterrupt(phaseOspin, phaseOsense,FALLING);
  PCintPort::attachInterrupt(phaseGspin, phaseGsense,FALLING);
  PCintPort::attachInterrupt(phaseYspin, phaseYsense,FALLING);  
}


void loop()
{
  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained
  // between calls to loop.

  noInterrupts();                                                 
  
  // take a local copy of which channels were updated in case we need to use this in the rest of loop
 
  modInterval=interval;
  modPOinterval=intervalPO;
  modPGinterval=intervalPG;
  modPYinterval=intervalPY;
   
  // in the current code, the shared values are always populated
  // so we could copy them without testing the flags
  // however in the future this could change, so lets
  // only copy when the flags tell us we can.


    valS=digitalRead(sensorPin);
    digitalWrite(speedPin, valS);
    
    valO=digitalRead(phaseOspin);
    digitalWrite(phaseOpin, valO);

    valG=digitalRead(phaseGspin);
    digitalWrite(phaseGpin, valG);

    valY=digitalRead(phaseYspin);
    digitalWrite(phaseYpin, valY);
    
  interrupts();


 
  // main processing from here onwards
  // only use the local values the shared variables are always owned by
  // the interrupt routines and should not be used in loop
  
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

  currentPOmicros = micros();                             
  if (currentPOmicros-previousPOspeed<1000000) 
  {
   if (currentPOmicros - previousPOmicros>((modPOinterval/2)/calFactor))          
       {previousPOmicros = currentPOmicros;                                                    
        if (pulsePOstate == LOW) pulsePOstate = HIGH;
        else pulsePOstate = LOW;                                              
        digitalWrite(phaseOpin, pulsePOstate);
        
        }
      }        

  currentPGmicros = micros();                             
  if (currentPGmicros-previousPGspeed<1000000) 
  {
   if (currentPGmicros - previousPGmicros>((modPGinterval/2)/calFactor))          
       {previousPGmicros = currentPGmicros;                                                    
        if (pulsePGstate == LOW) pulsePGstate = HIGH;
        else pulsePGstate = LOW;                                              
        digitalWrite(phaseGpin, pulsePGstate);
        
        }
      }        

  currentPYmicros = micros();                             
  if (currentPYmicros-previousPYspeed<1000000) 
  {
   if (currentPYmicros - previousPYmicros>((modPYinterval/2)/calFactor))          
       {previousPYmicros = currentPYmicros;                                                    
        if (pulsePYstate == LOW) pulsePYstate = HIGH;
        else pulsePYstate = LOW;                                              
        digitalWrite(phaseYpin, pulsePYstate);
        
        }
      }        

}
 
void speedSense()
{
  currentSpeed=micros();                   
  
  if (digitalRead(sensorPin)==LOW)
     {
              
       if ((currentSpeed - previousSpeed) > debounce)  
           {
            interval = currentSpeed - previousSpeed;        
            previousSpeed = currentSpeed;
            }
      }
}

void phaseOsense()
{
  currentPOspeed=micros();                   
  
 if (digitalRead(phaseOspin)==LOW) 
     {
              
       if ((currentPOspeed - previousPOspeed) > POdebounce)  
           {
            intervalPO = currentPOspeed - previousPOspeed;        
            previousPOspeed=currentPOspeed;
            }
      }
}

void phaseGsense()
{
  currentPGspeed=micros();                   
  
  if (digitalRead(phaseGspin)==LOW) 
     {
              
       if ((currentPGspeed - previousPGspeed) > PGdebounce)  
           {
            intervalPG = currentPGspeed - previousPGspeed;        
            previousPGspeed=currentPGspeed;
           }
      }
}

void phaseYsense()
{
  currentPYspeed=micros();                   
  
  if (digitalRead(phaseYspin)==LOW) 
     {
              
       if ((currentPYspeed - previousPYspeed) > PYdebounce)  
           {
            intervalPY = currentPYspeed - previousPYspeed;        
            previousPYspeed=currentPYspeed;
            }
      }
}


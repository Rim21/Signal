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
//
// rcarduino.blogspot.com
// 

// include the pinchangeint library - see the links in the related topics section above for details
#include <PinChangeInt.h>

#include <Servo.h>

// Assign your channel in pins
#define CHANNEL1_IN_PIN 2
#define CHANNEL2_IN_PIN 3
#define CHANNEL3_IN_PIN 4
#define CHANNEL4_IN_PIN 5
#define CHANNEL5_IN_PIN 6
#define CHANNEL6_IN_PIN 7
#define CHANNEL7_IN_PIN 8
#define CHANNEL8_IN_PIN 9

// Assign your channel out pins
#define CHANNEL1_OUT_PIN 10
#define CHANNEL2_OUT_PIN 11
#define CHANNEL3_OUT_PIN 12
#define CHANNEL4_OUT_PIN 13
#define CHANNEL5_OUT_PIN A0
#define CHANNEL6_OUT_PIN A1
#define CHANNEL7_OUT_PIN A2
#define CHANNEL8_OUT_PIN A3

// Servo objects generate the signals expected by Electronic Speed Controllers and Servos
// We will use the objects to output the signals we read in
// this example code provides a straight pass through of the signal with no custom processing
Servo servoChannel1;
Servo servoChannel2;
Servo servoChannel3;
Servo servoChannel4;
Servo servoChannel5;
Servo servoChannel6;
Servo servoChannel7;
Servo servoChannel8;

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define CHANNEL1_FLAG 1
#define CHANNEL2_FLAG 2
#define CHANNEL3_FLAG 4
#define CHANNEL4_FLAG 8
#define CHANNEL5_FLAG 16
#define CHANNEL6_FLAG 32
#define CHANNEL7_FLAG 64
#define CHANNEL8_FLAG 128

// holds the update flags defined above
volatile uint8_t bUpdateFlagsShared;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals
volatile uint16_t unChannel1InShared;
volatile uint16_t unChannel2InShared;
volatile uint16_t unChannel3InShared;
volatile uint16_t unChannel4InShared;
volatile uint16_t unChannel5InShared;
volatile uint16_t unChannel6InShared;
volatile uint16_t unChannel7InShared;
volatile uint16_t unChannel8InShared;

void setup()
{
  Serial.begin(115200);

  Serial.println("multiChannels");

  // attach servo objects, these will generate the correct
  // pulses for driving Electronic speed controllers, servos or other devices
  // designed to interface directly with RC Receivers 
  servoChannel1.attach(CHANNEL1_OUT_PIN);
  servoChannel2.attach(CHANNEL2_OUT_PIN);
  servoChannel3.attach(CHANNEL3_OUT_PIN);
  servoChannel4.attach(CHANNEL4_OUT_PIN);
  servoChannel5.attach(CHANNEL5_OUT_PIN);
  servoChannel6.attach(CHANNEL6_OUT_PIN);
  servoChannel7.attach(CHANNEL7_OUT_PIN);
  servoChannel8.attach(CHANNEL8_OUT_PIN);

  // using the PinChangeInt library, attach the interrupts
  // used to read the channels
  PCintPort::attachInterrupt(CHANNEL1_IN_PIN, calcChannel1,CHANGE);
  PCintPort::attachInterrupt(CHANNEL2_IN_PIN, calcChannel2,CHANGE);
  PCintPort::attachInterrupt(CHANNEL3_IN_PIN, calcChannel3,CHANGE);
  PCintPort::attachInterrupt(CHANNEL4_IN_PIN, calcChannel4,CHANGE);
  PCintPort::attachInterrupt(CHANNEL5_IN_PIN, calcChannel5,CHANGE);
  PCintPort::attachInterrupt(CHANNEL6_IN_PIN, calcChannel6,CHANGE);
  PCintPort::attachInterrupt(CHANNEL7_IN_PIN, calcChannel7,CHANGE);
  PCintPort::attachInterrupt(CHANNEL8_IN_PIN, calcChannel8,CHANGE);

  // for loop back test only, lets set each channel to a known value
  servoChannel1.writeMicroseconds(1100);
  servoChannel2.writeMicroseconds(1200);
  servoChannel3.writeMicroseconds(1300);
  servoChannel4.writeMicroseconds(1400);
  servoChannel5.writeMicroseconds(1500);
  servoChannel6.writeMicroseconds(1600);
  servoChannel7.writeMicroseconds(1700);
  servoChannel8.writeMicroseconds(1800);
}

void loop()
{
  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained
  // between calls to loop.
  static uint16_t unChannel1In;
  static uint16_t unChannel2In;
  static uint16_t unChannel3In;
  static uint16_t unChannel4In;
  static uint16_t unChannel5In;
  static uint16_t unChannel6In;
  static uint16_t unChannel7In;
  static uint16_t unChannel8In;

  uint8_t bUpdateFlags = 0;
  // check shared update flags to see if any channels have a new signal
  // for nicely formatted serial output use this
  if(bUpdateFlagsShared == 0xFF)
  // for more responsive projects update any channels whenever a new signal is available using this
  // if(bUpdateFlagsShared)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    bUpdateFlags = bUpdateFlagsShared;
   
    // in the current code, the shared values are always populated
    // so we could copy them without testing the flags
    // however in the future this could change, so lets
    // only copy when the flags tell us we can.
   

    if(bUpdateFlags & CHANNEL1_FLAG)
    {
      unChannel1In = unChannel1InShared;
    }
   
    if(bUpdateFlags & CHANNEL2_FLAG)
    {
      unChannel2In = unChannel2InShared;
    }
   
    if(bUpdateFlags & CHANNEL3_FLAG)
    {
      unChannel3In = unChannel3InShared;
    }

    if(bUpdateFlags & CHANNEL4_FLAG)
    {
      unChannel4In = unChannel4InShared;
    }
   
    if(bUpdateFlags & CHANNEL5_FLAG)
    {
      unChannel5In = unChannel5InShared;
    }
   
    if(bUpdateFlags & CHANNEL6_FLAG)
    {
      unChannel6In = unChannel6InShared;
    }
    
    if(bUpdateFlags & CHANNEL7_FLAG)
    {
      unChannel7In = unChannel7InShared;
    }
   
    if(bUpdateFlags & CHANNEL8_FLAG)
    {
      unChannel8In = unChannel8InShared;
    }
   
    // clear shared copy of updated flags as we have already taken the updates
    // we still have a local copy if we need to use it in bUpdateFlags
    bUpdateFlagsShared = 0;
   
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on
    // as soon as interrupts are back on, we can no longer use the shared copies, the interrupt
    // service routines own these and could update them at any time. During the update, the
    // shared copies may contain junk. Luckily we have our local copies to work with :-)
  }
  
  // do any processing from here onwards
  // only use the local values unAuxIn, unThrottleIn and unSteeringIn, the shared
  // variables unAuxInShared, unThrottleInShared, unSteeringInShared are always owned by
  // the interrupt routines and should not be used in loop
  if(bUpdateFlags & CHANNEL1_FLAG)
  {
      // when you are ready to add a servo or esc, remove the // from the following line to use the channel input to control it
      // servoChannel1.writeMicroseconds(unChannel1In);
      Serial.println("");
      Serial.print(bUpdateFlags);
      Serial.print(",");
      Serial.print(unChannel1In);
      Serial.print(",");
  }
  
  if(bUpdateFlags & CHANNEL2_FLAG)
  {
      // when you are ready to add a servo or esc, remove the // from the following line to use the channel input to control it
      // servoChannel2.writeMicroseconds(unChannel2In);
      Serial.print(unChannel2In);
      Serial.print(",");
  }
  
  if(bUpdateFlags & CHANNEL3_FLAG)
  {
      // when you are ready to add a servo or esc, remove the // from the following line to use the channel input to control it
      // servoChannel3.writeMicroseconds(unChannel3In);
      Serial.print(unChannel3In);
      Serial.print(",");
  }
  
  if(bUpdateFlags & CHANNEL4_FLAG)
  {
      // when you are ready to add a servo or esc, remove the // from the following line to use the channel input to control it
      // servoChannel4.writeMicroseconds(unChannel4In);
      Serial.print(unChannel4In);
      Serial.print(",");
  }
  
  if(bUpdateFlags & CHANNEL5_FLAG)
  {
      // when you are ready to add a servo or esc, remove the // from the following line to use the channel input to control it
      // servoChannel5.writeMicroseconds(unChannel5In);
      Serial.print(unChannel5In);
      Serial.print(",");
  }
  
  if(bUpdateFlags & CHANNEL6_FLAG)
  {
      // when you are ready to add a servo or esc, remove the // from the following line to use the channel input to control it
      // servoChannel6.writeMicroseconds(unChannel6In);
      Serial.print(unChannel6In);
      Serial.print(",");
  }
  
  if(bUpdateFlags & CHANNEL7_FLAG)
  {
      // when you are ready to add a servo or esc, remove the // from the following line to use the channel input to control it
      // servoChannel7.writeMicroseconds(unChannel7In);
      Serial.print(unChannel7In);
      Serial.print(",");
  }
  
  if(bUpdateFlags & CHANNEL8_FLAG)
  {
      // when you are ready to add a servo or esc, remove the // from the following line to use the channel input to control it
      // servoChannel8.writeMicroseconds(unChannel8In);
      Serial.print(unChannel8In);
      Serial.print(",");
  }
}


void calcChannel1()
{
  static uint32_t ulStart;
  
  if(PCintPort::pinState) // this is equivalent to digitalRead(CHANNEL1_IN_PIN) but about 10 times faster
  {
    ulStart = micros();
  }
  else
  {
    unChannel1InShared = (uint16_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL1_FLAG;
  }
}

void calcChannel2()
{
  static uint32_t ulStart;
  
  if(PCintPort::pinState)
  {
    ulStart = micros();
  }
  else
  {
    unChannel2InShared = (uint16_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL2_FLAG;
  }
}

void calcChannel3()
{
  static uint32_t ulStart;
  
  if(PCintPort::pinState)
  {
    ulStart = micros();
  }
  else
  {
    unChannel3InShared = (uint16_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL3_FLAG;
  }
}

void calcChannel4()
{
  static uint32_t ulStart;
  
  if(PCintPort::pinState)
  {
    ulStart = micros();
  }
  else
  {
    unChannel4InShared = (uint16_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL4_FLAG;
  }
}

void calcChannel5()
{
  static uint32_t ulStart;
  
  if(PCintPort::pinState)
  {
    ulStart = micros();
  }
  else
  {
    unChannel5InShared = (uint16_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL5_FLAG;
  }
}

void calcChannel6()
{
  static uint32_t ulStart;
  
  if(PCintPort::pinState)
  {
    ulStart = micros();
  }
  else
  {
    unChannel6InShared = (uint16_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL6_FLAG;
  }
}

void calcChannel7()
{
  static uint32_t ulStart;
  
  if(PCintPort::pinState)
  {
    ulStart = micros();
  }
  else
  {
    unChannel7InShared = (uint16_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL7_FLAG;
  }
}

void calcChannel8()
{
  static uint32_t ulStart;
  
  if(PCintPort::pinState)
  {
    ulStart = micros();
  }
  else
  {
    unChannel8InShared = (uint16_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL8_FLAG;
  }
}

// eBike SpeedTuner by Rim21 using extracts from - Bike computer by Chris Purola and Annikken Andee Tut6

#include <SPI.h> // These are the essential libraries you'll need
#include <Andee.h>

AndeeHelper speedo; //Declare an instance of AndeeHelper to help manage our display.

//int circ = 2268;  //circ circumference in millimeters

const int debounce = 2500;
//const int speedometerPin = 7;                    
const int sensorPin = 3;                                     
int pulseState = LOW;                                        
volatile unsigned long currentMicros = 0; 
volatile unsigned long previousMicros= 0;  
volatile unsigned long currentSpeed = 0;  
volatile unsigned long previousSpeed = 0;
volatile unsigned long interval = 0;                 
unsigned long modInterval = 0;                
float calFactor = .91;   // decrease to slow down speedometer                           
                         // calFactor of 1 makes no change to speedometer
double tph;
double wheel;
double calSpeed;

void setup()
{
  Andee.begin();  //Setup communications between Annikken Andee and Arduino
  initAndee();    //Init Andee MemBlocks
  Andee.clear();  //Clear the screen of any previous displays
  speedo.updateData(0); //init with speed 0
  wheel = 37.8/100000; //2268mm to cm / 6 pulses
//pinMode (13, OUTPUT);                        
//pinMode(speedometerPin, OUTPUT);    
  pinMode(sensorPin, INPUT);                 
  digitalWrite (sensorPin, HIGH);
  attachInterrupt (1, sense, FALLING);        
}


void loop()
{
  noInterrupts();                                                 
  modInterval=interval;
  interrupts();                                                      
  currentMicros = micros();                             
  if (currentMicros-previousSpeed<1000000) 
  {
   if (currentMicros - previousMicros>((modInterval/2)/calFactor))          
       { previousMicros = currentMicros;                                                    
        tph=modInterval/36000000;
        calSpeed = wheel/tph;
        //if (pulseState == LOW) pulseState = HIGH; else pulseState = LOW;
        //digitalWrite(13, pulseState);         //to blink onboard LED                                                         
        //digitalWrite(speedometerPin, pulseState);
        speedo.updateData(calSpeed, 2); // We're finally doing the updating here
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
            }
      }
}

void initAndee()
{
  speedo.setId(0);  //Associate andeeHelper with MemBlock 0 of Annikken Andee.
  speedo.setType(DATA_OUT);  //Set Type to DATA_OUT, BUTTON_IN or KEYBOARD_IN display
  speedo.setLocation(0, 0, FULL);  //Set row to appear and size. setLocation(int row, int order, int span)
  
  //Set the title, data and units of display. 
  speedo.setTitle("Speed");
  speedo.setData("");
  speedo.setUnit("km/h");
}

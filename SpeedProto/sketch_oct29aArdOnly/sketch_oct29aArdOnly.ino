// eBike SpeedTuner by Rim21 using extracts from - Bike computer by Chris Purola and Annikken Andee Tut6

//#include <SPI.h> // These are the essential libraries you'll need
//#include <Andee.h>

//AndeeHelper speedo; //Declare an instance of AndeeHelper to help manage our display.

//volatile unsigned long wheelCircum = 2230;  //circ circumference in millimeters
// Sensor setup
const int debounce = 6; //equates to 2.55 milliseconds;
const int speedometerPin = 7;                    
const int sensorPin = 3;                                     
int pulseState = LOW;

//Calibration variables                                        
volatile unsigned long currentMicros = 0; 
volatile unsigned long previousMicros= 0;  
volatile unsigned long currentSpeed = 0;  
volatile unsigned long previousSpeed = 0;
volatile unsigned long interval = 0;                 
unsigned long modInterval = 0;                
float calFactor = .93;   // decrease to slow down speedometer                           
                         // calFactor of 1 makes no change to speedometer

//Speed calculation variables
//volatile unsigned long speed100m = 0;
//volatile unsigned long pulseCircum = 0;
//double speedKm = 0;

void setup()
{
//  Andee.begin();  //Setup communications between Annikken Andee and Arduino
//  initAndee();    //Init Andee MemBlocks
//  Andee.clear();  //Clear the screen of any previous displays
//  speedo.updateData(0); //init with speed 0
//  pulseCircum = wheelCircum/5; //2230mm / 6 pulses
//pinMode (13, OUTPUT);                        
  pinMode(speedometerPin, OUTPUT);    
  pinMode(sensorPin, INPUT);                 
  digitalWrite (sensorPin, HIGH);
  attachInterrupt (1, sense, FALLING);        
}


void loop()
{
  noInterrupts();                                                 
  modInterval=interval;
//  speed100m = pulseCircum*36000/modInterval;
//  speedKm = speed100m/100;
//  speedo.updateData(speedKm,1); // We're finally doing the updating here
  interrupts();                                                      
  currentMicros = millis();                             
  if (currentMicros-previousSpeed<1000) 
  {
   if (currentMicros - previousMicros>((modInterval/2)/calFactor))          
       { previousMicros = currentMicros;                                                    
        if (pulseState == LOW) pulseState = HIGH; else pulseState = LOW;
        //digitalWrite(13, pulseState);         //to blink onboard LED                                                         
        digitalWrite(speedometerPin, pulseState);
        
        }                                  
   }
}
 
void sense()
{
  currentSpeed=millis();                   
  if (digitalRead(sensorPin)==LOW) 
     {
        if ((currentSpeed - previousSpeed) > debounce)  
           {
            interval = currentSpeed - previousSpeed;        
            previousSpeed=currentSpeed;
            }
      }
}

//void initAndee()
//{
//  speedo.setId(0);  //Associate andeeHelper with MemBlock 0 of Annikken Andee.
//  speedo.setType(DATA_OUT);  //Set Type to DATA_OUT, BUTTON_IN or KEYBOARD_IN display
//  speedo.setLocation(0, 0, FULL);  //Set row to appear and size. setLocation(int row, int order, int span)
  
  //Set the title, data and units of display. 
//  speedo.setTitle("Speed");
//  speedo.setData("");
//  speedo.setUnit("km/h");
//}

// eBike SpeedTuner by Rim21 using extracts from - Bike computer by Chris Purola and Annikken Andee Tut6

#include <SPI.h> // These are the essential libraries you'll need
#include <Andee.h>

AndeeHelper speedo; //Declare an instance of AndeeHelper to help manage our display.
AndeeHelper phaseReading;
AndeeHelper phaseReading2;
AndeeHelper phaseReading3;

volatile unsigned long wheelCircum = 2230;  //circ circumference in millimeters
// Sensor setup
const int debounce = 3500; //equates to 3.5 milliseconds;
const int speedometerPin = 7;                    
const int sensorPin = 3;
const int pinPhase = A0; // Analog Pin A0 connects to phase wire
const int pinPhase2 = A1; // Analog Pin A1 connects to phase wire
const int pinPhase3 = A2; // Analog Pin A2 connects to phase wire
int pulseState = LOW;

//Calibration variables                                        
volatile unsigned long currentMicros = 0; 
volatile unsigned long previousMicros= 0;  
volatile unsigned long currentSpeed = 0;  
volatile unsigned long previousSpeed = 0;
volatile unsigned long interval = 0;                 
unsigned long modInterval = 0;                
float calFactor = .90;   // decrease to slow down speedometer                           
                         // calFactor of 1 makes no change to speedometer

//Speed calculation variables
volatile unsigned long speed100m = 0;
volatile unsigned long pulseCircum = 0;
double speedKm = 0;

//Phase measurement variable(s)
float valuePhase; // Need thie float variable for calculations
float valuePhase2;
float valuePhase3;

void setup()
{
  Andee.begin();  //Setup communications between Annikken Andee and Arduino
  initAndee();    //Init Andee MemBlocks
  Andee.clear();  //Clear the screen of any previous displays
  speedo.updateData(0); //init with speed 0
  pulseCircum = wheelCircum/6; //2230mm / 6 pulses                     
  pinMode(speedometerPin, OUTPUT);    
  pinMode(sensorPin, INPUT);                 
  digitalWrite (sensorPin, HIGH);
  attachInterrupt (1, sense, FALLING);        
}


void loop()
{
  noInterrupts();                                                 
  //do speed calcs
  modInterval=interval;
  speed100m = pulseCircum*36000/modInterval;
  speedKm = speed100m/100;
  
  //read phase voltage
  valuePhase = analogRead(pinPhase)*5.0/1024; // Store the calculated voltage in a float var
  phaseReading.setData(valuePhase,2);
  valuePhase2 = analogRead(pinPhase2)*5.0/1024; // Store the calculated voltage in a float var
  phaseReading2.setData(valuePhase2,2);
  valuePhase3 = analogRead(pinPhase3)*5.0/1024; // Store the calculated voltage in a float var
  phaseReading3.setData(valuePhase3,2);
  
  //update readings
  phaseReading.update();
  phaseReading2.update();
  phaseReading3.update();
  speedo.updateData(speedKm,1); // We're finally doing the updating here
  
  //turn interrupts on
  interrupts();                                                      
  currentMicros = micros();                             
  if (currentMicros-previousSpeed<1000000) 
  {
   if (currentMicros - previousMicros>((modInterval/2)/calFactor))          
       { previousMicros = currentMicros;                                                    
        if (pulseState == LOW) pulseState = HIGH; else pulseState = LOW;
        digitalWrite(speedometerPin, pulseState);
        
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
  //Setup speedo display(s)
  speedo.setId(0);  //Associate andeeHelper with MemBlock 0 of Annikken Andee.
  speedo.setType(DATA_OUT);  //Set Type to DATA_OUT, BUTTON_IN or KEYBOARD_IN display
  speedo.setLocation(0, 0, FULL);  //Set row to appear and size. setLocation(int row, int order, int span)
  
  //Set the title, data and units of display. 
  speedo.setTitle("Speed");
  speedo.setData("");
  speedo.setUnit("km/h");
  
  //Setup Phase voltage display(s)
  phaseReading.setId(1); // assign unique value to each object
  phaseReading.setType(DATA_OUT);
  phaseReading.setLocation(1,0,FULL);
  
  phaseReading2.setId(2); // assign unique value to each object
  phaseReading2.setType(DATA_OUT);
  phaseReading2.setLocation(2,0,FULL);
 
  phaseReading3.setId(3); // assign unique value to each object
  phaseReading3.setType(DATA_OUT);
  phaseReading3.setLocation(3,0,FULL);
  
  //Set the title, data and units of display. 
  phaseReading.setTitle("Phase Voltage Blue Wire"); 
  phaseReading.setUnit("volts");
  phaseReading.setTitleColor(WHITE); // Set title bg to white
  phaseReading.setTitleTextColor(TEXT_DARK); // Set title font to black
  phaseReading.setColor(BLACK); // Set bg to black
  
  phaseReading2.setTitle("Phase Voltage Green Wire"); 
  phaseReading2.setUnit("volts");
  phaseReading2.setTitleColor(WHITE); // Set title bg to white
  phaseReading2.setTitleTextColor(TEXT_DARK); // Set title font to black
  phaseReading2.setColor(BLACK); // Set bg to black

  phaseReading3.setTitle("Phase Voltage Yellow Wire"); 
  phaseReading3.setUnit("volts");
  phaseReading3.setTitleColor(WHITE); // Set title bg to white
  phaseReading3.setTitleTextColor(TEXT_DARK); // Set title font to black
  phaseReading3.setColor(BLACK); // Set bg to black

}

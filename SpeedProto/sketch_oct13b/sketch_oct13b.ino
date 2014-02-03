// eBike SpeedTuner by Rim21 using extracts from - Bike computer by Chris Purola and Annikken Andee Tut6

#include <SPI.h> // These are the essential libraries you'll need
#include <Andee.h>

AndeeHelper speedReading;

int circ = 2268;  //circ circumference in millimeters
int speedA = 255; // ARGB values for Speedvoltage display
int speedR = 0;
int speedG = 0;
int speedB = 0;

char colorA[9]; // Strings to store the ARGB color codes
char colorB[9];

const int debounce = 2500;
const int speedometerPin = 7;                    
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
float mph = 0;

void setup()
{
pinMode (13, OUTPUT);                        
pinMode(speedometerPin, OUTPUT);    
pinMode(sensorPin, INPUT);                 
digitalWrite (sensorPin, HIGH);
  Andee.begin(); // Sets up the communication link between Arduino and the Annikken Andee
  setInitialData();
  Andee.clear(); // Clear the screen of any previous displays             
attachInterrupt (1, sense, FALLING);        
}

void setInitialData()
{
  speedReading.setId(0); // Don't forget to assign unique values to each object
  speedReading.setType(DATA_OUT);
  speedReading.setLocation(0,0,FULL);
  speedReading.setTitle("Input Speed signal "); 
  speedReading.setUnit("MPH");
  speedReading.setTitleColor(WHITE); // Set title bg to white
  speedReading.setTitleTextColor(TEXT_DARK); // Set title font to black
  speedReading.setColor(BLACK); // Set bg to black
}
   
void loop()
{
  noInterrupts();                                                 
  modInterval=interval;
  mph=interval;
      speedReading.setData(mph,2);  
      speedB = 255 * (mph/5); // Divide by five to get a range of values from 0 to 1
      speedR = (255/3) * (mph/5); // (255/3) because we don't want the full intensity of red.
      sprintf(colorA, "%02X%02X%02X%02X", speedA, speedR, speedG, speedB);
      speedReading.setColor(colorA); // Change display box to new colour. 
      speedReading.update(); // We're finally doing the updating here      
  interrupts();                                                      
  currentMicros = micros();                             
  if (currentMicros-previousSpeed<1000000) 
  {
   if (currentMicros - previousMicros>((modInterval/2)/calFactor))          
       { previousMicros = currentMicros;                                                    
        if (pulseState == LOW) pulseState = HIGH; else pulseState = LOW;
        //digitalWrite(13, pulseState);         //to blink onboard LED                                                         
        //digitalWrite(speedometerPin, pulseState);
        //speedReading.setData(mph,2);  
        //speedB = 255 * (mph/5); // Divide by five to get a range of values from 0 to 1
        //speedR = (255/3) * (mph/5); // (255/3) because we don't want the full intensity of red.
        //sprintf(colorA, "%02X%02X%02X%02X", speedA, speedR, speedG, speedB);
        //speedReading.setColor(colorA); // Change display box to new colour. 
        //speedReading.update(); // We're finally doing the updating here
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
            mph = (circ / interval) * 2.2369;
            //speedReading.setData(mph,2);  
            //speedB = 255 * (mph/5); // Divide by five to get a range of values from 0 to 1
            //speedR = (255/3) * (mph/5); // (255/3) because we don't want the full intensity of red.
            //sprintf(colorA, "%02X%02X%02X%02X", speedA, speedR, speedG, speedB);
            //speedReading.setColor(colorA); // Change display box to new colour. 
            //speedReading.update(); // We're finally doing the updating here                  
           }
      }
}

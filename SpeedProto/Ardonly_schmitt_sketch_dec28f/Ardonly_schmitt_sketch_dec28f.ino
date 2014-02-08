// eBike SpeedTuner by Rim21 using extracts from - Bike computer by Chris Purola and Annikken Andee Tut6

// Sensor setup
const int debounce = 6000; //equates to 6 milliseconds;
const int speedPin = 7;                    
const int sensorPin = 3;                                     
int pulseState = LOW;

//Calibration variables                                        
volatile unsigned long currentMicros = 0; 
volatile unsigned long previousMicros= 0;  
volatile unsigned long currentSpeed = 0;  
volatile unsigned long previousSpeed = 0;
volatile unsigned long interval = 0;                 
unsigned long modInterval = 0;                
float calFactor = .85;   // decrease to slow down speedometer                           
                         // calFactor of 1 makes no change to speedometer



void setup()
{
                        
  pinMode(speedPin, OUTPUT);    
  pinMode(sensorPin, INPUT);                 
  digitalWrite (sensorPin, HIGH);
  
  attachInterrupt (0, sense, FALLING);        
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
       {previousMicros = currentMicros;                                                    
        if (pulseState == LOW) pulseState = HIGH;
        else pulseState = LOW;                                              
        digitalWrite(speedPin, pulseState);
        
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

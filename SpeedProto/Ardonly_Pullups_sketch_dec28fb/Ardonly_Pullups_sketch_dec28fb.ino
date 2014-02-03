// eBike SpeedTuner by Rim21 using extracts from - Bike computer by Chris Purola and Annikken Andee Tut6
//This sketch is for a Leo board as it uses a different external interrupt to a Uno

// Sensor setup
const int debounce = 6000; //equates to 6 milliseconds;
const int speedPin = 7;                    
const int sensorPin = 3;                                     
int pulseState = LOW;

// set surrounding pin numbers:
const int aPin = 0;
const int bPin = 1;
const int cPin = 2;
const int dPin = 4;
const int ePin = 5;
const int fPin = 6;

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

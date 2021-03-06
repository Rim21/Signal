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

const int SensorPin = 3;                //Input pin for speed sensor
unsigned long tripTime;                 //last time sensor was tripped
unsigned long lastTripTime;             //time before last sensor was tripped
unsigned long revs = 0;                 //number of revs
unsigned int milsecs;                   //this will overflow after 64 seconds or so between hits
float outspeed;                        //speed output to screen
//unsigned long totalmm;                  //total millimeters
float mph = 0;                          //current calculated speed
unsigned int avg[4];                    //array for averaging speed
volatile boolean state = false;         //check for if sensor has been tripped
unsigned long displayTimer;             //last screen update

void setup()
{
  pinMode (SensorPin,INPUT);            //enable input sensor
  digitalWrite (SensorPin, HIGH);       //turn on pull up resistor
  Andee.begin(); // Sets up the communication link between Arduino and the Annikken Andee
  setInitialData();
  Andee.clear(); // Clear the screen of any previous displays
  attachInterrupt(1,sense,FALLING);     //Setup interrupt for wheel sensor
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
  if(state == true && (millis() - tripTime) > 40)    //check if the sensor has been tripped, and also debounce the input (40ms = approx 250mph).
  {
    tripTime = millis();                //set the time the interrupt occured
    revs = revs + 1;                    //update the revs number
    calc();                             //recalculate the current stats
  }

  if(state == true)                     //if it's still true...
    state = false;                      //falsify state    
}

void calc() 
{
  milsecs = tripTime - lastTripTime;    //calculate the difference between the current time and the last tripped time
  lastTripTime = tripTime;              //reset the last tripped time
  mph = (circ / milsecs) * 2.2369;      //calculate the speed
  avg[0] = avg[1];                      //shift storage array
  avg[1] = avg[2];                      //    "
  avg[2] = avg[3];                      //    "
  avg[3] = int(mph);                    //insert current calculated mph
  if ((millis() - displayTimer) >= 500) //run the display routine every 500ms if the wheel is spinning
    {
      displayTimer = millis();
      displaySpeed();
    }
}

void displaySpeed()
{
  outspeed = ((avg[0] + avg[1] + avg[2] + avg[3]) / 4);    //average the speed over the last four readings (mean)
      speedReading.setData(outspeed,2);  
      speedB = 255 * (outspeed/5); // Divide by five to get a range of values from 0 to 1
      speedR = (255/3) * (outspeed/5); // (255/3) because we don't want the full intensity of red.
      sprintf(colorA, "%02X%02X%02X%02X", speedA, speedR, speedG, speedB);
      speedReading.setColor(colorA); // Change display box to new colour.
      speedReading.update(); // We're finally doing the updating here  
}

void sense()
{
  state = true;
}

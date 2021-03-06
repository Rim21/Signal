//Program outputs speed to a display
#include <SPI.h>
#include <Andee.h>

AndeeHelper speedo; //Declare an instance of AndeeHelper to help manage our display.
int count;
unsigned char flag;
//const double radius = 21.0; //cm
//const double pi = 3.142;
double wheelCircumference;
double calSpeed;
unsigned long time, pTime, tDiff, loopTime;
double tDiffHr;
double totalDistance;

void setup(){
  Andee.begin();  //Setup communications between Annikken Andee and Arduino
  initAndee();    //Init Andee MemBlocks
  Andee.clear();  //Clear the screen of any previous displays
  
  pinMode(2, INPUT); //I connect my speed sensor to pin2, note pin2 = interrupt 0 and pin3 = interrupt 1
  digitalWrite(2, HIGH); //I enabled the pullup resistor attached to this pin2
  attachInterrupt(0, isr, FALLING); //to capture when the sensor changes
  
  count = 0; //count is the number of times magnet on wheel cuts sensor.

  flag = 0;

  speedo.updateData(0); //init with speed 0
  wheelCircumference = 37.8/100000; //2268mm to cm / 6 pulses
  totalDistance = 0.0;
  loopTime = millis();
}

void loop(){
  //flag will be set inside isr(void)
  //If so we calculate speed based on the time wheel takes to make one revolution
  if(flag){
    flag = 0;
    if(count > 1){    
      tDiff = calculateTime(time); //time is being recorded already in the isr(void)
      totalDistance += wheelCircumference;
      
      tDiffHr = ((double)tDiff)/3600000;
      
      calSpeed = wheelCircumference/tDiffHr;
      speedo.updateData(calSpeed, 2);
    }
    pTime = time;
  }
  else{
    if(millis()-pTime > 2000){
      pTime = millis();
      speedo.updateData("0.00");
    }
  }
}

//When sensor pin makes a High to Low, isr(void) will be called. 
void isr(void){
  if(digitalRead(2)==LOW){
    count++;
    flag = 1;
    time = millis();
  }
}

unsigned long calculateTime(unsigned long nTime){
  unsigned long timeDiff;
  timeDiff = nTime-pTime;
  return timeDiff;
}

void initAndee(){
  speedo.setId(0);  //Associate andeeHelper with MemBlock 0 of Annikken Andee.
  speedo.setType(DATA_OUT);  //Set Type to DATA_OUT, BUTTON_IN or KEYBOARD_IN display
  speedo.setLocation(0, 0, FULL);  //Set row to appear and size. setLocation(int row, int order, int span)
  
  //Set the title, data and units of display. 
  speedo.setTitle("Speed");
  speedo.setData("");
  speedo.setUnit("km/h");
}

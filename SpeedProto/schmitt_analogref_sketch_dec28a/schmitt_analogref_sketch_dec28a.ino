// constants won't change. They're used here to 
// set pin numbers:
const int sensorPin = 2;     // the sensor input pin
const int speedPin = 7;      // the modified sensor output pin

// Variables will change:
int speedState = HIGH;       // the current state of the output pin
int sensorState = HIGH;      // the current reading from the input pin
int lastsensorState = LOW;   // the previous reading from the input pin

void setup() {
  // put your setup code here, to run once:
  
  pinMode(sensorPin, INPUT);
  pinMode(speedPin, OUTPUT);

  // set initial speed state
  digitalWrite(speedPin, speedState);
  
  // turn on pull-up to sensor pin state
  digitalWrite(sensorPin, sensorState);
 
  // external reference from eBike power circuit
  analogReference(EXTERNAL);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //Variables for RC-Schmitt Trigger emulator debounce algorithm
uint8_t y_old=0, temp=0;
boolean buttonPressed = false;

  //Sensor polling loop
  while(true){
      
    //Grab pin state
    boolean pinState = PIND & ( 1 << 2 ); //Reading pin state  directly off register (pin PD2 / digital 2)
    pinState = !pinState; // The sensor grounds the pin, which is otherwise pulled high, so we flip it
                          // to achieve TRUE = sensor
  

    //This is the "RC circuit" part of the emulator, a recursive low-pass filter of recent pin states
    //Equation: y_old = (pinState * 0.25) + (y_old * 0.75)
    
    //First we get (y_old * 0.75)
    temp = (y_old >> 2);     //Bitwise operation for (y_old / 4)
    y_old -= temp;           // y_old - (y_old/4) is equivalent to (y_old * 0.75)
    
        //Second we add (pinState * 0.25) to y_old, but only if pinState is 1 (since otherwise the product is 0)
        //Note that we are actually adding 63 rather than (1*0.25). This is because we are using a byte
        //to represent the pin state in the equation for the purpose of having sufficient resolution to 
        //later apply the hysteresis of the virtual Schmitt trigger. 63 is the integer result of multiplying
        //255 (maximum 8-bit number) by 0.25. 
    if(pinState) y_old += 0x3f;  //0x3f is hex for 63


    //This is the Schmitt trigger part of the algorithm. Note the hysteresis -- different thresholds for 
    //detecting button-down and button-up 
        //Thresholds are 240 (for a valid sensor read low) and 15 (for a valid sensor read high) [out of 255]
        //(These may be tuned to the particular sensor being debounced)   
    if( (y_old > 0xf0) && (!buttonPressed) ){  //This means a real sensor low transition has occurred
        buttonPressed = true;
       // add in a debug sequence here later
        PORTD |= ( 1 << 7);  //This puts a high signal to digital port 7
    }
    else if( (y_old < 0x0f) && (buttonPressed) ){  //This means a real sensor high transition has occurred
        buttonPressed = false;
        PORTD &= ~(1 << 7); //This turns off the the signal to digital port 7)
    }
        
    delay(4); //The algorithm specifies that the above routine be called by a timer interrupt every 
              //4-5 ms. With the default Schmitt thresholds of 0x0f (15) and 0xf0 (240), a delay of 4 ms
              //here results in a debouncing period of approximately 50 ms (it is not exact because
              //this is not a pure timing algorithm, so depending on actual bounce, the period may vary
              //somewhat
}
}

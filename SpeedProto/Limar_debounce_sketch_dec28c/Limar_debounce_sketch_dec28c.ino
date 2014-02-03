/* 
 Debounce
 
 Each time the input pin goes from LOW to HIGH, the output pin is toggled
 from LOW to HIGH or HIGH to LOW.  There's a minimum delay between toggles
 to debounce the circuit (i.e. to ignore noise).
 */

// constants won't change. They're used here to 
// set pin numbers:
const int sensorPin = 3;    // the number of the pushbutton pin
const int speedPin = 7;     // the number of the LED pin

// Variables will change:
int speedState = HIGH;       // the current state of the output pin
int sensorState;             // the current reading from the input pin
int lastsensorState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 10;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(speedPin, OUTPUT);

  // set initial speed state
  digitalWrite(speedPin, speedState);
  digitalWrite(sensorPin, speedState);
}

void loop() {
  // read the state of the sensor into a local variable:
  int reading = digitalRead(sensorPin);

  // check to see if the sensor has registered 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastsensorState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the sensor state has changed:
    if (reading != sensorState) {
      sensorState = reading;

      // only toggle the speed out if the new sensor state is HIGH
      if (sensorState == HIGH) {
        speedState = !speedState;
      }
    }
  }
  
  // set the speed
  digitalWrite(speedPin, speedState);

  // save the reading.  Next time through the loop,
  // it'll be the lastsensorState:
  lastsensorState = reading;
}

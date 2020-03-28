/*
  This sketch uses a single RC input into pwm pin 6.

  This program should run in two modes

  Mode 1 - Steering LEDs light up when rc remote steering wheel is centered, or turns left and right
  Mode 2 - LEDS Disabled, No LEDs light up when steering wheel is turned

  There is a Potentiometer on input A0 that should mimic the driving mode switch that is in the car.

  If the Potentiometer value is less than or equal to 1000, this should invoke Mode 1 -
  This mimics the driving mode switch being set to rc mode

  Mode 1 function
  If the steering is neutral, where the RC input is PWM between 1400 and 1500 - The Yellow LED should light up
  If the steering is full left, where the RC input is PWM less than 1400 - The Green LED should light up
  If the steering is full right, where the RC input is PWM greater than 1500 - the Red LED should light up


  If the Potentiometer value is greater than 1000, this should invoke Mode 2 -
  This mimics the driving mode switch in the car being set to child guided mode

  Mode 2 function
  No LEDS should turn on

  NOTE: No Override mode is currently in place.
*/

/* ------------ Averaging -- */
// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int inputPin = A0;
/* ----------- End of averaging ------- */


int rcsteering;
int rcthrottle;
int rcdrivemode;

int sensorValue = 0;

int RedLedPin = 13;
int YellowLedPin = 12;
int GreenLedPin = 8;

void setup() {     // put your setup code here, to run once:


/* -------- averaging ------ */

  // initialize serial communication with computer:
  Serial.begin(9600);
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

/* -------- end of averaging ------- */


  // Initialize RC inputs
  // pinMode(3, INPUT); // RC Throttle input
  pinMode(6, INPUT); // RC Steering input
  pinMode(5, INPUT); // RC Driving Mode input

  //Initialize Manual inputs
  pinMode(A0, INPUT);

  // Initialize LED Outputs
  pinMode(RedLedPin, OUTPUT); // Red
  pinMode(YellowLedPin, OUTPUT); // Yellow
  pinMode(GreenLedPin, OUTPUT); // Green

  // Initialize Servo output

  Serial.begin(9600);
  Serial.println("Initialise");
}

void loop() {
  // put your main code here, to run repeatedly:


/* ------------------------ */

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  Serial.println(average);
  //delay(1);        // delay in between reads for stability

/* ------------------------ */

  
  rcsteering = pulseIn(6, HIGH, 25000);
  rcdrivemode = pulseIn(5, HIGH, 25000); // controlled by Potentiometer, but will be a switch in future.
  //Serial.println(sensorValue);

  if (average <= 1000) // Mode 1 - LEDS should light up when steering is turned right and left
  {
    if (rcsteering <= 1400)
    {
      digitalWrite(GreenLedPin, HIGH);
      digitalWrite(YellowLedPin, LOW);
      digitalWrite(RedLedPin, LOW);
    };

    if (rcsteering >= 1400 && rcsteering <= 1500)
    {
      digitalWrite(YellowLedPin, HIGH);
      digitalWrite(GreenLedPin, LOW);
      digitalWrite(RedLedPin, LOW);
      Serial.println("Yello");
      Serial.println(sensorValue);
    };

    if (rcsteering >= 1500)
    {
      digitalWrite(RedLedPin, HIGH);
      digitalWrite(GreenLedPin, LOW);
      digitalWrite(YellowLedPin, LOW);

    }
  }
  else // Mode 2 - No LEDS should light up
  {
    digitalWrite(YellowLedPin, LOW);
    digitalWrite(GreenLedPin, LOW);
    digitalWrite(RedLedPin, LOW);
  };
}

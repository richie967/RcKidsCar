#include <Servo.h>

const int PIN_OUTPUT_STEERING = 10;
const int PIN_OUTPUT_THROTTLE = 11;

Servo throttleESC;
Servo steeringServo;

void configureOutput()
{
//  configureSerialOutput();
  configureThrottleOutput();
  configureSteeringOutput();
}

void configureSerialOutput()
{
  Serial.begin(9600);
  Serial.println("Arduino Car Started. Brmmm Brmmm.");
}

void configureThrottleOutput()
{
  // initialize the throttle output
  throttleESC.attach(PIN_OUTPUT_THROTTLE, 1000, 2000);
}

void configureSteeringOutput()
{
  // initialize and centre the steering
  steeringServo.attach(PIN_OUTPUT_STEERING, 1000, 2000);
  steeringServo.write(90);
}

void refreshThrottleOutput(int throttle, Enums::GearSelection gearSelection)
{  
  // set the power servo state
  int throttleOut = 90;
  
  if (gearSelection == Enums::GearSelection::Forward)
    throttleOut = map(currentState.Throttle, 0, 100, 90, 180);
  else if (gearSelection == Enums::GearSelection::Reverse)
    throttleOut = map(currentState.Throttle, 0, 100, 90, 0);

  throttleESC.write(throttleOut);
//  Serial.print("Throttle: ");
//  Serial.println(throttleOut);
}

void refreshSteeringOutput(int angle)
{
  steeringServo.write(currentState.SteeringAngle);
//  Serial.print("Steering: ");
//  Serial.println(angle);
}

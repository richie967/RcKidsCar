#include <Servo.h>

const int PIN_OUTPUT_STEERING = 10;
const int PIN_OUTPUT_THROTTLE = 11;

Servo steeringServo;
Servo throttleESC;

volatile int throttleLast = 90;

void configureOutput()
{
//  configureSerialOutput();
  configureSteeringOutput();
  configureThrottleOutput();
}

void configureSerialOutput()
{
  Serial.begin(9600);
  Serial.println("Arduino Car Started. Brmmm Brmmm.");
}

void configureSteeringOutput()
{
  // initialize and centre the steering
  steeringServo.attach(PIN_OUTPUT_STEERING, 1000, 2000);
  steeringServo.write(90);
}

void configureThrottleOutput()
{
  // initialize the throttle output
  throttleESC.attach(PIN_OUTPUT_THROTTLE, 1000, 2000);
}

void refreshSteeringOutput(int angle)
{
  steeringServo.write(currentState.SteeringAngle);
}

void refreshThrottleOutput(Enums::GearSelection gearSelection, int throttle, int throttleRestrictionFactor)
{  
  // set the power servo state
  int throttleOut = 90;

  if (gearSelection == Enums::GearSelection::Forward)
  {
    // reduce throttle by restriction factor
    int restrictedThrottle = (throttle / 100.0) * throttleRestrictionFactor;
    throttleOut = map(restrictedThrottle, 0, 100, 90, 180);
  }
  else if (gearSelection == Enums::GearSelection::Reverse)
    throttleOut = map(throttle, 0, 100, 90, 0);

  writeThrottle(throttleOut);
}

void applyBrake()
{
  if (throttleLast <= 90)
    return;
  
  writeThrottle(0);
  delay(100);
  writeThrottle(90);
}

void writeThrottle(int throttle)
{
  throttleLast = throttle;
  throttleESC.write(throttle);
}

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

void refreshThrottleOutput(Enums::GearSelection gearSelection, int throttle, int throttleRestrictionFactor)
{
  // reduce throttle by restriction factor
  int restrictedThrottle = (throttle / 100) * throttleRestrictionFactor;
  
  // set the power servo state
  int throttleOut = 90;

  // if throttle output is now 0 and we are travelling forward, briefly apply brakes (full power reverse)
  if (restrictedThrottle == 0 && gearSelection == Enums::GearSelection::Forward)
  {
    throttleESC.write(0);
    delay(10);
  }
  
  if (gearSelection == Enums::GearSelection::Forward)
    throttleOut = map(restrictedThrottle, 0, 100, 90, 180);
  else if (gearSelection == Enums::GearSelection::Reverse)
    throttleOut = map(restrictedThrottle, 0, 100, 90, 0);

  throttleESC.write(throttleOut);
}

void refreshSteeringOutput(int angle)
{
  steeringServo.write(currentState.SteeringAngle);
}

#include <Servo.h>

const int PIN_OUTPUT_STEERING = 10;
const int PIN_OUTPUT_THROTTLE = 11;

Servo throttleESC;
Servo steeringServo;

volatile int throttleLast;
volatile int steeringLast;

void configureOutput()
{
//  configureSerialOutput();
  configureThrottleOutput();
  configureSteeringOutput();
}

void refreshOutput()
{
//  refreshSerialOutput();
  refreshThrottleOutput();
  refreshSteeringOutput();
}

void configureSerialOutput()
{
  Serial.begin(9600);
  Serial.println("Arduino Car Started. Brmmm Brmmm.");
}

void configureThrottleOutput()
{
  throttleESC.attach(PIN_OUTPUT_THROTTLE, 1000, 2000);
}

void configureSteeringOutput()
{
  steeringServo.attach(PIN_OUTPUT_STEERING, 1000, 2000);
}

void refreshSerialOutput()
{
  // update output
  Serial.println("");

  Serial.print("Remote status: ");
  Serial.println(static_cast<int>(currentState.RemoteStatus));

  Serial.print("Remote control mode: ");
  Serial.println(static_cast<int>(currentState.RemoteControlMode));

  Serial.print("Internal control mode: ");
  Serial.println(static_cast<int>(currentState.InternalControlMode));

  Serial.print("Control device: ");
  Serial.println(static_cast<int>(currentState.ControlDevice));

  Serial.print("Proximity: ");
  Serial.println(currentState.Proximity);

  Serial.print("Gear selection: ");
  Serial.println(static_cast<int>(currentState.GearSelection));

  Serial.print("Throttle: ");
  Serial.println(currentState.Throttle);

  Serial.print("Steering angle: ");
  Serial.println(currentState.SteeringAngle);

  delay(2000);
}

void refreshThrottleOutput()
{
  int throttle = 90;
  
  // set the power servo state
  if (currentState.GearSelection == Enums::GearSelection::Forward)
    throttle = map(currentState.Throttle, 0, 100, 90, 180);
  else if (currentState.GearSelection == Enums::GearSelection::Reverse)
    throttle = map(currentState.Throttle, 0, 100, 90, 0);

  if (throttle == throttleLast)
    return;

  throttleLast = throttle;
  throttleESC.write(throttle);
}

void refreshSteeringOutput()
{
  if (currentState.SteeringAngle == steeringLast)
    return;

  steeringLast = currentState.SteeringAngle;
  steeringServo.write(currentState.SteeringAngle);
}

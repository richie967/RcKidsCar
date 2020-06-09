#include <Servo.h>

const int PIN_OUTPUT_STEERING = 10;
const int PIN_OUTPUT_THROTTLE = 11;

Servo throttleServo;

void configureOutput()
{
  configureSerialOutput();
//  configureThrottleOutput();
//  configureSteeringOutput();
}

void configureSerialOutput()
{
  Serial.begin(9600);
  Serial.println("Arduino Car Started. Brmmm Brmmm.");
}

void configureThrottleOutput()
{
  throttleServo.attach(PIN_OUTPUT_THROTTLE, 1000, 2000);
}

void configureSteeringOutput()
{
  // TODO
}

void refreshOutput()
{
  refreshSerialOutput();
//  refreshThrottleOutput();
//  refreshSteeringOutput();
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

  throttleServo.write(throttle);
}

void refreshSteeringOutput()
{
  // TODO
}

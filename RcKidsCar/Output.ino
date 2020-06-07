const int PIN_OUTPUT_THROTTLE = 3;

Servo powerServo;

void configureOutput()
{
  pinMode(PIN_INPUT_REMOTE_THROTTLE, INPUT);
  powerServo.attach(PIN_OUTPUT_THROTTLE, 1000, 2000);

  Serial.begin(9600);
  Serial.println("Arduino Car Started. Brmmm Brmmm.");
}

void refreshOutput()
{
  // set the power servo state
  if (currentState.GearSelection == Enums::GearSelection::Forward)
  {
    powerServo.write(map(currentState.Throttle, 0, 100, 90, 180));
  }
  else if (currentState.GearSelection == Enums::GearSelection::Reverse)
  {
    powerServo.write(map(currentState.Throttle, 0, 100, 90, 0));
  }
  else
  {
    powerServo.write(90);
  }

//  refreshSerialOutput();
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

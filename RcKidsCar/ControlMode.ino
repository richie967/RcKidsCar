const int PIN_INPUT_INTERNAL_CONTROL_MODE = 7;

void configureControlMode()
{
  pinMode(PIN_INPUT_INTERNAL_CONTROL_MODE, INPUT);
  PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_CONTROL_MODE, controlModeChanged, CHANGE);
  
  controlModeChanged();
}

void controlModeChanged()
{
  int pinValue = digitalRead(PIN_INPUT_INTERNAL_CONTROL_MODE);
  Enums::ControlMode controlMode = static_cast<Enums::ControlMode>(pinValue);

  currentState.setInternalControlMode(controlMode);
}

const int PIN_INPUT_INTERNAL_CONTROL_MODE = 8;

void configureControlModeInternal()
{
  pinMode(PIN_INPUT_INTERNAL_CONTROL_MODE, INPUT);
  PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_CONTROL_MODE, refreshInternalControlMode, CHANGE);
  
  refreshInternalControlMode();
}

void refreshInternalControlMode()
{
  int pinValue = digitalRead(PIN_INPUT_INTERNAL_CONTROL_MODE);
  Enums::ControlMode controlMode = static_cast<Enums::ControlMode>(pinValue);

  currentState.setInternalControlMode(controlMode);
}

const int PIN_INPUT_INTERNAL_CONTROL_MODE = 8;
const int PIN_INPUT_REMOTE_CONTROL_MODE = 5;

void configureControlMode()
{
  pinMode(PIN_INPUT_INTERNAL_CONTROL_MODE, INPUT);
  pinMode(PIN_INPUT_REMOTE_CONTROL_MODE, INPUT);
}

ControlMode readRemoteControlMode()
{
  // PWM signal from remote sends one of three values 0 (remote not connected), ~1000 (LOW, switch off), ~2000 (HIGH, switch on)
  int pinValue = pulseIn(PIN_INPUT_REMOTE_CONTROL_MODE, HIGH, PWM_RECEIVE_TIMEOUT);
  
  if (pinValue >= REMOTE_HIGH_VALUE_MINIMUM && pinValue <= REMOTE_HIGH_VALUE_MAXIMUM)
  {
    return ControlMode::Remote;
  }

  return ControlMode::Internal;
}

ControlMode readInternalControlMode()
{
  int pinValue = digitalRead(PIN_INPUT_INTERNAL_CONTROL_MODE);
  return static_cast<ControlMode>(pinValue);
}

ControlDevice determineControlDevice()
{
  switch (currentState.RemoteStatus)
  {
    case RemoteStatus::Failsafe:
      return ControlDevice::None;
      break;
    case RemoteStatus::Enabled:
      return (currentState.RemoteControlMode == ControlMode::Remote) ? ControlDevice::Remote : ControlDevice::Internal;
      break;
    case RemoteStatus::Disabled:
      return (currentState.InternalControlMode == ControlMode::Internal) ? ControlDevice::Internal : ControlDevice::None;
      break;
  }
}

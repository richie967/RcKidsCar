void handleRemoteStatus(int remoteStatusValue)
{
  if (remoteStatusValue == 0)
    currentState.setRemoteStatus(Enums::RemoteStatus::Disabled);
  else if (remoteStatusValue >= REMOTE_LOW_VALUE_MINIMUM && remoteStatusValue <= REMOTE_LOW_VALUE_MAXIMUM)
    currentState.setRemoteStatus(Enums::RemoteStatus::Failsafe);
  else
    currentState.setRemoteStatus(Enums::RemoteStatus::Enabled);
}

void handleRemoteSteering(int steeringValue)
{
  // ignore if remote is not the control device
  if (currentState.ControlDevice != Enums::ControlDevice::Remote)
    return;

  int steeringAngle = STEERING_ANGLE_CENTRE;
  
  // ignore jitter on the pulse value when no steering angle is applied
  if (steeringValue < REMOTE_MIDDLE_VALUE_MINIMUM || steeringValue > REMOTE_MIDDLE_VALUE_MAXIMUM)
    steeringAngle = map(steeringValue, REMOTE_LOW_VALUE_MAXIMUM, REMOTE_HIGH_VALUE_MINIMUM, STEERING_ANGLE_MINIMUM, STEERING_ANGLE_MAXIMUM);
    
  currentState.setSteeringAngle(steeringAngle);
}

void handleRemoteThrottle(int throttleValue)
{
  // ignore if remote is not the control device
  if (currentState.ControlDevice != Enums::ControlDevice::Remote)
    return;
  
  if (throttleValue <= REMOTE_MIDDLE_VALUE_MINIMUM)
  {
    // negative throttle (reverse)
    int throttle = map(throttleValue, REMOTE_LOW_VALUE_MAXIMUM, REMOTE_MIDDLE_VALUE_MINIMUM, -100, 0) * -1;
    currentState.setThrottle(throttle);
    currentState.GearSelection = Enums::GearSelection::Reverse;
  }
  else if (throttleValue >= 1500)
  {
    // positive throttle (forward)
    int throttle = map(throttleValue, REMOTE_MIDDLE_VALUE_MAXIMUM, REMOTE_HIGH_VALUE_MINIMUM, 0, 100);
    currentState.setThrottle(throttle);
    currentState.GearSelection = Enums::GearSelection::Forward;
  }
  else
  {
    currentState.setThrottle(0);
    currentState.GearSelection = Enums::GearSelection::Neutral;
  }
}

void handleRemoteControlMode(int controlModeValue)
{
  Enums::ControlMode controlMode = controlModeValue >= REMOTE_HIGH_VALUE_MINIMUM
    ? Enums::ControlMode::Remote
    : Enums::ControlMode::Internal;
  
  currentState.setRemoteControlMode(controlMode);
}

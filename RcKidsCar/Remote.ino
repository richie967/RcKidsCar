const int PIN_INPUT_REMOTE = 6;
const int PPM_PULSE_MAX_LENGTH = 3000;

unsigned long channel[6], timer[7];
int pulse;

// define the ppm channel handlers, position in the array = offset channel number
// e.g. position 0 is channel 1, position 1 is channel 2, etc.
void (* channelHandlers [6])(int) = { handleRemoteSteering, handleRemoteThrottle, handleRemoteControlMode, handleRemoteStatus, NULL, NULL };

void configureRemote()
{
  PCintPort::attachInterrupt(PIN_INPUT_REMOTE, receiveRemotePulse, RISING);
}

void receiveRemotePulse()
{
  // read the time in microseconds for the current pulse
  timer[pulse] = micros();

  // if pulse is 0 there's no previous pulse to compare the value with
  if (pulse == 0)
  {
    pulse++;
    return;
  }

  // store the channel value as the difference between the current pulse and the last
  channel[pulse] = timer[pulse] - timer[pulse - 1];

  // if there is a synchronisation issue (we're reading a long interval when we shouldn't), reset to start reading pulse 1 on the next pulse
  if (channel[pulse] > PPM_PULSE_MAX_LENGTH)
  {
    timer[0] = timer[pulse];
    pulse = 1;
    return;
  }
    
  // now call the handler for the specific channel if we have one
  if (channelHandlers[pulse])
  {
    channelHandlers[pulse](channel[pulse - 1]);
  }

  pulse++;
}

void handleRemoteStatus(int remoteStatusValue)
{
  if (remoteStatusValue == 0)
  {
    currentState.setRemoteStatus(Enums::RemoteStatus::Disabled);
  }
  else if (remoteStatusValue >= REMOTE_LOW_VALUE_MINIMUM && remoteStatusValue <= REMOTE_LOW_VALUE_MAXIMUM)
  {
    currentState.setRemoteStatus(Enums::RemoteStatus::Failsafe);
  }
  else
  {
    currentState.setRemoteStatus(Enums::RemoteStatus::Enabled);
  }
}

void handleRemoteSteering(int steeringValue)
{
  // ignore if remote is not the control device
  if (currentState.ControlDevice != Enums::ControlDevice::Remote)
  {
    return;
  }
  
  // ignore jitter on the pulse value when no steering angle is applied
  if (steeringValue >= REMOTE_MIDDLE_VALUE_MINIMUM && steeringValue <= REMOTE_MIDDLE_VALUE_MAXIMUM)
  {
    return STEERING_ANGLE_CENTRE;
  }
  
  int steeringAngle = map(steeringValue, REMOTE_LOW_VALUE_MAXIMUM, REMOTE_HIGH_VALUE_MINIMUM, STEERING_ANGLE_MINIMUM, STEERING_ANGLE_MAXIMUM);
  currentState.setSteeringAngle(steeringAngle);
}

void handleRemoteThrottle(int throttleValue)
{
  // ignore if remote is not the control device
  if (currentState.ControlDevice != Enums::ControlDevice::Remote)
  {
    return;
  }
  
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
  if (controlModeValue >= REMOTE_HIGH_VALUE_MINIMUM && controlModeValue <= REMOTE_HIGH_VALUE_MAXIMUM)
  {
    currentState.setRemoteControlMode(Enums::ControlMode::Remote);
    return;
  }

  currentState.setRemoteControlMode(Enums::ControlMode::Internal);
}

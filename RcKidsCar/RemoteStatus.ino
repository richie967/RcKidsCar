const int PIN_INPUT_REMOTE_FAILSAFE = 10;

void configureRemote()
{
  pinMode(PIN_INPUT_REMOTE_FAILSAFE, INPUT);
}

RemoteStatus readRemoteStatus()
{
  // when remote receiver started without remote connected remote receiver outputs 0, otherwise LOW, MIDDLE, HIGH values as described in constants
  // failsafe value is LOW
  int pinValue = pulseIn(PIN_INPUT_REMOTE_FAILSAFE, HIGH, PWM_RECEIVE_TIMEOUT);
  
  if (pinValue == 0)
  {
    return RemoteStatus::Disabled;
  }
  else if (pinValue >= REMOTE_LOW_VALUE_MINIMUM && pinValue <= REMOTE_LOW_VALUE_MAXIMUM)
  {
    return RemoteStatus::Failsafe;
  }

  return RemoteStatus::Enabled;
}

const int PIN_INPUT_INTERNAL_THROTTLE = A0;
const int INTERNAL_THROTTLE_VALUE_MINIMUM = 250;
const int INTERNAL_THROTTLE_VALUE_MAXIMUM = 750;

void configureInternalThrottle()
{
  pinMode(PIN_INPUT_INTERNAL_THROTTLE, INPUT);
}

int readThrottlePosition()
{
  if (currentState.ControlDevice == ControlDevice::None)
  {
    return 0;
  }

  return (currentState.ControlDevice == ControlDevice::Internal) ? readThrottlePositionInternal() : readThrottlePositionRemote();
}

int readThrottlePositionInternal()
{
  int throttleValue = analogRead(PIN_INPUT_INTERNAL_THROTTLE);
  return map(throttleValue, INTERNAL_THROTTLE_VALUE_MINIMUM, INTERNAL_THROTTLE_VALUE_MAXIMUM, 0, INTERNAL_THROTTLE_MAXIMUM_OUTPUT);
}

int readThrottlePositionRemote()
{
  int throttleValue = pulseIn(PIN_INPUT_REMOTE_THROTTLE, HIGH);

  if (throttleValue <= REMOTE_MIDDLE_VALUE_MINIMUM)
  {
    // negative throttle (reverse)
    return map(throttleValue, REMOTE_LOW_VALUE_MAXIMUM, REMOTE_MIDDLE_VALUE_MINIMUM, -100, 0) * -1;
  }
  else if (throttleValue >= 1500)
  {
    // positive throttle (forward)
    return map(throttleValue, REMOTE_MIDDLE_VALUE_MAXIMUM, REMOTE_HIGH_VALUE_MINIMUM, 0, 100);
  }

  return 0;
}

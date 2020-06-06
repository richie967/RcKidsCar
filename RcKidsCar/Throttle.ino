int readThrottlePosition()
{
  int throttlePosition = 0;
  int throttleMax = 100;

  if (currentState.ControlDevice == ControlDevice::None)
  {
    return 0;
  }
  else if (currentState.ControlDevice == ControlDevice::Internal)
  {
    throttleMax = INTERNAL_THROTTLE_MAXIMUM_OUTPUT;
    throttlePosition = readThrottlePositionInternalRaw();
  }
  else
  {
    throttlePosition = readThrottlePositionRemoteRaw();
  }

  if (throttlePosition < 0)
  {
    return 0;
  }
  else if (throttlePosition > throttleMax)
  {
    return throttleMax;
  }

  return throttlePosition;
}

int readThrottlePositionInternalRaw()
{
  int throttleValue = analogRead(PIN_INPUT_INTERNAL_THROTTLE);
  return map(throttleValue, INTERNAL_THROTTLE_VALUE_MINIMUM, INTERNAL_THROTTLE_VALUE_MAXIMUM, 0, INTERNAL_THROTTLE_MAXIMUM_OUTPUT);
}

int readThrottlePositionRemoteRaw()
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

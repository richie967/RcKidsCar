Rotary internalSteering = Rotary(PIN_INPUT_INTERNAL_STEERING_DATA, PIN_INPUT_INTERNAL_STEERING_CLOCK);

void toggleInternalSteering(bool enabled)
{
  // NOTE: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
  // can only use pins 2, 5, 7, 8, 10, 11, 12, 13 with CHANGE
  // data pin needs to be modified
  int internalSteeringDataInterruptId = digitalPinToInterrupt(PIN_INPUT_INTERNAL_STEERING_DATA);
  int internalSteeringClockInterruptId = digitalPinToInterrupt(PIN_INPUT_INTERNAL_STEERING_CLOCK);
  
  if (enabled)
  {
    attachInterrupt(internalSteeringDataInterruptId, internalSteeringRotate, CHANGE);
    attachInterrupt(internalSteeringClockInterruptId, internalSteeringRotate, CHANGE);
  }
  else
  {
    detachInterrupt(internalSteeringDataInterruptId);
    detachInterrupt(internalSteeringClockInterruptId);
  }
}

void readSteeringAngle()
{
  if (currentState.ControlDevice == ControlDevice::None)
  {
    currentState.setSteeringAngle(STEERING_ANGLE_CENTRE);
  }
  else if (currentState.ControlDevice == ControlDevice::Remote)
  {
    int angle = readSteeringAngleRemote();
    currentState.setSteeringAngle(angle);
  }
  // internal steering inputs handled by interrupts
}

void internalSteeringRotate()
{
  unsigned char result = internalSteering.process();
  if (result == DIR_CW) {
    currentState.incrementSteeringAngle();
  } else if (result == DIR_CCW) {
    currentState.decrementSteeringAngle();
  }
}

int readSteeringAngleRemote()
{
  int steeringValue = pulseIn(PIN_INPUT_REMOTE_STEERING, HIGH);

  // ignore jitter on the pulse value when no steering angle is applied
  if (steeringValue >= REMOTE_MIDDLE_VALUE_MINIMUM && steeringValue <= REMOTE_MIDDLE_VALUE_MAXIMUM)
  {
    return STEERING_ANGLE_CENTRE;
  }
  
  return map(steeringValue, REMOTE_LOW_VALUE_MAXIMUM, REMOTE_HIGH_VALUE_MINIMUM, STEERING_ANGLE_MINIMUM, STEERING_ANGLE_MAXIMUM);
}

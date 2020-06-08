const int PIN_INPUT_INTERNAL_STEERING_DATA = 2;
const int PIN_INPUT_INTERNAL_STEERING_CLOCK = 3;

Rotary internalSteering = Rotary(PIN_INPUT_INTERNAL_STEERING_DATA, PIN_INPUT_INTERNAL_STEERING_CLOCK);

void toggleSteeringInternal(bool enabled)
{
  // NOTE: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
  // see reference above for valid interrupt pins
  // data pin needs to be modified
  int dataInterruptId = digitalPinToInterrupt(PIN_INPUT_INTERNAL_STEERING_DATA);
  int clockInterruptId = digitalPinToInterrupt(PIN_INPUT_INTERNAL_STEERING_CLOCK);
  
  if (enabled)
  {
    attachInterrupt(dataInterruptId, steeringInternalRotate, CHANGE);
    attachInterrupt(clockInterruptId, steeringInternalRotate, CHANGE);
  }
  else
  {
    detachInterrupt(dataInterruptId);
    detachInterrupt(clockInterruptId);
  }
}

void steeringInternalRotate()
{
  unsigned char result = internalSteering.process();
  if (result == DIR_CW) {
    currentState.incrementSteeringAngle();
  } else if (result == DIR_CCW) {
    currentState.decrementSteeringAngle();
  }
}

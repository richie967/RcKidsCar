const int PIN_INPUT_INTERNAL_THROTTLE = A0;
const int INTERNAL_THROTTLE_VALUE_MINIMUM = 250;
const int INTERNAL_THROTTLE_VALUE_MAXIMUM = 750;

void pollThrottle()
{
  // bypass if we're not using internal controls
  if (currentState.ControlDevice != Enums::ControlDevice::Internal)
    return;
  
  int throttleValue = analogRead(PIN_INPUT_INTERNAL_THROTTLE);
  int throttle = map(throttleValue, INTERNAL_THROTTLE_VALUE_MINIMUM, INTERNAL_THROTTLE_VALUE_MAXIMUM, 0, INTERNAL_THROTTLE_MAXIMUM_OUTPUT);

  currentState.setThrottle(throttle);
}

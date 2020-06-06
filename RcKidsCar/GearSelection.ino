GearSelection readGearSelection()
{
  if (currentState.ControlDevice == ControlDevice::None)
  {
    return GearSelection::Neutral;
  }
  
  if (currentState.ControlDevice == ControlDevice::Internal)
  {
    return readGearSelectionInternal();
  }
  
  return readGearSelectionRemote();
}

GearSelection readGearSelectionInternal()
{
  int forwardPinValue = !digitalRead(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD);
  int reversePinValue = !digitalRead(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE);

  if (forwardPinValue)
  {
    return GearSelection::Forward;
  }
  else if (reversePinValue)
  {
    return GearSelection::Reverse;
  }

  return GearSelection::Neutral;
}

GearSelection readGearSelectionRemote()
{
  int throttleValue = pulseIn(PIN_INPUT_REMOTE_THROTTLE, HIGH);

  if (throttleValue > REMOTE_MIDDLE_VALUE_MAXIMUM)
  {
    return GearSelection::Forward;
  }
  else if (throttleValue < REMOTE_MIDDLE_VALUE_MINIMUM)
  {
    return GearSelection::Reverse;
  }

  return GearSelection::Neutral;
}

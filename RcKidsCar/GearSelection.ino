const int PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD = 0;
const int PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE = 1;

void configureGearSelectionInternal()
{
  pinMode(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD, INPUT_PULLUP);
  pinMode(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE, INPUT_PULLUP);
}

void toggleGearSelectionInternal(bool enabled)
{
  if (enabled)
  {
    PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD, refreshGearSelectionInternal, CHANGE);
    PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE, refreshGearSelectionInternal, CHANGE);
    refreshGearSelectionInternal();
  }
  else
  {
    PCintPort::detachInterrupt(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD);
    PCintPort::detachInterrupt(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE);
  }
}

void refreshGearSelectionInternal()
{
  int forwardPinValue = !digitalRead(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD);
  int reversePinValue = !digitalRead(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE);

  if (forwardPinValue)
  {
    currentState.GearSelection = Enums::GearSelection::Forward;
  }
  else if (reversePinValue)
  {
    currentState.GearSelection = Enums::GearSelection::Reverse;
  }
  else
  {
    currentState.GearSelection = Enums::GearSelection::Neutral;
  }
}

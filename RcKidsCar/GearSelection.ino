// pins 0 + 1 are used for serial communication, cannot be used if serial.begin is called
//const int PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD = 0;
//const int PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE = 1;
const int PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD = 12;
const int PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE = 13;

void configureGearSelection()
{
  pinMode(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD, INPUT_PULLUP);
  pinMode(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE, INPUT_PULLUP);

  toggleGearSelection(true);
}

void toggleGearSelection(bool enabled)
{
  if (enabled)
  {
    PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD, gearSelectionChanged, CHANGE);
    PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE, gearSelectionChanged, CHANGE);
    gearSelectionChanged();
  }
  else
  {
    PCintPort::detachInterrupt(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD);
    PCintPort::detachInterrupt(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE);
  }
}

void gearSelectionChanged()
{
  int forwardPinValue = !digitalRead(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD);
  int reversePinValue = !digitalRead(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE);

  if (forwardPinValue)
    currentState.GearSelection = Enums::GearSelection::Forward;
  else if (reversePinValue)
    currentState.GearSelection = Enums::GearSelection::Reverse;
  else
    currentState.GearSelection = Enums::GearSelection::Neutral;
}

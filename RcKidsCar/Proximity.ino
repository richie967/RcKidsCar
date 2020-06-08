const int PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER = 8;
const int PIN_INPUT_INTERNAL_PROXIMITY_ECHO = 9;

void configureProximityInternal()
{
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, OUTPUT);
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, INPUT);
}

void refreshProximity()
{
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, LOW);

  int duration = pulseIn(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, HIGH);

  currentState.Proximity = (duration / 2) * 0.0343;
}

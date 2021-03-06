const int PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER = 7;
const int PIN_INPUT_INTERNAL_PROXIMITY_ECHO = 11;

void configureInternalProximity()
{
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, OUTPUT);
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, INPUT);
}

int readProximity()
{
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, LOW);

  int duration = pulseIn(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, HIGH);

  return (duration / 2) * 0.0343;
}

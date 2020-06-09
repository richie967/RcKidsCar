const int PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER = 8;
const int PIN_INPUT_INTERNAL_PROXIMITY_ECHO = 9;

// https://www.instructables.com/id/Arduino-Timer-Interrupts/
// values for timer interrupt
// timer0, timer2 are 8 bit, max count value is 255
// timer1 is 16 bit, max count value is 65535
const int CORE_CLOCK_FREQUENCY_HZ = 16000000;
const int PROXIMITY_TIMER_MAX_COUNT = 255; // this is the max value for timer 1, 16 bit timer
const int PROXIMITY_TIMER_PRESCALE_FACTOR = 1024; // alternative values are 1, 8, 64, 256 (if changing this must also change TCCR1B register in configureProximityTimerInterrupt)

volatile int proximityPulseStart;
volatile bool proximityPulseStateLast;

void configureProximity()
{
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, OUTPUT);

  configureProximityTimerInterrupt();
  PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, proximityPulseChanged, CHANGE);
}

void configureProximityTimerInterrupt()
{
  // timer0 used by micros function that we need, timer1 used by servo library, so must use timer2
  // ensure configuration and counter registers are cleared
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;

  // set compare match register, results in an interrupt frequency of ~60Hz
  OCR2A = PROXIMITY_TIMER_MAX_COUNT;
  
  TCCR2A |= (1 << WGM21);                     // turn on CTC mode (clear timer when it matches the preset value)
  TCCR2B |= (1 << CS22) | (1 << CS20);        // Set CS22 and CS20 bits to enable 1024 prescaler
  TIMSK2 |= (1 << OCIE2A);                    // enable timer compare interrupt
}

//int calculateProximityTimerRegister()
//{
//  // calculate register count value
//  int registerValue = (CORE_CLOCK_FREQUENCY_HZ / (PROXIMITY_TIMER_PRESCALE_FACTOR * PROXIMITY_ECHO_FREQUENCY)) - 1; // there's an implicit cast from double to int here which can result in a slight innaccuracy
//
//  return (registerValue > PROXIMITY_TIMER_MAX_COUNT) ? PROXIMITY_TIMER_MAX_COUNT : registerValue;
//}

// in-built interrupt routine for timer 0 (servo library uses timer 1)
ISR(TIMER0_COMPA_vect)
{
  // timer interrupt triggered, send an ultrasonic pulse
  // echo will be captured by pin change interrupt on echo pin
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, LOW);
}

void proximityPulseChanged()
{
  int timer = micros();
  bool pulseState = digitalRead(PIN_INPUT_INTERNAL_PROXIMITY_ECHO);

  // determine if pin condition is different from the last time we saw it
  bool pulseStateChanged = pulseState != proximityPulseStateLast;

  // keep a record of the current pin state
  proximityPulseStateLast = pulseState;

  // if we've missed a pulse somehow, try again next time
  if (!pulseStateChanged)
    return;

  if (pulseState == HIGH)
  {
    // pin has gone high, start timing
    proximityPulseStart = timer;
    return;
  }

  // pin has gone low, calculate distance
  int duration = micros() - proximityPulseStart;
  currentState.Proximity = (duration / 2) * 0.0343; // 0.0343 is speed of sound in cm/microsecond
}

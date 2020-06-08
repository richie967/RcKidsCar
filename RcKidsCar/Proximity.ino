const int PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER = 8;
const int PIN_INPUT_INTERNAL_PROXIMITY_ECHO = 9;

// https://www.instructables.com/id/Arduino-Timer-Interrupts/
// values for timer interrupt
// timer0, timer2 are 8 bit, max count value is 255
// timer1 is 16 bit, max count value is 65535
const int CORE_CLOCK_FREQUENCY_HZ = 16000000;
const int PROXIMITY_TIMER_MAX_COUNT = 255; // this is the max value for timer 1, 16 bit timer
const int PROXIMITY_TIMER_PRESCALE_FACTOR = 1024; // alternative values are 1, 8, 64, 256 (if changing this must also change TCCR1B register in configureProximityTimerInterrupt)
const int PROXIMITY_ECHO_FREQUENCY = 64; // minimum frequency for 8 bit timer is 64Hz (64 intervals per second)

volatile int proximityPulseStart;

void configureProximityInternal()
{
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, OUTPUT);
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, INPUT);

  configureProximityTimerInterrupt();
  PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, proximityPulseStarted, RISING);
}

void configureProximityTimerInterrupt()
{
  // ensure configuration and counter registers are cleared
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;

  // set compare match register
  OCR0A = calculateProximityTimerRegister();
  
  TCCR0A |= (1 << WGM01);                     // turn on CTC mode (clear timer when it matches the preset value)
  TCCR0B |= (1 << CS02) | (1 << CS00);        // Set CS02 and CS00 bits to enable 1024 prescaler
  TIMSK0 |= (1 << OCIE0A);                    // enable timer compare interrupt
}

int calculateProximityTimerRegister()
{
  // calculate register count value
  int registerValue = (CORE_CLOCK_FREQUENCY_HZ / (PROXIMITY_TIMER_PRESCALE_FACTOR * PROXIMITY_ECHO_FREQUENCY)) - 1; // there's an implicit cast from double to int here which can result in a slight innaccuracy

  return (registerValue > PROXIMITY_TIMER_MAX_COUNT) ? PROXIMITY_TIMER_MAX_COUNT : registerValue;
}

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

void proximityPulseStarted()
{
  proximityPulseStart = micros();
  PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, proximityPulseComplete, FALLING);
}

void proximityPulseComplete()
{
  int duration = micros() - proximityPulseStart;
  currentState.Proximity = (duration / 2) * 0.0343; // 0.0343 is speed of sound in cm/microsecond
  
  PCintPort::attachInterrupt(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, proximityPulseStarted, RISING);
}

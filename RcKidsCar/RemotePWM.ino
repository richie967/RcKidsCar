#ifndef USE_REMOTE_PPM

const int PIN_INPUT_REMOTE_STEERING = 4;      // RC channel 1
const int PIN_INPUT_REMOTE_THROTTLE = 5;      // RC channel 2
const int PIN_INPUT_REMOTE_CONTROL_MODE = 6;  // RC channel 3
const int PIN_INPUT_REMOTE_FAILSAFE = 7;      // RC channel 4

volatile int remotePin[4] = { PIN_INPUT_REMOTE_STEERING, PIN_INPUT_REMOTE_THROTTLE, PIN_INPUT_REMOTE_CONTROL_MODE, PIN_INPUT_REMOTE_FAILSAFE };
volatile int remotePulseStart[4];
volatile bool remotePulseStateLast[4];
volatile void (* channelHandlers [4])(int) = { handleRemoteSteering, handleRemoteThrottle, handleRemoteControlMode, handleRemoteStatus };

void configureRemoteControl()
{
  PCintPort::attachInterrupt(PIN_INPUT_REMOTE_STEERING, remoteSteeringPulseChanged, CHANGE);
  PCintPort::attachInterrupt(PIN_INPUT_REMOTE_THROTTLE, remoteThrottlePulseChanged, CHANGE);
  PCintPort::attachInterrupt(PIN_INPUT_REMOTE_CONTROL_MODE, remoteControlModePulseChanged, CHANGE);
  PCintPort::attachInterrupt(PIN_INPUT_REMOTE_FAILSAFE, remoteFailsafePulseChanged, CHANGE);
}

void remoteSteeringPulseChanged()
{
  remotePulseChanged(0);
}

void remoteThrottlePulseChanged()
{
  remotePulseChanged(1);
}

void remoteControlModePulseChanged()
{
  remotePulseChanged(2);
}

void remoteFailsafePulseChanged()
{
  remotePulseChanged(3);
}

void remotePulseChanged(int channelIndex)
{
  int timer = micros();
  bool pulseState = digitalRead(remotePin[channelIndex]);  

  // determine if pin condition is different from the last time we saw it
  bool pulseStateChanged = pulseState != remotePulseStateLast[channelIndex];

  // keep a record of the current pin state
  remotePulseStateLast[channelIndex] = pulseState;

  // if we've missed a pulse somehow, try again next time
  if (!pulseStateChanged)
    return;

  if (pulseState == HIGH)
  {
    // pin has gone high, start timing
    remotePulseStart[channelIndex] = timer;
    return;
  }

  // pin has gone low, calculate pulse time
  int duration = micros() - remotePulseStart[channelIndex];

  // call relevant remote channel handler
  if (channelHandlers[channelIndex])
    channelHandlers[channelIndex](duration);
}

#endif

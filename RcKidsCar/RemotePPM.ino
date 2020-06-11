#ifdef USE_REMOTE_PPM

const int PIN_INPUT_REMOTE = 4;
const int PPM_PULSE_MAX_LENGTH = 3000;

// currently configured to read 4 channels of input
volatile unsigned long channelValues[4], timer[5];
volatile int pulse;

// define the ppm channel handlers, position in the array = offset channel number
// e.g. position 0 = channel 1, position 1 = channel 2, etc.
volatile void (* channelHandlers [4])(int) = { handleRemoteSteering, handleRemoteThrottle, handleRemoteControlMode, handleRemoteStatus };

void configureRemoteControl()
{
  PCintPort::attachInterrupt(PIN_INPUT_REMOTE, remotePulseReceived, RISING);
}

void remotePulseReceived()
{
  // read the time in microseconds for the current pulse
  timer[pulse] = micros();

  // if pulse is 0 there's no previous pulse to compare the value with
  if (pulse == 0)
  {
    pulse++;
    return;
  }

  // store the channel value as the difference between the current pulse and the last
  int channelIndex = pulse - 1;
  channelValues[channelIndex] = timer[pulse] - timer[pulse - 1];

  // if there is a synchronisation issue (we're reading a long interval when we shouldn't), reset to start reading pulse 1 on the next pulse
  if (channelValues[channelIndex] > PPM_PULSE_MAX_LENGTH)
  {
    timer[0] = timer[pulse];
    pulse = 1;
    return;
  }
    
  // now call the handler for the specific channel if we have one
  if (channelHandlers[channelIndex])
    channelHandlers[channelIndex](channelValues[channelIndex]);

  pulse++;
}

#endif

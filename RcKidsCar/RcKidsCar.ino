#include <Servo.h>
#include "libraries/Rotary/Rotary.cpp"

// pin configuration
const int PIN_INPUT_REMOTE_THROTTLE = 9;
const int PIN_OUTPUT_THROTTLE = 3;
// const int PIN_OUTPUT_STEERING = 0;

// input signal configuration
// remote reciever sends PWM signals in the range ~1000 to ~2000
const int PWM_RECEIVE_TIMEOUT = 100000;
const int REMOTE_LOW_VALUE_MINIMUM = 950;
const int REMOTE_LOW_VALUE_MAXIMUM = 1050;
const int REMOTE_MIDDLE_VALUE_MINIMUM = 1450;
const int REMOTE_MIDDLE_VALUE_MAXIMUM = 1550;
const int REMOTE_HIGH_VALUE_MINIMUM = 1950;
const int REMOTE_HIGH_VALUE_MAXIMUM = 2050;
const int STEERING_ANGLE_CENTRE = 90;
const int STEERING_ANGLE_MINIMUM = 45;
const int STEERING_ANGLE_MAXIMUM = 135;
const int INTERNAL_THROTTLE_MAXIMUM_OUTPUT = 50;

enum class RemoteStatus
{
  Disabled,
  Enabled,
  Failsafe
};

enum class ControlMode
{
  Internal,
  Remote
};

enum class ControlDevice
{
  Internal,
  Remote,
  None
};

enum class GearSelection
{
  Forward,
  Reverse,
  Neutral
};

struct ControlState
{
  RemoteStatus RemoteStatus;
  ControlMode RemoteControlMode;
  ControlMode InternalControlMode;
  ControlDevice ControlDevice;
  GearSelection GearSelection;
  int Proximity;
  int SteeringAngle;
  int Throttle;

  void setThrottle(int throttle)
  {
    // ensure throttle never exceeds max/min values
    int throttleMax = 100;
    if (ControlDevice == ControlDevice::Internal)
    {
      throttleMax = INTERNAL_THROTTLE_MAXIMUM_OUTPUT;
    }
    
    if (throttle < 0)
    {
      Throttle = 0;
    }
    else if (throttle > throttleMax)
    {
      Throttle = throttleMax;
    }
    else
    {
      Throttle = throttle;
    }
  }

  void incrementSteeringAngle()
  {
    setSteeringAngle(SteeringAngle + 1);
  }

  void decrementSteeringAngle()
  {
    setSteeringAngle(SteeringAngle - 1);
  }

  void setSteeringAngle(int angle)
  {
    if (ControlDevice == ControlDevice::None)
    {
      SteeringAngle = STEERING_ANGLE_CENTRE;
    }
    
    if (angle >= STEERING_ANGLE_MINIMUM && angle <= STEERING_ANGLE_MAXIMUM)
    {
      SteeringAngle = angle;
    }
  }
} currentState;

Servo powerServo;

void setup() {
  // configure input
  configureControlMode();
  configureInternalGearSelection();
  configureInternalProximity();
  configureInternalThrottle();
  configureRemote();
  configureSteering();

  // throttle pin provides both remote throttle and remote gear selection
  pinMode(PIN_INPUT_REMOTE_THROTTLE, INPUT);
  
  // configure output pins
  powerServo.attach(PIN_OUTPUT_THROTTLE, 1000, 2000);
  powerServo.write(90);

  Serial.begin(9600);
  Serial.println("Arduino Car Started. Brmmm Brmmm.");
}

void loop()
{
  refreshCurrentState();

  if (currentState.GearSelection == GearSelection::Forward)
  {
    powerServo.write(map(currentState.Throttle, 0, 100, 90, 180));
  }
  else if (currentState.GearSelection == GearSelection::Reverse)
  {
    powerServo.write(map(currentState.Throttle, 0, 100, 90, 0));
  }
  else
  {
    powerServo.write(90);
  }

//  // update output
//  Serial.println("");
//
//  Serial.print("Remote status: ");
//  Serial.println(static_cast<int>(currentState.RemoteStatus));
//
//  Serial.print("Remote control mode: ");
//  Serial.println(static_cast<int>(currentState.RemoteControlMode));
//
//  Serial.print("Internal control mode: ");
//  Serial.println(static_cast<int>(currentState.InternalControlMode));
//
//  Serial.print("Control device: ");
//  Serial.println(static_cast<int>(currentState.ControlDevice));
//
//  Serial.print("Proximity: ");
//  Serial.println(currentState.Proximity);
//
//  Serial.print("Gear selection: ");
//  Serial.println(static_cast<int>(currentState.GearSelection));
//
//  Serial.print("Power: ");
//  Serial.println(currentState.Power);
//
//  Serial.print("Steering angle: ");
//  Serial.println(currentState.SteeringAngle);
//
//  delay(2000);
}

void refreshCurrentState()
{
    // RC status and internal drive mode switch required to determine drive device
    currentState.RemoteStatus = readRemoteStatus();
    currentState.RemoteControlMode = readRemoteControlMode();
    currentState.InternalControlMode = readInternalControlMode();
  
    // can now determine drive device
    ControlDevice controlDevice = determineControlDevice();

    // configure interrupts if switching in/out of internal control mode
    if (controlDevice != currentState.ControlDevice && (currentState.ControlDevice == ControlDevice::Internal || controlDevice == ControlDevice::Internal))
    {
      toggleInternalSteering(controlDevice == ControlDevice::Internal);
    }

    currentState.ControlDevice = controlDevice;
  
    // once drive device is known drive inputs can be evaluated
    currentState.Proximity = readProximity();
    currentState.GearSelection = readGearSelection();

    int throttle = readThrottlePosition();
    currentState.setThrottle(throttle);

    if (currentState.ControlDevice == ControlDevice::Internal)
    {
      currentState.setSteeringAngle(readSteeringAngle());
    }
}

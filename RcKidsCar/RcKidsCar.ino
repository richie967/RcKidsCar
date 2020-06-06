#include <Servo.h>
#include "libraries/Rotary/Rotary.cpp"

// pin configuration
const int PIN_INPUT_INTERNAL_DRIVE_MODE = 8;
const int PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD = 12;
const int PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE = 13;
const int PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER = 7;
const int PIN_INPUT_INTERNAL_PROXIMITY_ECHO = 11;
const int PIN_INPUT_INTERNAL_THROTTLE = A0;
const int PIN_INPUT_INTERNAL_STEERING_DATA = 4;
const int PIN_INPUT_INTERNAL_STEERING_CLOCK = 2;
const int PIN_INPUT_REMOTE_FAILSAFE = 10;
const int PIN_INPUT_REMOTE_OVERRIDE = 5;
const int PIN_INPUT_REMOTE_THROTTLE = 9;
const int PIN_INPUT_REMOTE_STEERING = 6;
const int PIN_OUTPUT_THROTTLE = 3;
// const int PIN_OUTPUT_STEERING = 0;

// remote reciever sends PWM signals in the range ~1000 to ~2000
const unsigned long PWM_RECEIVE_TIMEOUT = 100000;
const int REMOTE_LOW_VALUE_MINIMUM = 950;
const int REMOTE_LOW_VALUE_MAXIMUM = 1050;
const int REMOTE_MIDDLE_VALUE_MINIMUM = 1450;
const int REMOTE_MIDDLE_VALUE_MAXIMUM = 1550;
const int REMOTE_HIGH_VALUE_MINIMUM = 1950;
const int REMOTE_HIGH_VALUE_MAXIMUM = 2050;
const int INTERNAL_THROTTLE_VALUE_MINIMUM = 250;
const int INTERNAL_THROTTLE_VALUE_MAXIMUM = 750;
const int INTERNAL_THROTTLE_MAXIMUM_OUTPUT = 50;
const int STEERING_ANGLE_CENTRE = 90;
const int STEERING_ANGLE_MINIMUM = 45;
const int STEERING_ANGLE_MAXIMUM = 135;
const int REMOTE_STEERING_CENTRE_MINIMUM = 86;
const int REMOTE_STEERING_CENTRE_MAXIMUM = 94;

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
  int Power;

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
  // configure input pins
  pinMode(PIN_INPUT_INTERNAL_DRIVE_MODE, INPUT);
  pinMode(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD, INPUT_PULLUP);
  pinMode(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE, INPUT_PULLUP);
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, OUTPUT);
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, INPUT);
  pinMode(PIN_INPUT_INTERNAL_THROTTLE, INPUT);
//  pinMode(PIN_INPUT_INTERNAL_STEERING_DATA, INPUT_PULLUP);
//  pinMode(PIN_INPUT_INTERNAL_STEERING_CLOCK, INPUT_PULLUP);
  pinMode(PIN_INPUT_REMOTE_FAILSAFE, INPUT);
  pinMode(PIN_INPUT_REMOTE_OVERRIDE, INPUT);
  pinMode(PIN_INPUT_REMOTE_THROTTLE, INPUT);
  pinMode(PIN_INPUT_REMOTE_STEERING, INPUT);

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
    powerServo.write(map(currentState.Power, 0, 100, 90, 180));
  }
  else if (currentState.GearSelection == GearSelection::Reverse)
  {
    powerServo.write(map(currentState.Power, 0, 100, 90, 0));
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
  
    // once drive device is known drive inputs can be calculated
    currentState.Proximity = readProximity();
    currentState.GearSelection = readGearSelection();
    currentState.Power = readThrottlePosition();
  
    // when drive device changes we need to enable/disable internal steering interrupts
    toggleInternalSteering(currentState.ControlDevice == ControlDevice::Internal);
    readSteeringAngle();
}

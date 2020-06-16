#include "libraries/PinChangeInt/PinChangeInt.h"

// uncomment the line below to switch RC control from separate channels to PPM
//#define USE_REMOTE_PPM

// input signal configuration
// remote reciever sends PWM signals in the range ~1000 to ~2000
const int REMOTE_LOW_VALUE_MINIMUM = 950;
const int REMOTE_LOW_VALUE_MAXIMUM = 1050;
const int REMOTE_MIDDLE_VALUE_MINIMUM = 1400;
const int REMOTE_MIDDLE_VALUE_MAXIMUM = 1600;
const int REMOTE_HIGH_VALUE_MINIMUM = 1950;
const int REMOTE_HIGH_VALUE_MAXIMUM = 2050;
const int STEERING_ANGLE_CENTRE = 90;
const int STEERING_ANGLE_MINIMUM = 40;
const int STEERING_ANGLE_MAXIMUM = 140;
const int STEERING_ANGLE_INCREMENT = 10;
const int INTERNAL_THROTTLE_MAXIMUM_OUTPUT = 50;

namespace Enums
{
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
}

struct ControlState
{
  // event handlers
  (* controlDeviceChanged)(Enums::ControlDevice, Enums::ControlDevice);
  (* steeringOutputChanged)(int);
  (* throttleOutputChanged)(int, Enums::GearSelection);
  
  // default to in car control
  Enums::RemoteStatus RemoteStatus = Enums::RemoteStatus::Disabled;
  Enums::ControlMode RemoteControlMode = Enums::ControlMode::Internal;  
  Enums::ControlMode InternalControlMode = Enums::ControlMode::Internal;
  Enums::ControlDevice ControlDevice = Enums::ControlDevice::Internal;
  Enums::GearSelection GearSelection = Enums::GearSelection::Neutral;
  int Proximity;
  int SteeringAngle = STEERING_ANGLE_CENTRE;
  int Throttle;

  void setRemoteStatus(Enums::RemoteStatus status)
  {
    RemoteStatus = status;
    determineControlDevice();
  }

  void setRemoteControlMode(Enums::ControlMode mode)
  {
    RemoteControlMode = mode;
    determineControlDevice();
  }

  void setInternalControlMode(Enums::ControlMode mode)
  {
    InternalControlMode = mode;
    determineControlDevice();
  }

  void determineControlDevice()
  {
    Enums::ControlDevice deviceLast = ControlDevice;
    
    switch (RemoteStatus)
    {
      case Enums::RemoteStatus::Failsafe:
        ControlDevice = Enums::ControlDevice::None;
        break;
      case Enums::RemoteStatus::Enabled:
        ControlDevice = (RemoteControlMode == Enums::ControlMode::Remote) ? Enums::ControlDevice::Remote : Enums::ControlDevice::Internal;
        break;
      case Enums::RemoteStatus::Disabled:
        ControlDevice = (InternalControlMode == Enums::ControlMode::Internal) ? Enums::ControlDevice::Internal : Enums::ControlDevice::None;
        break;
    }

    if (ControlDevice != deviceLast && controlDeviceChanged)
      controlDeviceChanged(deviceLast, ControlDevice);
  }

  void setGearSelection(Enums::GearSelection gearSelection)
  {
    Enums::GearSelection gearSelectionLast = GearSelection;

    GearSelection = gearSelection;

    // changing gear selection is equivalent to a modification to the throttle, needs to trigger the same behaviour
    if (GearSelection != gearSelection && throttleOutputChanged)
      throttleOutputChanged(Throttle, GearSelection);
  }

  void setThrottle(int throttle)
  {
    int throttleLast = Throttle;
    
    // ensure throttle never exceeds max/min values
    int throttleMax = 100;
    
    if (ControlDevice == Enums::ControlDevice::Internal)
      throttleMax = INTERNAL_THROTTLE_MAXIMUM_OUTPUT;
    
    if (throttle < 0)
      Throttle = 0;
    else if (throttle > throttleMax)
      Throttle = throttleMax;
    else
      Throttle = throttle;

    if (Throttle != throttleLast && throttleOutputChanged)
      throttleOutputChanged(Throttle, GearSelection);
  }

  void incrementSteeringAngle()
  {
    setSteeringAngle(SteeringAngle + STEERING_ANGLE_INCREMENT);
  }

  void decrementSteeringAngle()
  {
    setSteeringAngle(SteeringAngle - STEERING_ANGLE_INCREMENT);
  }

  void setSteeringAngle(int angle)
  {
    int steeringlast = SteeringAngle;
    
    if (ControlDevice == Enums::ControlDevice::None)
      return;

    if (angle < STEERING_ANGLE_MINIMUM)
      SteeringAngle = STEERING_ANGLE_MINIMUM;
    else if (angle > STEERING_ANGLE_MAXIMUM)
      SteeringAngle = STEERING_ANGLE_MAXIMUM;
    else
      SteeringAngle = angle;
      
    if (SteeringAngle != steeringlast && steeringOutputChanged)
      steeringOutputChanged(SteeringAngle);
  }
} currentState;

volatile bool updateThrottle;
volatile bool updateSteering;

void setup() {
  // disable interrupts during setup
  noInterrupts();

  // hook up handler for when the control device changes
  currentState.controlDeviceChanged = controlDeviceChanged;
  currentState.throttleOutputChanged = throttleOutputChanged;
  currentState.steeringOutputChanged = steeringOutputChanged;
  
  // configure inputs, default state is internal controls enabled
  configureControlMode();
  configureGearSelection();
  configureSteering();
  configureProximity();

  // configure remote inputs
  configureRemoteControl();

  // configure output devices
  configureOutput();

  // reenable interrupts
  interrupts();
}

void loop()
{
  // poll non-interrupt devices
  pollThrottle();

  // update output if changes
  if (updateThrottle)
  {
    refreshThrottleOutput(currentState.Throttle, currentState.GearSelection);
    updateThrottle = false;
  }

  if (updateSteering)
  {
    refreshSteeringOutput(currentState.SteeringAngle);
    updateSteering = false;
  }
}

void controlDeviceChanged(Enums::ControlDevice oldDevice, Enums::ControlDevice newDevice)
{
  // changing to or from internal control device we must enable/disabled internal input interrupts
  if (oldDevice == Enums::ControlDevice::Internal || newDevice == Enums::ControlDevice::Internal)
  {
    bool enabled = (newDevice == Enums::ControlDevice::Internal);
    configureInternalInterrupts(enabled);
  }
}

void configureInternalInterrupts(bool enabled)
{
  toggleGearSelection(enabled);
  toggleSteering(enabled);
}

void throttleOutputChanged(int throttle, Enums::GearSelection gearSelection)
{  
  updateThrottle = true;
}

void steeringOutputChanged(int angle)
{
  updateSteering = true;
}

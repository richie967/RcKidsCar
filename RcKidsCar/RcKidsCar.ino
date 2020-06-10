#include "libraries/PinChangeInt/PinChangeInt.h"

// input signal configuration
// remote reciever sends PWM signals in the range ~1000 to ~2000
const int REMOTE_LOW_VALUE_MINIMUM = 950;
const int REMOTE_LOW_VALUE_MAXIMUM = 1050;
const int REMOTE_MIDDLE_VALUE_MINIMUM = 1450;
const int REMOTE_MIDDLE_VALUE_MAXIMUM = 1550;
const int REMOTE_HIGH_VALUE_MINIMUM = 1950;
const int REMOTE_HIGH_VALUE_MAXIMUM = 2050;
const int STEERING_ANGLE_CENTRE = 90;
const int STEERING_ANGLE_MINIMUM = 50;
const int STEERING_ANGLE_MAXIMUM = 130;
const int STEERING_ANGLE_INCREMENT = 3;
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
  (* controlDeviceChanged)(Enums::ControlDevice, Enums::ControlDevice);
  
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
    Enums::ControlDevice oldDevice = ControlDevice;
    
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

    if (ControlDevice != oldDevice && controlDeviceChanged)
    {
      controlDeviceChanged(oldDevice, ControlDevice);
    }
  }

  void setThrottle(int throttle)
  {
    // ensure throttle never exceeds max/min values
    int throttleMax = 100;
    if (ControlDevice == Enums::ControlDevice::Internal)
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
    setSteeringAngle(SteeringAngle + STEERING_ANGLE_INCREMENT);
  }

  void decrementSteeringAngle()
  {
    setSteeringAngle(SteeringAngle - STEERING_ANGLE_INCREMENT);
  }

  void setSteeringAngle(int angle)
  {
    if (ControlDevice == Enums::ControlDevice::None)
    {
      SteeringAngle = STEERING_ANGLE_CENTRE;
    }
    
    if (angle >= STEERING_ANGLE_MINIMUM && angle <= STEERING_ANGLE_MAXIMUM)
    {
      SteeringAngle = angle;
    }
  }
} currentState;

void setup() {
  // disable interrupts during setup
  noInterrupts();

  // hook up handler for when the control device changes
  currentState.controlDeviceChanged = controlDeviceChanged;
  
  // configure inputs, default state is internal controls enabled
  configureControlMode();
  configureGearSelection();
  configureSteering();
//  configureProximity();

  // configure remote inputs
//  configureRemoteControl();

  // configure output devices
  configureOutput();

  // reenable interrupts
  interrupts();
}

void loop()
{
  // poll non-interrupt devices
  pollThrottle();
  
  // update output devices
  refreshOutput();
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

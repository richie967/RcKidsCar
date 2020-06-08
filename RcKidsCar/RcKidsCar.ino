#include <Servo.h>
#include "libraries/Rotary/Rotary.cpp"
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
const int STEERING_ANGLE_MINIMUM = 70;
const int STEERING_ANGLE_MAXIMUM = 110;
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
  int SteeringAngle;
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

    if (ControlDevice != oldDevice)
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
    setSteeringAngle(SteeringAngle + 1);
  }

  void decrementSteeringAngle()
  {
    setSteeringAngle(SteeringAngle - 1);
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
  currentState.controlDeviceChanged = controlDeviceChanged;
  
  // configure input
  configureRemote();
  configureControlModeInternal();
  configureGearSelectionInternal();
  configureProximityInternal();
  configureThrottleInternal();
  configureOutput();
}

void loop()
{
  refreshCurrentState();
  refreshOutput();
}

void refreshCurrentState()
{
  // all of the following properties of the state are updated as and when they occur by interrupts
  // currentState.RemoteStatus
  // currentState.RemoteControlMode
  // currentState.InternalControlMode
  // currentState.GearSelection
  // currentState.SteeringAngle

  // currentState.ControlDevice is updated automatically when relevant properties of the state change

  // update the remaining inputs
  refreshProximity();

  if (currentState.ControlDevice == Enums::ControlDevice::Internal)
  {
    refreshThrottleInternal();
  }
}

void controlDeviceChanged(Enums::ControlDevice oldDevice, Enums::ControlDevice newDevice)
{
  // changing to or from internal control device we must enable/disabled internal input interrupts
  if (oldDevice == Enums::ControlDevice::Internal || newDevice == Enums::ControlDevice::Internal)
  {
    bool enabled = newDevice == Enums::ControlDevice::Internal;
    toggleGearSelectionInternal(enabled);
    toggleSteeringInternal(enabled);
  }
}

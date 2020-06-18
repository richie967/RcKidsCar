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
const int REMOTE_THROTTLE_MAXIMUM_OUTPUT = 100;
const int PROXIMITY_STOP_DISTANCE = 75;
const int PROXIMITY_SLOW_DISTANCE = 300;
const int THROTTLE_RESTRICTION_FACTOR_MINIMUM = 25;
const int THROTTLE_RESTRICTION_FACTOR_MAXIMUM = 100;

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
  (* throttleOutputChanged)(Enums::GearSelection, int, int);
  
  // default to in car control
  Enums::RemoteStatus RemoteStatus = Enums::RemoteStatus::Disabled;
  Enums::ControlMode RemoteControlMode = Enums::ControlMode::Internal;  
  Enums::ControlMode InternalControlMode = Enums::ControlMode::Internal;
  Enums::ControlDevice ControlDevice = Enums::ControlDevice::Internal;
  Enums::GearSelection GearSelection = Enums::GearSelection::Neutral;
  int Proximity;
  int SteeringAngle = STEERING_ANGLE_CENTRE;
  int Throttle;
  int ThrottleRestrictionFactor = THROTTLE_RESTRICTION_FACTOR_MAXIMUM;

  void setRemoteStatus(Enums::RemoteStatus remoteStatus)
  {
    RemoteStatus = remoteStatus;
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
    // detect changes in gear selection
    Enums::GearSelection gearSelectionLast = GearSelection;

    GearSelection = gearSelection;

    // changing gear selection is equivalent to a modification to the throttle, needs to trigger the same behaviour
    if (GearSelection != gearSelection)
    {
      // when de-selecting forward remove throttle restriction
      if (GearSelection != Enums::GearSelection::Forward)
        setThrottleRestrictionFactor(THROTTLE_RESTRICTION_FACTOR_MAXIMUM);
      
      refreshThrottleOutput();
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
    // detect changes in steering angle
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

  void setThrottle(int throttle)
  {
    // detect changes in throttle
    int throttleLast = Throttle;
    
    // ensure throttle never exceeds max/min values
    int throttleMax = getMaxThrottle();
    
    if (throttle < 0)
      Throttle = 0;
    else if (throttle > throttleMax)
      Throttle = throttleMax;
    else
      Throttle = throttle;

    // update output when throttle value has changed
    if (Throttle != throttleLast)
      refreshThrottleOutput();
  }

  int getMaxThrottle()
  {
    if (ControlDevice == Enums::ControlDevice::None)
      return 0;

    return (ControlDevice == Enums::ControlDevice::Internal) ? INTERNAL_THROTTLE_MAXIMUM_OUTPUT : REMOTE_THROTTLE_MAXIMUM_OUTPUT;
  }

  void setProximity(int distance)
  {
    int proximityLast = Proximity;
    
    Proximity = distance;

    // if no change in proximity do nothing
    if (Proximity == proximityLast)
      return;

    // if car is not in forward gear do nothing
    if (GearSelection != Enums::GearSelection::Forward)
      return;

    // apply a throttle restriction factor within proximity range
    int factor = THROTTLE_RESTRICTION_FACTOR_MAXIMUM;

    if (Proximity <= PROXIMITY_STOP_DISTANCE)
      factor = 0;
    else
      factor = map(Proximity, PROXIMITY_STOP_DISTANCE, PROXIMITY_SLOW_DISTANCE, THROTTLE_RESTRICTION_FACTOR_MINIMUM, THROTTLE_RESTRICTION_FACTOR_MAXIMUM);
    
    setThrottleRestrictionFactor(factor);
  }

  void setThrottleRestrictionFactor(int factor)
  {
    // detect changes in restriction factor
    int factorLast = ThrottleRestrictionFactor;

    if (factor < 0)
      ThrottleRestrictionFactor = 0;
    else if (factor > THROTTLE_RESTRICTION_FACTOR_MAXIMUM)
      ThrottleRestrictionFactor = THROTTLE_RESTRICTION_FACTOR_MAXIMUM;
    else
      ThrottleRestrictionFactor = factor;

    if (ThrottleRestrictionFactor != factorLast)
      refreshThrottleOutput();
  }

  void refreshThrottleOutput()
  {
    if (throttleOutputChanged)
      throttleOutputChanged(GearSelection, Throttle, ThrottleRestrictionFactor);
  }
} currentState;

volatile bool refreshSteering;
volatile bool refreshThrottle;

void setup() {
  // disable interrupts during setup
  noInterrupts();

  // hook up handler for when the control device changes
  currentState.controlDeviceChanged = controlDeviceChanged;
  currentState.steeringOutputChanged = steeringOutputChanged;
  currentState.throttleOutputChanged = throttleOutputChanged;
  
  // configure internal inputs, default state is internal controls enabled
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

  // refresh output
  if (refreshSteering)
  {
    refreshSteeringOutput(currentState.SteeringAngle);
    refreshSteering = false;
  }

  if (refreshThrottle)
  {
    refreshThrottleOutput(currentState.GearSelection, currentState.Throttle, currentState.ThrottleRestrictionFactor);
    refreshThrottle = false;
  }
}

void controlDeviceChanged(Enums::ControlDevice deviceLast, Enums::ControlDevice device)
{
  // changing to or from internal control device we must enable/disabled internal input interrupts
  if (deviceLast == Enums::ControlDevice::Internal || device == Enums::ControlDevice::Internal)
  {
    bool enabled = (device == Enums::ControlDevice::Internal);
    toggleGearSelection(enabled);
    toggleSteering(enabled);
  }
}

void steeringOutputChanged(int angle)
{
  refreshSteering = true;
}

void throttleOutputChanged(Enums::GearSelection gearSelection, int throttle, int throttleRestrictionFactor)
{
  refreshThrottle = true;
}

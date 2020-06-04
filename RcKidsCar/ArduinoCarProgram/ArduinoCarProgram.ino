#include <Servo.h>

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
const int MANUAL_THROTTLE_VALUE_MINIMUM = 250;
const int MANUAL_THROTTLE_VALUE_MAXIMUM = 750;
const int MANUAL_THROTTLE_MAXIMUM_OUTPUT = 50;
const int STEERING_ANGLE_CENTRE = 90;
const int STEERING_ANGLE_MINIMUM = 45;
const int STEERING_ANGLE_MAXIMUM = 135;
const int REMOTE_STEERING_CENTRE_MINIMUM = 86;
const int REMOTE_STEERING_CENTRE_MAXIMUM = 94;

enum DriveMode
{
  Manual,
  RemoteControl
};

enum DriveDevice
{
  Internal,
  Remote,
  None
};

enum Gear
{
  Forward,
  Reverse,
  Neutral
};

struct ControlState
{
  bool RemoteInUse;
  bool Failsafe;
  DriveMode InternalDriveMode;
  DriveMode RemoteDriveMode;
  DriveDevice DriveDevice;
  int Proximity;
  Gear Gear;
  int SteeringAngle;
  int Power;
} currentState;

int internalSteeringDataLast;
Servo powerServo;

void setup() {
  // configure input pins
  pinMode(PIN_INPUT_INTERNAL_DRIVE_MODE, INPUT);
  pinMode(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD, INPUT_PULLUP);
  pinMode(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE, INPUT_PULLUP);
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, OUTPUT);
  pinMode(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, INPUT);
  pinMode(PIN_INPUT_INTERNAL_THROTTLE, INPUT);
  pinMode(PIN_INPUT_INTERNAL_STEERING_DATA, INPUT_PULLUP);
  pinMode(PIN_INPUT_INTERNAL_STEERING_CLOCK, INPUT_PULLUP);
  pinMode(PIN_INPUT_REMOTE_FAILSAFE, INPUT);
  pinMode(PIN_INPUT_REMOTE_OVERRIDE, INPUT);
  pinMode(PIN_INPUT_REMOTE_THROTTLE, INPUT);
  pinMode(PIN_INPUT_REMOTE_STEERING, INPUT);

  // configure output pins
  powerServo.attach(PIN_OUTPUT_THROTTLE, 1000, 2000);
  powerServo.write(90);

  internalSteeringDataLast = digitalRead(PIN_INPUT_INTERNAL_STEERING_DATA);

  Serial.begin(9600);
  Serial.println("Arduino Car Started. Brmmm Brmmm.");
}

void loop()
{
  // read all inputs
  refreshCurrentState();

  if (currentState.Gear == Forward)
  {
    powerServo.write(map(currentState.Power, 0, 100, 90, 180));
  }
  else if (currentState.Gear == Reverse)
  {
    powerServo.write(map(currentState.Power, 0, 100, 90, 0));
  }
  else
  {
    powerServo.write(90);
  }

  // update output
//  Serial.println("");
//
//  Serial.print("Remote in use: ");
//  Serial.println(currentState.RemoteInUse);
//
//  Serial.print("Failsafe: ");
//  Serial.println(currentState.Failsafe);
//
//  Serial.print("Remote drive mode: ");
//  Serial.println(currentState.RemoteDriveMode);
//
//  Serial.print("Internal drive mode: ");
//  Serial.println(currentState.InternalDriveMode);
//
//  Serial.print("Drive device: ");
//  Serial.println(currentState.DriveDevice);
//
//  Serial.print("Proximity: ");
//  Serial.println(currentState.Proximity);
//
//  Serial.print("Gear selection: ");
//  Serial.println(currentState.Gear);
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
  currentState.RemoteInUse = readRemoteInUse();
  currentState.Failsafe = readFailsafe();
  currentState.RemoteDriveMode = readRemoteDriveMode();
  currentState.InternalDriveMode = readInternalDriveMode();

  // can now determine drive device
  currentState.DriveDevice = determineDriveDevice();

  // once drive device is known drive inputs can be calculated
  currentState.Proximity = readProximity();
  currentState.Gear = readGearPosition();
  currentState.Power = readThrottlePosition();
  currentState.SteeringAngle = readSteeringAngle(currentState.SteeringAngle);
}

bool readRemoteInUse()
{
  // when remote receiver started without remote connected remote receiver outputs 0, otherwise LOW, MIDDLE, HIGH values as described in constants
  // failsafe value is LOW
  int pinValue = pulseIn(PIN_INPUT_REMOTE_FAILSAFE, HIGH, PWM_RECEIVE_TIMEOUT);
  return pinValue > 0;
}

bool readFailsafe()
{
  // see readRemoteInUse for failsafe value description
  int pinValue = pulseIn(PIN_INPUT_REMOTE_FAILSAFE, HIGH, PWM_RECEIVE_TIMEOUT);
  return pinValue >= REMOTE_LOW_VALUE_MINIMUM && pinValue <= REMOTE_LOW_VALUE_MAXIMUM;
}

DriveMode readRemoteDriveMode()
{
  // PWM signal from remote sends one of three values 0 (remote not connected), ~1000 (LOW, switch off), ~2000 (HIGH, switch on)
  int pinValue = pulseIn(PIN_INPUT_REMOTE_OVERRIDE, HIGH, PWM_RECEIVE_TIMEOUT);
  
  if (pinValue >= REMOTE_HIGH_VALUE_MINIMUM && pinValue <= REMOTE_HIGH_VALUE_MAXIMUM)
  {
    return RemoteControl;
  }

  return Manual;
}

DriveMode readInternalDriveMode()
{
  int pinValue = digitalRead(PIN_INPUT_INTERNAL_DRIVE_MODE);
  return static_cast<DriveMode>(pinValue);
}

DriveDevice determineDriveDevice()
{
  if (currentState.RemoteInUse)
  {
    if (currentState.Failsafe)
    {
      return None;
    }

    if (currentState.RemoteDriveMode == RemoteControl)
    {
      return Remote;
    }

    return Internal;
  }

  if (currentState.InternalDriveMode == Manual)
  {
    return Internal;
  }

  return None;
}

int readProximity()
{
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_INPUT_INTERNAL_PROXIMITY_TRIGGER, LOW);

  int duration = pulseIn(PIN_INPUT_INTERNAL_PROXIMITY_ECHO, HIGH);

  return (duration / 2) * 0.0343;
}

Gear readGearPosition()
{
  if (currentState.DriveDevice == None)
  {
    return Neutral;
  }
  
  if (currentState.DriveDevice == Manual)
  {
    return readGearPositionManual();
  }
  
  return readGearPositionRemote();
}

Gear readGearPositionManual()
{
  int forwardPinValue = !digitalRead(PIN_INPUT_INTERNAL_GEAR_SELECTION_FORWARD);
  int reversePinValue = !digitalRead(PIN_INPUT_INTERNAL_GEAR_SELECTION_REVERSE);

  if (forwardPinValue)
  {
    return Forward;
  }
  else if (reversePinValue)
  {
    return Reverse;
  }

  return Neutral;
}

Gear readGearPositionRemote()
{
  int throttleValue = pulseIn(PIN_INPUT_REMOTE_THROTTLE, HIGH);

  if (throttleValue > REMOTE_MIDDLE_VALUE_MAXIMUM)
  {
    return Forward;
  }
  else if (throttleValue < REMOTE_MIDDLE_VALUE_MINIMUM)
  {
    return Reverse;
  }

  return Neutral;
}

int readThrottlePosition()
{
  int throttlePosition = 0;
  int throttleMax = 100;

  if (currentState.DriveDevice == Manual)
  {
    throttleMax = MANUAL_THROTTLE_MAXIMUM_OUTPUT;
    throttlePosition = readThrottlePositionManualRaw();
  }
  else
  {
    throttlePosition = readThrottlePositionRemoteRaw();
  }

  if (throttlePosition < 0)
  {
    return 0;
  }
  else if (throttlePosition > throttleMax)
  {
    return throttleMax;
  }

  return throttlePosition;
}

int readThrottlePositionManualRaw()
{
  int throttleValue = analogRead(PIN_INPUT_INTERNAL_THROTTLE);
  return map(throttleValue, MANUAL_THROTTLE_VALUE_MINIMUM, MANUAL_THROTTLE_VALUE_MAXIMUM, 0, MANUAL_THROTTLE_MAXIMUM_OUTPUT);
}

int readThrottlePositionRemoteRaw()
{
  int throttleValue = pulseIn(PIN_INPUT_REMOTE_THROTTLE, HIGH);

  if (throttleValue <= REMOTE_MIDDLE_VALUE_MINIMUM)
  {
    // negative throttle (reverse)
    return map(throttleValue, REMOTE_LOW_VALUE_MAXIMUM, REMOTE_MIDDLE_VALUE_MINIMUM, -100, 0) * -1;
  }
  else if (throttleValue >= 1500)
  {
    // positive throttle (forward)
    return map(throttleValue, REMOTE_MIDDLE_VALUE_MAXIMUM, REMOTE_HIGH_VALUE_MINIMUM, 0, 100);
  }

  return 0;
}

int readSteeringAngle(int steeringLast)
{
  int steeringAngle = STEERING_ANGLE_CENTRE;

  if (currentState.DriveDevice == None)
  {
    return STEERING_ANGLE_CENTRE;
  }
  else if (currentState.DriveDevice == Manual)
  {
    steeringAngle = readSteeringAngleManualRaw(steeringLast);
  }
  else
  {
    steeringAngle = readSteeringAngleRemoteRaw();
  }
  
  if (steeringAngle < STEERING_ANGLE_MINIMUM)
  {
    return STEERING_ANGLE_MINIMUM;
  }
  else if (steeringAngle > STEERING_ANGLE_MAXIMUM)
  {
    return STEERING_ANGLE_MAXIMUM;
  }

  return steeringAngle;
}

int readSteeringAngleManualRaw(int steeringLast)
{
  int data = digitalRead(PIN_INPUT_INTERNAL_STEERING_DATA);

  if (data != internalSteeringDataLast)
  {
    internalSteeringDataLast = data;
    int clock = digitalRead(PIN_INPUT_INTERNAL_STEERING_CLOCK);

    if (clock != data)
    {
      return steeringLast + 4;
    }
    else
    {
      return steeringLast - 4;
    }
  }

  return steeringLast;
}

int readSteeringAngleRemoteRaw()
{
  int steeringValue = pulseIn(PIN_INPUT_REMOTE_STEERING, HIGH);
  int steeringAngle = map(steeringValue, REMOTE_LOW_VALUE_MAXIMUM, REMOTE_HIGH_VALUE_MINIMUM, STEERING_ANGLE_MINIMUM, STEERING_ANGLE_MAXIMUM);

  if (steeringAngle >= REMOTE_STEERING_CENTRE_MINIMUM && steeringAngle <= REMOTE_STEERING_CENTRE_MAXIMUM)
  {
    return STEERING_ANGLE_CENTRE;
  }

  return steeringAngle;
}

#include <Servo.h>

const int DRIVE_MODE_PIN = 3;
const int STEERING_SERVO_PIN = 4; // Moved to a Non OWM Pin
const int STEERING_INPUT_MANUAL_PIN = A0;
const int STEERING_INPUT_REMOTE_PIN = 6;
const int STEERING_CENTRE_MINIMUM = 86;
const int STEERING_CENTRE_MAXIMUM = 96;
const int PARENTAL_OVERRIDE_PIN = 5;
const int THROTTLE_INPUT_REMOTE_PIN = 9;
const int THROTTLE_OUTPUT_PIN = 10;
const int DRIVE_DIRECTION_FORWARD_PIN = 13;
const int DRIVE_DIRECTION_REVERSE_PIN = 12;

Servo _steeringServo;

void setup() {
  // put your setup code here, to run once:

  // Inputs
  pinMode(DRIVE_MODE_PIN, INPUT);
  pinMode(STEERING_INPUT_MANUAL_PIN, INPUT);
  pinMode(STEERING_INPUT_REMOTE_PIN, INPUT);
  pinMode(PARENTAL_OVERRIDE_PIN, INPUT);
  pinMode(THROTTLE_INPUT_REMOTE_PIN, INPUT);
  pinMode(DRIVE_DIRECTION_FORWARD_PIN, INPUT_PULLUP);
  pinMode(DRIVE_DIRECTION_REVERSE_PIN, INPUT_PULLUP);

  _steeringServo.attach(STEERING_SERVO_PIN, 700, 2400);
  _steeringServo.write(90);

  Serial.begin(9600);
  Serial.println("Initialise Program");
}

void loop() {
  // put your main code here, to run repeatedly:

  int parentalOverrideValue = pulseIn(PARENTAL_OVERRIDE_PIN, HIGH);
  bool isParentalOverride = parentalOverrideValue > 1300;
  bool isRemoteControl = digitalRead(DRIVE_MODE_PIN);


  int gear = readGearstick();
  Serial.println(gear);

  int steeringAngle = 90;
  int throttlePosition = 0;

  // If Remote control or parent override is on

  if (isRemoteControl || isParentalOverride)
  {
    steeringAngle = readRemoteSteeringAngle();
    throttlePosition = readRemoteThrottlePosition();
  }


  // If Manual Mode

  else
  {
    steeringAngle = readManualSteeringAngle();
  }

  if (steeringAngle >= STEERING_CENTRE_MINIMUM && steeringAngle <= STEERING_CENTRE_MAXIMUM)
  {
    steeringAngle = 90;
  }

  //Serial.println(steeringAngle);
  //Serial.println(isParentalOverride);
  //  Serial.print("Throttle Position= ");
  //  Serial.print( throttlePosition);
  //  Serial.print(" Steering Angle= ");
  //  Serial.print( steeringAngle);
  //  Serial.print(" Is Remote Control= ");
  //  Serial.print(isRemoteControl);
  //  Serial.print(" Is Parent Override= ");
  //  Serial.print(isParentalOverride);
  //  Serial.println();

  _steeringServo.write(steeringAngle);


}

int readManualSteeringAngle()
{
  int steeringPosition = 90;
  int steeringValue = analogRead(STEERING_INPUT_MANUAL_PIN);
  return map(steeringValue, 0, 1023, 0, 180);
}

int readRemoteSteeringAngle()
{
  int steeringPosition = 90;
  int steeringValue = pulseIn(STEERING_INPUT_REMOTE_PIN, HIGH);
  return map(steeringValue, 990, 1990, 0, 180);
}

int readRemoteThrottlePosition()
{
  int throttlePosition = 0;
  int throttleValue = pulseIn(THROTTLE_INPUT_REMOTE_PIN, HIGH);
  if (throttleValue <= 1450)
  {
    throttlePosition = map(throttleValue, 990, 1450, -100, 0);
    if (throttlePosition < -100)
    {
      throttlePosition = -100;
    }
    else if (throttlePosition > 0)
    {
      throttlePosition = 0;
    }
  }
  else if (throttleValue >= 1500)
  {
    throttlePosition = map(throttleValue, 1500, 1980, 0, 100);
    if (throttlePosition < 0)
    {
      throttlePosition = 0;
    }
    else if (throttlePosition > 100)
    {
      throttlePosition = 100;
    }
  }
  return throttlePosition;
}

int readGearstick()
{
  //Determine Gearsstick potion
  bool forwardValue = digitalRead(DRIVE_DIRECTION_FORWARD_PIN);
  bool reverseValue = digitalRead(DRIVE_DIRECTION_REVERSE_PIN);
  int gear = 0;

  if (!forwardValue) // Runs if gearstick is forwards
  {
    return gear = 1;
  }
  if (!reverseValue) // Runs if gearstick is reverse
  {
    return gear = 2;
  }
  if (forwardValue && reverseValue) // Runs if gearstick is neutral
  {
    return gear = 3;
  };
}

#include <Servo.h>

const int DRIVE_MODE_PIN = 3;
const int STEERING_SERVO_PIN = 11;
const int STEERING_INPUT_MANUAL_PIN = A0;
const int STEERING_INPUT_REMOTE_PIN = 6;
const int STEERING_CENTRE_MINIMUM = 87;
const int STEERING_CENTRE_MAXIMUM = 93;
const int PARENTAL_OVERRIDE_PIN = 5;
const int THROTTLE_INPUT_REMOTE_PIN = 9;

Servo _steeringServo;

void setup() {
  // put your setup code here, to run once:

  // Inputs
  pinMode(DRIVE_MODE_PIN, INPUT);
  pinMode(STEERING_INPUT_MANUAL_PIN, INPUT);
  pinMode(STEERING_INPUT_REMOTE_PIN, INPUT);
  pinMode(PARENTAL_OVERRIDE_PIN, INPUT);
  pinMode(THROTTLE_INPUT_REMOTE_PIN, INPUT);

  _steeringServo.attach(STEERING_SERVO_PIN, 700, 2400);
  _steeringServo.write(90);

  Serial.begin(9600);
  Serial.println("Initialise Program");
}

void loop() {
  // put your main code here, to run repeatedly:#
  int parentalOverrideValue = pulseIn(PARENTAL_OVERRIDE_PIN, HIGH);
  bool isParentalOverride = parentalOverrideValue > 1300;
  bool isRemoteControl = digitalRead(DRIVE_MODE_PIN);

  int steeringAngle = 90;
  int throttlePosition = 0;
  if (isRemoteControl || isParentalOverride)
  {
    steeringAngle = readRemoteSteeringAngle();
    throttlePosition = readRemoteThrottlePosition();
  }
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
  Serial.println(throttlePosition);

  _steeringServo.write(steeringAngle);


}

int readManualSteeringAngle()
{
  int steeringValue = analogRead(STEERING_INPUT_MANUAL_PIN);
  return map(steeringValue, 0, 1023, 0, 180);
}

int readRemoteSteeringAngle()
{
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

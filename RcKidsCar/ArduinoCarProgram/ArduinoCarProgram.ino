#include <Servo.h>

const int DRIVE_MODE_PIN = 3;
const int STEERING_SERVO_PIN = 4; // Moved to a Non OWM Pin
const int STEERING_INPUT_MANUAL_PIN = A0;
const int STEERING_INPUT_REMOTE_PIN = 6;
const int STEERING_CENTRE_MINIMUM = 86;
const int STEERING_CENTRE_MAXIMUM = 96;
const int PARENTAL_OVERRIDE_PIN = 5;
const int THROTTLE_INPUT_REMOTE_PIN = 9;
const int THROTTLE_INPUT_MANUAL_PIN = 2;
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
  pinMode(THROTTLE_INPUT_MANUAL_PIN, INPUT_PULLUP);


  // Outputs
  _steeringServo.attach(STEERING_SERVO_PIN, 700, 2400);
  _steeringServo.write(90);

  Serial.begin(9600);
  Serial.println("Welcome to the Arduino Car program");
}

void loop() {

  // In Car Controls
  int parentalOverrideValue = pulseIn(PARENTAL_OVERRIDE_PIN, HIGH);

  // Remote Controls
  bool isParentalOverride = parentalOverrideValue > 1300; // This reads a button on the Remote control that determines if the parent should override the cr controls
  bool isRemoteControl = digitalRead(DRIVE_MODE_PIN);

  // Variables that get set by the remote control or the in car controls
  int steeringAngle = 90;
  int throttlePosition = 0;

  // Variables that get set from the in car controls
  int gear = readGearstick();

  // If Remote control or parent override is on

  if (isRemoteControl || isParentalOverride)
  {
    steeringAngle = readRemoteSteeringAngle();
    throttlePosition = readRemoteThrottlePosition();
    Serial.println("Remote or override is on");
    Serial.println(throttlePosition);
  }


  // If Manual Mode

  else
  {
    steeringAngle = readManualSteeringAngle();
    Serial.println("Gear in main loop function");
    Serial.println(gear);
    
    Serial.println("Throttle Position at top of function");
    Serial.println(throttlePosition);
    if (gear == 1 && throttlePosition == 0) // Forward gear selected and foot pedal pressed down
    {
      throttlePosition = 25;
    };
    if (gear == 2 && throttlePosition == 0) // Reverse gear selected and foot pedal pressed down
    {
      throttlePosition = -25;
    };
    if (gear == 3 && throttlePosition == 1) // Neutral is selected, set throttle to disabled
    {
    Serial.println("Throttle Disabled");
      throttlePosition = 0;
    };

    

    Serial.println("Manual Throttle is on");
    Serial.println("Throttle Position at bottom of function");
    Serial.println(throttlePosition);
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
  delay(3000);
}

// End of Main Loop Function

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

int readManualThrottlePosition()
{
  int throttlePosition = 0;
  int manualthrottlePosition = digitalRead(THROTTLE_INPUT_MANUAL_PIN);
  Serial.println("Manual Throtte Position From sub function");
  Serial.println("manualthrottlePosition");
  return throttlePosition = manualthrottlePosition;
  
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
  Serial.println("Gear from Function");
  Serial.println(gear);

}

#include <Servo.h>
#include "AccelStepper.h"

//stepper motor variables
const int steps = 2048;

#define MotorInterface 8

AccelStepper motor(MotorInterface, 9, 11, 10, 12);

//LDR variables
#define LDR A2
int LDRValue = 0;

bool sleeping = false;
bool previousState = false;

//7 segment display led variables (corresponds to the parts of a 7 semgent):
//https://microcontrollerslab.com/7-segment-display-pinout-working-examples-applications/
const int a = A0;
const int b = A1;
const int c = 2;
const int d = 3;
const int e = 4;
const int f = 5;
const int g = 6;

//ultra sonic distance sensor variables
const int trig = 7;
const int echo = 8;
long duration = 0;
float distance = 0;

//wing behaviours
const int flapDelay = 1000;
const int flapAngle = 45;
long lastFlap = 0;

//servo definitions
Servo leftWing;
Servo rightWing;
Servo neck;
Servo Jaw;

void setup() 
{
  Serial.begin(9600);

  motor.setMaxSpeed(1000);

  //7 segment pin initialisation
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);

  //distance sensor
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  Blink();
}

//opens the eyes
void EyesOpen()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
  delay(50);
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, LOW);

}

//closes the eyes
void EyesClosed()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
  delay(50);
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);

}

//runs through a blinking routine for the 7 segment
void Blink()
{
  EyesOpen();
  delay(300);
  EyesClosed();
  delay(300);
  EyesOpen();
  delay(300);
  EyesClosed();
  delay(300);
  EyesOpen();
}

//goes through the sleep protocol
void Sleep()
{
  leftWing.write(0);
  rightWing.write(0);
  EyesClosed();
}

//moves the dino back slightly
void MoveBack()
{
  leftWing.write(0);
  rightWing.write(0);
  motor.setSpeed(-1000);
  motor.runSpeed();
}

//moves the wings as well as the motor to control the dino
void MoveForward()
{
  if ((millis() - lastFlap) > 2*flapDelay)
  {
    leftWing.write(0);
    rightWing.write(0);
  }
  else if ((millis() - lastFlap) > flapDelay)
  {
    leftWing.write(flapAngle);
    rightWing.write(flapAngle);
  }
  motor.setSpeed(1000);
  motor.runSpeed();

  lastFlap = millis();
}

//stops the stepper motor rotation
void Stop()
{
  motor.setSpeed(0);
  motor.runSpeed();
}

void loop() 
{
  //uses ultrasound to get distance
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;

  if ((distance <= 50) && (distance > 8))
  {
    MoveForward();
  }
  else if (distance <= 5)
  {
    MoveBack();
  }
  else
  {
    Stop();
  }

  Serial.println(distance);

  LDRValue = analogRead(LDR);

  //Serial.println(LDRValue);

  if (LDRValue < 100)
  {
    sleeping = true;
  }
  else
  {
    sleeping = false;
  }

  //detects if there was a change between sleeping and being awake
  if (sleeping != previousState)
  {
    if (sleeping == true)
    {
      EyesClosed();
    }
    else if (sleeping == false)
    {
      EyesOpen();
    }
  }

  previousState = sleeping;

}

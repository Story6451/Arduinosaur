#include <Servo.h>
#include "AccelStepper.h"

const int MOTOR_INTERFACE = 8;

AccelStepper motor(MOTOR_INTERFACE, 9, 11, 10, 12);

//LDR variables
#define LDR A2
int LDRValue = 0;

bool sleeping = false;
bool previousState = false;

//7 segment display led variables (corresponds to the parts of a 7 semgent):
//https://microcontrollerslab.com/7-segment-display-pinout-working-examples-applications/
const int A = A0;
const int B = A1;
const int C = 2;
const int D = 3;
const int E = 4;
const int F = 5;
const int G = 6;

//ultra sonic distance sensor variables
const int TRIG = 7;
const int ECHO = 8;
long duration = 0;
float distance = 0;

//wing behaviours
const int FLAP_DELAY = 1000;
const int FLAP_ANGLE = 45;
long lastFlap = 0;

//servo definitions
Servo leftWing;
Servo rightWing;
Servo neck;
Servo jaw;

void setup() 
{
  Serial.begin(9600);

  motor.setMaxSpeed(1000);

  //7 segment pin initialisation
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);

  //distance sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Blink();
}

//opens the eyes
void EyesOpen()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  delay(50);
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, LOW);

}

//closes the eyes
void EyesClosed()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, HIGH);
  digitalWrite(G, HIGH);
  delay(50);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);

}

//conveys angry mode
void EyesAngry()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, LOW);
  digitalWrite(G, HIGH);
}

//conveys passive mode
void EyesPassive()
{
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);
  digitalWrite(E, HIGH);
  digitalWrite(F, HIGH);
  digitalWrite(G, LOW);
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
  if ((millis() - lastFlap) > (2 * FLAP_DELAY))
  {
    leftWing.write(0);
    rightWing.write(0);
  }
  else if ((millis() - lastFlap) > FLAP_DELAY)
  {
    leftWing.write(FLAP_ANGLE);
    rightWing.write(FLAP_ANGLE);
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
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);
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

  //for debugging
  //Serial.println(LDRValue);

  if (LDRValue < 100)
  {
    if (distance > 50)
    {
      sleeping = true;
    }
    else if(distance < 50)
    {
      EyesAngry();
      sleeping = false;
    }
  }
  else
  {
    EyesPassive();
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

#include <Servo.h>
#include <Stepper.h>

//stepper motor variables
const int steps = 2048;

Stepper motor(steps, 8, 9, 10, 11);

//LDR variables
#define LDR A0
int LDRValue = 0;

//7 segment display led variables (corresponds to the parts of a 7 semgent):
//https://microcontrollerslab.com/7-segment-display-pinout-working-examples-applications/
const int a = 0;
const int b = 1;
const int c = 2;
const int d = 3;
const int e = 4;
const int f = 5;
const int g = 6;
const int dp = 7;

//ultra sonic distance sensor variables
const int trig = 9;
const int echo = 10;
long duration = 0;
float distance = 0;

//wing behaviours
const int flapDelay = 1000;
const int flapAngle = 45;

//servo definitions
Servo leftWing;
Servo rightWing;
Servo neck;
Servo Jaw;

void setup() 
{
  Serial.begin(9600);

  motor.setSpeed(100);

  //7 segment pin initialisation
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(dp, OUTPUT);

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
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, LOW);
  digitalWrite(dp, LOW);
}

//closes the eyes
void EyesClosed()
{
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);
  digitalWrite(dp, LOW);
}

//runs through a blinking routine for the 7 segment
void Blink()
{
  EyesOpen();
  delay(200);
  EyesClosed();
  delay(200);
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
  motor.step(-steps);
}

//moves the wings as well as the motor to control the dino
long lastFlap = 0;
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
  motor.step(steps);

  lastFlap = millis();
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
    EyesOpen();
    MoveForward();
  }
  else if (distance <= 5)
  {
    EyesOpen();
    MoveBack();
  }


  LDRValue = analogRead(LDR);
  if (LDRValue < 300)
  {
    Sleep();
  }
}

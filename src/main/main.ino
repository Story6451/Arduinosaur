#include <Servo.h>
#include <Stepper.h>

//stepper motor variables
const int steps = 2048;

Stepper motor(steps, 8, 9, 10, 11);

//LDR variables
#define LDR A0
int LDRValue = 0;
int minLDRValue = 2000;
int maxLDRValue = 0;

//7 segment display led variables (corresponds to the parts of a 7 semgent):
//https://microcontrollerslab.com/7-segment-display-pinout-working-examples-applications/
const int a = 2;
const int b = 3;
const int c = 4;
const int d = 5;
const int e = 6;
const int f = 7;
const int g = 8;

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


  //distance sensor
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  Blink();

  attachInterrupt(digitalPinToInterrupt(trig), ISR_ObjectDetected, RISING);
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

}

//runs through a blinking routine for the 7 segment
void Blink()
{
  EyesOpen();
  delay(500);
  EyesClosed();
  delay(500);
  EyesOpen();
}

//goes through the sleep protocol
void Sleep()
{
  leftWing.write(0);
  rightWing.write(0);
  EyesClosed();
}

void Wake()
{
  Blink();
  EyesOpen();
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

void ISR_ObjectDetected()
{
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  EyesOpen();
  if ((distance <= 50) && (distance > 8))
  {
    MoveForward();
  }
  else if (distance <= 5)
  {
    MoveBack();
  }
}

void loop() 
{

  
  //uses ultrasound to get distance
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  

  LDRValue = analogRead(LDR);

  if (LDRValue > maxLDRValue)
  {
    maxLDRValue = LDRValue;
  }
  if (LDRValue < minLDRValue)
  {
    minLDRValue = LDRValue;
  }
  Serial.println(LDRValue);
  if (LDRValue > (minLDRValue + 0.8*maxLDRValue))
  {
    Sleep();
  }
  else
  {
    Wake();
  }
}

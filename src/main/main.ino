#include <Servo.h>

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

Servo leftWing;
Servo rightWing;

bool hunting = false;

void setup() 
{
  Serial.begin(9600);

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

void Sleep()
{
  leftWing.write(0);
  rightWing.write(0);
  EyesClosed();
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

  if (distance < 50)
  {
    hunting = true;
  }
  LDRValue = analogRead(LDR);
  if (LDRValue < 300)
  {
    Sleep();
  }
}

#include <Servo.h>
#include "AccelStepper.h"

const int MOTOR_INTERFACE = 8;

AccelStepper motor(MOTOR_INTERFACE, 9, 11, 10, 12);
const int MAX_SPEED = 5000;
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
long lastBlink = 0;


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
Servo jaw;

const int LEFT_WING_PIN = A3;
const int RIGHT_WING_PIN = A4;
const int JAW_PIN = A5;


void setup() 
{
  Serial.begin(9600);

  motor.setMaxSpeed(MAX_SPEED);

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

  leftWing.attach(LEFT_WING_PIN);
  rightWing.attach(RIGHT_WING_PIN);
  jaw.attach(JAW_PIN);
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
  motor.setSpeed(-MAX_SPEED);
  motor.runSpeed();
}

//moves the wings as well as the motor to control the dino
void MoveForward()
{
  if ((millis() - lastFlap) > (FLAP_DELAY))
  {
    leftWing.write(0);
    rightWing.write(0);
    Serial.print("LOW");
    lastFlap = millis();
  }
  else if ((millis() - lastFlap) > (0.5 * FLAP_DELAY))
  {
    leftWing.write(FLAP_ANGLE);
    rightWing.write(FLAP_ANGLE);
    Serial.print("HIGH");
  }
  motor.setSpeed(MAX_SPEED);
  motor.runSpeed();


}

//stops the stepper motor rotation
void Stop()
{
  motor.setSpeed(0);
  motor.runSpeed();
}

/*
void Inspect()
{
  neck.write(45);
  delay(2000);
  neck.write(0);
}
*/
void Bite()
{
  //neck.write(45);
  delay(500);
  jaw.write(45);
  delay(1000);
  jaw.write(0);
  //neck.write(0);
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

  LDRValue = analogRead(LDR);

  //for debugging
  
  Serial.print(lastFlap);
  Serial.print(" ");
  Serial.print(LDRValue);
  Serial.print(" ");
  Serial.println(distance);
  
  if (LDRValue < 300)
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
    sleeping = false;
    if (distance > 50)
    {
      long timer = millis();
      if ((timer - lastBlink) > 3000)
      {
        Blink();
        lastBlink = millis();
      }
    }
  }

  if ((distance <= 50) && (distance > 15))
  {
    MoveForward();
  }
  else if (distance <= 5)
  {
    MoveBack();
  }
  else if ((distance <= 10) && (distance > 5))
  {
    if (LDRValue < 300)
    {
      Bite();
    }
    else
    {
      //Inspect();
    }
  }
  else
  {
    Stop();
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

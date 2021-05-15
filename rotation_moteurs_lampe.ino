//setup
#include <Servo.h>
Servo motor;

//position du moteur
int pos = 1;
int servo = 90;
const int bob1 = 4;
const int bob2 = 5;
const int bob3 = 6;
const int bob4 = 7;
const int bob5 = 8;
const int bob6 = 9;
const int bob7 = 10;

//sensibilité des moteurs 
const int delayServo = 20;
const int delayStep = 7;

//sensibilité photorésistances
const int sensiH = 70;
const int sensiV = 25;
const int sensiVh = 20;

//initialisation du système de délai
int pasServo = 0;
int pasStep = 0;

//fonction definissant les positions du moteur pas à pas
void writeStep(int s){
  switch(s){
  case 1:
    digitalWrite(bob1,LOW);
    digitalWrite(bob2,LOW);
    digitalWrite(bob3,HIGH);
    digitalWrite(bob4,HIGH);
    digitalWrite(bob5,HIGH);
    digitalWrite(bob6,LOW);
    digitalWrite(bob7,LOW);
    break;
  case 2:
    digitalWrite(bob1,LOW);
    digitalWrite(bob2,LOW);
    digitalWrite(bob3,LOW);
    digitalWrite(bob4,LOW);
    digitalWrite(bob5,HIGH);
    digitalWrite(bob6,HIGH);
    digitalWrite(bob7,LOW);
    break;
  case 3:
    digitalWrite(bob1,LOW);
    digitalWrite(bob2,LOW);
    digitalWrite(bob3,LOW);
    digitalWrite(bob4,LOW);
    digitalWrite(bob5,LOW);
    digitalWrite(bob6,HIGH);
    digitalWrite(bob7,HIGH);
    break;
  case 4:
    digitalWrite(bob1,HIGH);
    digitalWrite(bob2,HIGH);
    digitalWrite(bob3,HIGH);
    digitalWrite(bob4,HIGH);
    digitalWrite(bob5,LOW);
    digitalWrite(bob6,LOW);
    digitalWrite(bob7,HIGH);
    break;
  }
}

//initialisation du programme
void setup() {
  pinMode(bob1,OUTPUT);
  pinMode(bob2,OUTPUT);
  pinMode(bob3,OUTPUT);
  pinMode(bob4,OUTPUT);
  pinMode(bob5,OUTPUT);
  pinMode(bob6,OUTPUT);
  pinMode(bob7,OUTPUT);
  motor.attach(3);

  writeStep(1);
  motor.write(servo);
  Serial.begin(9600);

}

//code executé en boucle
void loop() {
  pasStep = (pasStep+1)%delayStep;
  pasServo = (pasServo+1)%delayServo;

  if(pasStep == 0) testStep();
  if(pasServo == 0) testServo();
  
  delay(1);
}

//teste s'il faut faire tourner le moteur pas à pas
void testStep(){
  if(analogRead(0) > 600 || analogRead(3)-analogRead(5) >= sensiH){
    pos = (pos==4)? 0 : pos+1;
    writeStep(pos);
  } 
  else if(analogRead(0) < 500 || analogRead(5)-analogRead(3) >= sensiH){
    pos = (pos==0)? 4 : pos-1;
    writeStep(pos);
  }
}

//teste s'il faut faire tourner le servo moteur
void testServo(){
  if(analogRead(1) < 500 || (analogRead(4)-analogRead(3) >= sensiV && analogRead(4)-analogRead(5) >= sensiV)){
    servo++;
    motor.write(servo);
  } 
  else if(analogRead(1) > 600 || (analogRead(3)-analogRead(4) >= sensiVh && analogRead(5)-analogRead(4) >= sensiVh)){
    servo--;
    motor.write(servo);
  }
}


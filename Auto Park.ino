#include <ESP32Servo.h>
// 15 , 2 -> back ultra
// 22, 23 -> sec ultra

// front
const int ECHO1 = 21;
const int TRIGGER1 = 5;
// back
const int ECHO2 = 23;
const int TRIGGER2 = 22;

unsigned long onetime;
unsigned long twotime;
int dist1;
int dist2;
double x;

// Motor A
int IN1 = 19;  // Motor 1 (Left) control
int IN2 = 18;  // Motor 1 (Left) control
int enA = 26;  // Motor 1 speed control

// Motor B
int IN3 = 25;  // Motor 2 (Right) control
int IN4 = 33;  // Motor 2 (Right) control
int enB = 4;   // Motor 2 speed control

int space = 0;

Servo ultrasonicServo;

double UFun(int triger, int echo);

int Check();

void setup() {
  Serial.begin(9600);

  // Motor setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(enA, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(enB, OUTPUT);

  //ultrasonics setup
  pinMode(ECHO1, INPUT);
  pinMode(TRIGGER1, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIGGER2, OUTPUT);

  //servo setup
  ultrasonicServo.attach(14);

  // wait for 2 seconds
  delay(2000);
}

void loop() {
  ultrasonicServo.write(90);
  if (Check() == 1) {  //space is measured by trial and error to check the right value ,it's varied by the speed
    space = 0;
    stopMotors();
    delay(1000);
    park();
    // exit(0);
  } else {
    moveForward();
  }
}

void moveBackward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(enA, 80);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(enB, 70);
}
void moveForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(enA, 70);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(enB, 70);
}
void back_right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(enA, 70);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(enB, 0);
}
void back_left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(enA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(enB, 70);
}
void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(enA, 0);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(enB, 70);
}
void turnRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(enA, 70);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(enB, 0);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(enA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(enB, 0);
}

void park() {
  moveForward();
  delay(700);
  stopMotors();
  delay(1500);
  turnRight();
  delay(600);

  ultrasonicServo.write(180);
  delay(100);
  while (true) {
    x = UFun(TRIGGER1, ECHO1);
    Serial.println(x);
    if (x > 30) {
      moveBackward();
    } else
      break;
    delay(10);
  }
  stopMotors();
  delay(2000);
  back_right();
  delay(500);
  stopMotors();
  delay(5000);
}

int Check() {  //Chicking the sutable parking space for the car width
  int Flag = 0;
  while (UFun(TRIGGER1, ECHO1) > 20 && space < 25) {
    moveForward();
    space++;
    Flag = 1;
  }

  return Flag;
}

double UFun(int triger, int echo) {  //ultrasonic reading
  digitalWrite(triger, LOW);
  delayMicroseconds(2);

  digitalWrite(triger, HIGH);  //HIGH THEN 10 MICROSECONDS THEN LOW TO TRIGGER THE ULTRA SONIC
  delayMicroseconds(10);
  digitalWrite(triger, LOW);

  onetime = pulseIn(echo, HIGH);  //TO LAUNCH TIME CALCULATION
  dist2 = onetime * 0.0343 / 2;
  return dist2;
}

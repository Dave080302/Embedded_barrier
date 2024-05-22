#include <Servo.h>


const int buttonPin = 2;
const int greenPin = 3;
const int bluePin = 4;
const int redPin = 5;
const int echoPin = 6;
const int trigPin = 7;
const int servoPin = 9;
int r = 255, g = 0, b = 0;

int angle = 0;
volatile bool buttonState = LOW;

void myISR() {
  buttonState = !buttonState;
}
Servo servoMotor;
long duration;
int distance;

unsigned long objectDetectedTime = 0;

void setColor(int R, int G, int B) {
  analogWrite(redPin,   R);
  analogWrite(greenPin, G);
  analogWrite(bluePin,  B);
}

void setup()
{
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), myISR, FALLING);

  servoMotor.attach(servoPin);
  servoMotor.write(angle);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT_PULLUP);

  Serial.begin(9600);
}

int checkObjectDetected() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;
  if (distance == 0)
    return 0;
  if (distance < 10) {
    for (int i = 0; i < 10; i++) {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);

        distance = duration * 0.034 / 2;
        if (distance >= 10)
          return -1;
    }
    Serial.println("Object detected!");
    objectDetectedTime = millis();
    return 1;
  } else {
    return -1;
  }
}

void lift_servo() {
  float pre_pos = 0.0;
  bool objectDetected = false;

  for (float pos = 0.0; pos <= 100.0; pos += 0.01) {
    servoMotor.write(pos);
  }
}

void down_servo() {
  for (float pos = 100.0; pos >= 0.0; pos -= 0.01) {
    servoMotor.write(pos);

  }
}

bool carPassed = false;
void loop()
{
  setColor(r, g, b);
  if (buttonState)
  {
    Serial.println("Button pressed!");

    if(angle == 0) {
      r = 255;
      g = 255;
      b = 0;
      setColor(r, g, b);
      delay(1000);
      lift_servo();
      r = 0;
      delay(100);
      setColor(r, g, b);
      angle = 100;
    } else if(angle == 100) {
      objectDetectedTime = millis();
    }

    delay(200);
    buttonState = LOW;
  }

  int objectDetected = checkObjectDetected();
  if (angle == 100 && objectDetected > 0) {
    Serial.println("Car passed");
    Serial.println((millis() - objectDetectedTime));
    carPassed = true;
  }
  if (carPassed) {
    objectDetected = checkObjectDetected();
    bool still_there = false;
    for (int i = 0; i < 20; i++) {
      objectDetected = checkObjectDetected();
      if (objectDetected > 0) {
        still_there = true;
        break;
      }
    }
    if (millis() - objectDetectedTime >= 3000 && !still_there && carPassed) {
      r = 255;
      g = 255;
      b = 0;
      setColor(r, g, b);
      delay(100);
      down_servo();
      g = 0;
      setColor(r, g, b);
      angle = 0;
      carPassed = false;
    }
  }
}
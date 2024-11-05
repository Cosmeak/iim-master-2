#include <Servo.h>

Servo servoCircle;
Servo servoLinear;

const int ultrasonicTrigger = 9;
const int ultrasonicEcho = 8;

void setup() {
  Serial.begin(9600);
  pinMode(ultrasonicTrigger, OUTPUT);
  pinMode(ultrasonicEcho, INPUT);
  servoCircle.attach(6);
  servoCircle.write(1);
  servoLinear.attach(5);
  servoLinear.write(90);
}

void loop() {
  long duration, distance;

  digitalWrite(ultrasonicTrigger, LOW);  
	delayMicroseconds(2);  
	digitalWrite(ultrasonicTrigger, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(ultrasonicTrigger, LOW); 

  duration = pulseIn(ultrasonicEcho, HIGH);
  distance = microsecondsToCentimeters(duration);

  Serial.println(distance);
  Serial.println(servoLinear.read());

  if (distance < 10 && servoLinear.read() != 180) {
    servoLinear.write(180);
    servoCircle.detach();
  } else if (distance > 10 && servoLinear.read() != -180) {
    servoLinear.write(-180);
    servoCircle.attach(6);
    servoCircle.write(1);
  }

  delay(100);
}

long microsecondsToCentimeters(long microseconds) {
  return (microseconds / 2) / 29.1 ;
}
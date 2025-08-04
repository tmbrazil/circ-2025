#include <Servo.h>

#define CH1_PIN 7 // servos
#define CH2_PIN 8 // tracao
#define CH4_PIN 13 // (cima - tracao / baixo - braco)

#define LPWM_PIN_MIDDLE 4
#define RPWM_PIN_MIDDLE 5
#define LPWM_PIN_FRONT 6
#define RPWM_PIN_FRONT 7

#define SERVO_LEFT_F_PIN 12
#define SERVO_RIGHT_F_PIN 44
#define SERVO_LEFT_B_PIN 45
#define SERVO_RIGHT_B_PIN 46

// int CH1 = 0;
int CH2 = 0;
int CH4 = 0;
// int CH5 = 0;

Servo servoLFRONT;
Servo servoRFRONT;
Servo servoLBACK;
Servo servoRBACK;

void setup() {
  // Serial.begin(9600);

  pinMode(LPWM_PIN_MIDDLE, OUTPUT);
  pinMode(RPWM_PIN_MIDDLE, OUTPUT);
  pinMode(LPWM_PIN_FRONT, OUTPUT);
  pinMode(RPWM_PIN_FRONT, OUTPUT);

  analogWrite(LPWM_PIN_MIDDLE, 0);
  analogWrite(RPWM_PIN_MIDDLE, 0);
  analogWrite(LPWM_PIN_FRONT, 0);
  analogWrite(RPWM_PIN_FRONT, 0);

  servoLFRONT.attach(SERVO_LEFT_F_PIN);
  servoRFRONT.attach(SERVO_RIGHT_F_PIN);
  servoLBACK.attach(SERVO_LEFT_B_PIN);
  servoRBACK.attach(SERVO_RIGHT_B_PIN);
  
  servoLFRONT.write(90);
  servoRFRONT.write(90);
  servoLBACK.write(90);
  servoRBACK.write(90);
}

void loop() {
  RC_READ();
  
  SET_MOTORS(CH2);
  SET_ROVER_SERVOS(CH4);
  
  delay(50);
}

void RC_READ(void) {
  CH2 = pulseIn(CH2_PIN, HIGH);
  CH2 = constrain(map(CH2, 987, 1973, -255, 255), -255, 255);
  if (abs(CH2) < 20) {
    CH2 = 0;
  }

  CH4 = pulseIn(CH4_PIN, HIGH);
  CH4 = constrain(map(CH4, 994, 1988, 60, 120), 60, 120);
  if (abs(90 - CH4) <= 3) {
    CH4 = 90;
  }
}

void SET_MOTORS(int velocity) {
  if (velocity >= 0) {
    analogWrite(LPWM_PIN_MIDDLE, velocity);
    analogWrite(RPWM_PIN_MIDDLE, 0);

    analogWrite(LPWM_PIN_FRONT, velocity);
    analogWrite(RPWM_PIN_FRONT, 0);
  } else {
    analogWrite(LPWM_PIN_MIDDLE, 0);
    analogWrite(RPWM_PIN_MIDDLE, -velocity);

    analogWrite(LPWM_PIN_FRONT, 0);
    analogWrite(RPWM_PIN_FRONT, -velocity);
  }
}

void SET_ROVER_SERVOS(int angle) {
  servoLFRONT.write(angle);
  servoRFRONT.write(angle);
  servoLBACK.write(180 - angle);
  servoRBACK.write(180 - angle);
}

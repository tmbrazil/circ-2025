#include <Servo.h>

#define CH1_PIN 7 // servos
#define CH2_PIN 8 // tracao
#define CH5_PIN 13 // (cima - tracao / baixo - braco)

#define RPWM_PIN 6
#define LPWM_PIN 5

#define SERVO_LEFT_F_PIN 12
#define SERVO_RIGHT_F_PIN 44
#define SERVO_LEFT_B_PIN 45
#define SERVO_RIGHT_B_PIN 46

int CH1;
int CH2;
int CH5;

Servo servoEsqFrente;
Servo servoDirFrente;
Servo servoEsqTras;
Servo servoDirTras;

bool state;

void setup() {
  Serial.begin(9600);

  pinMode(RPWM_PIN, OUTPUT);
  pinMode(LPWM_PIN, OUTPUT);

  analogWrite(RPWM_PIN, 0);
  analogWrite(LPWM_PIN, 0);

  servoEsqFrente.attach(SERVO_LEFT_F_PIN);
  servoDirFrente.attach(SERVO_RIGHT_F_PIN);
  servoEsqTras.attach(SERVO_LEFT_B_PIN);
  servoDirTras.attach(SERVO_RIGHT_B_PIN);
  
  servoEsqFrente.write(0);
  servoDirFrente.write(0);
  servoEsqTras.write(0);
  servoDirTras.write(0);
}

void loop() {
  RC_READ();
  
  state = CH5 < 0 ? 1 : 0;

  if (state) {
    SET_MOTORS(CH2);
    SET_SERVOS(CH1);
    // Serial.print(CH1);
    // Serial.print(" - ");
    // Serial.println(CH2);

  } else {
    Serial.println("B");
  }

  delay(50);
}

void RC_READ(void) {
  CH1 = pulseIn(CH1_PIN, HIGH);
  CH1 = constrain(map(CH1, 994, 1988, 60, 120), 60, 120);
  if (abs(90 - CH1) <= 3) {
    CH1 = 90;
  }

  CH2 = pulseIn(CH2_PIN, HIGH);
  CH2 = constrain(map(CH2, 987, 1973, -255, 255), -255, 255);
  if (abs(CH2) < 30) {
    CH2 = 0;
  }

  CH5 = (pulseIn(CH5_PIN, HIGH) - RC_CENTER_PULSE);
}

void SET_MOTORS(int velocidade) {
  if (velocidade >= 0) {
    analogWrite(RPWM_PIN, velocidade);
    analogWrite(LPWM_PIN, 0);
  } else {
    analogWrite(RPWM_PIN, 0);
    analogWrite(LPWM_PIN, -velocidade);
  }
}

void SET_SERVOS(int angle) {
  servoEsqFrente.write(angle);
  servoDirFrente.write(angle);
  servoEsqTras.write(90 - angle);
  servoDirTras.write(90 - angle);
}

void controlarServoBraco();

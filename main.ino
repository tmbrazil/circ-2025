#include <Servo.h>

// #define CH1_PIN 7 // servos
#define CH2_PIN 8 // tracao
// #define CH5_PIN 13 // (cima - tracao / baixo - braco)

#define LPWM_PIN_MEIO 4
#define RPWM_PIN_MEIO 5
#define LPWM_PIN_FRENTE 6
#define RPWM_PIN_FRENTE 7

// #define SERVO_LEFT_F_PIN 12
// #define SERVO_RIGHT_F_PIN 44
// #define SERVO_LEFT_B_PIN 45
// #define SERVO_RIGHT_B_PIN 46

int CH1 = 0;
int CH2 = 0;
int CH5 = 0;

// Servo servoEsqFrente;
// Servo servoDirFrente;
// Servo servoEsqTras;
// Servo servoDirTras;

bool state;

void setup() {
  Serial.begin(9600);

  pinMode(LPWM_PIN_MEIO, OUTPUT);
  pinMode(RPWM_PIN_MEIO, OUTPUT);
  pinMode(LPWM_PIN_FRENTE, OUTPUT);
  pinMode(RPWM_PIN_FRENTE, OUTPUT);

  analogWrite(LPWM_PIN_MEIO, 0);
  analogWrite(RPWM_PIN_MEIO, 0);
  analogWrite(LPWM_PIN_FRENTE, 0);
  analogWrite(RPWM_PIN_FRENTE, 0);

  // servoEsqFrente.attach(SERVO_LEFT_F_PIN);
  // servoDirFrente.attach(SERVO_RIGHT_F_PIN);
  // servoEsqTras.attach(SERVO_LEFT_B_PIN);
  // servoDirTras.attach(SERVO_RIGHT_B_PIN);
  
  // servoEsqFrente.write(0);
  // servoDirFrente.write(0);
  // servoEsqTras.write(0);
  // servoDirTras.write(0);
}

void loop() {
  RC_READ();
  
  // state = CH5 < 0 ? 1 : 0;

  SET_MOTORS(CH2);
    // Serial.print(CH1);
    // Serial.print(" - ");
    // Serial.println(CH2);

  delay(50);
}

void RC_READ(void) {
  // CH1 = pulseIn(CH1_PIN, HIGH);
  // CH1 = constrain(map(CH1, 994, 1988, 60, 120), 60, 120);
  // if (abs(90 - CH1) <= 3) {
  //   CH1 = 90;
  // }

  CH2 = pulseIn(CH2_PIN, HIGH);
  CH2 = constrain(map(CH2, 987, 1973, -255, 255), -255, 255);
  if (abs(CH2) < 20) {
    CH2 = 0;
  }

  // CH5 = (pulseIn(CH5_PIN, HIGH) - RC_CENTER_PULSE);
}

void SET_MOTORS(int velocidade) {
  if (velocidade >= 0) {
    analogWrite(LPWM_PIN_MEIO, velocidade);
    analogWrite(RPWM_PIN_MEIO, 0);

    analogWrite(LPWM_PIN_FRENTE, velocidade);
    analogWrite(RPWM_PIN_FRENTE, 0);
  } else {
    analogWrite(LPWM_PIN_MEIO, 0);
    analogWrite(RPWM_PIN_MEIO, -velocidade);

    analogWrite(LPWM_PIN_FRENTE, 0);
    analogWrite(RPWM_PIN_FRENTE, -velocidade);
  }
}

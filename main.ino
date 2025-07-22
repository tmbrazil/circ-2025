#include <Servo.h>

#define CH1_PIN 30 // servos
#define CH3_PIN 31 // tracao
#define CH5_PIN 32 // (cima - tracao / baixo - braco)

#define R_PWM 5
#define L_PWM 9

#define SERVO_LEFT_F 12
#define SERVO_RIGHT_F 44
#define SERVO_LEFT_B 45
#define SERVO_RIGHT_B 46

#define RC_CENTER_PULSE 1491
#define RC_SCALE_FACTOR 0.513
#define DEADZONE_WIDTH 30

int CH1;
int CH3;
int CH5;

enum State {
    MOTOR,
    ARM,
    STANDBY
};

State currentState;

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando o teste do motor...");

  // pinMode(R_EN, OUTPUT);
  // pinMode(L_EN, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);

  // digitalWrite(R_EN, HIGH);
  // digitalWrite(L_EN, HIGH);

  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 0);

  Servo servoEsqFrente;
  Servo servoDirFrente;
  Servo servoEsqTras;
  Servo servoDirTras;
}

void loop() {
  lerControle();
  
  currentState = CH5 < 0 ? MOTOR : ARM;
  
  Serial.print("CH1: ");
  Serial.print(CH1);
  Serial.print(" | CH3: ");
  Serial.print(CH3);
  Serial.print(" | MODE: ");
  
  if (currentState == MOTOR) {
  Serial.print("MOTOR\n");
  } else {
  Serial.println("BRACO");
  }

  switch(currentState) {
    case MOTOR:
      controlarMotor(CH3);
      controlarServo(CH1);
      break;

    case ARM:
      delay(100);
      break;
  }

  delay(100);
}

void controlarMotor(int velocidade) {
  
  if (velocidade >= 0) {
    //Mover para FRENTE
    analogWrite(R_PWM, constrain(velocidade, 0, 255));
    analogWrite(L_PWM, 0);
  } else {
    // Mover para TRÁS
    analogWrite(R_PWM, 0);
    analogWrite(L_PWM, constrain(abs(velocidade), 0, 255));
  }
}

void controlarServo(int angle) {

}

void controlarServoBraco();

void lerControle(void) {
  CH1 = (pulseIn(CH1_PIN, HIGH) - RC_CENTER_PULSE);
  if (CH1 == -RC_CENTER_PULSE) {
    CH1 = 0;
  } else if ((abs(CH1) < DEADZONE_WIDTH)) {
    CH1 = 0;
  } else {
  CH1 = CH1 * RC_SCALE_FACTOR;
  }

  CH3 = (pulseIn(CH3_PIN, HIGH) - RC_CENTER_PULSE);
  if (CH3 == -RC_CENTER_PULSE){
    CH3 = 0;
  } else if ((abs(CH3) < DEADZONE_WIDTH)) {
    CH3 = 0;
  } else {
  CH3 = CH3 * RC_SCALE_FACTOR;
  }

  CH5 = (pulseIn(CH5_PIN, HIGH) - RC_CENTER_PULSE);
}

#define CH1_PIN 7 // servos
#define CH3_PIN 3 // tracao
#define CH5_PIN 5 // (cima - tracao / baixo - braco)
#define MOTOR_ENB 10 // Pino de velocidade (Enable B), deve ser um pino PWM (~).
#define MOTOR_IN3 8  // Pino de direção 1
#define MOTOR_IN4 9  // Pino de direção 2

#define RC_CENTER_PULSE 1490
#define RC_SCALE_FACTOR 0.514
#define DEADZONE_WIDTH 90

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

  pinMode(MOTOR_ENB, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);

  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
  analogWrite(MOTOR_ENB, 0);
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
      break;

    case ARM:
      delay(100);
      break;
  }

  delay(100);
}

void controlarMotor(int velocidade) {
  if (velocidade >= 0) {
    // Mover para FRENTE
    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);
  } else {
    // Mover para TRÁS
    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);
  }

  int velocidadeAbsoluta = abs(velocidade);

  analogWrite(MOTOR_ENB, constrain(velocidadeAbsoluta, 0, 255));
}

void lerControle(void) {
  CH1 = (pulseIn(CH1_PIN, HIGH) - RC_CENTER_PULSE);
  if (CH1 == -RC_CENTER_PULSE) {
    CH1 = 0;
  } else if (CH1 > - DEADZONE_WIDTH && CH1 < DEADZONE_WIDTH) {
  CH1 = 0;
  } else if (CH1 > 0) {
  CH1 = CH1 * RC_SCALE_FACTOR;
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

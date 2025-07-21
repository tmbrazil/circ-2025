#define FRENTE_RE 3
#define MOTOR_ENB 10 // Pino de velocidade (Enable B), deve ser um pino PWM (~).
#define MOTOR_IN3 8  // Pino de direção 1
#define MOTOR_IN4 9  // Pino de direção 2

int CH3_FR;

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
  RC_READ();

  Serial.println(CH3_FR);

  if (abs(CH3_FR) < 90) {
    controlarMotor(0);
  } else {controlarMotor(CH3_FR);}
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

void RC_READ(void) {
  CH3_FR = (pulseIn(FRENTE_RE, HIGH) - 1490);

  if (CH3_FR == -1490) {
    CH3_FR = 0;
  } else if (CH3_FR > -30 && CH3_FR < 30) {
  CH3_FR = 0;
  } else if (CH3_FR > 0) {
  CH3_FR = CH3_FR * 0.514;
  } else {
  CH3_FR = CH3_FR * 0.514;
  }

}
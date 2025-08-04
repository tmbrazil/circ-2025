const int stepPin = 5; 
const int dirPin = 2; 
const int enPin = 8;

#define CH5_PIN 12

int CH5 = 0;
float anguloAtual = 0; // Armazena a posição atual do motor (em graus)

const float passosPorVolta = 1600.0; // TB6600 com 1/8 de passo
const float grausPorPasso = 360.0 / passosPorVolta;

void setup() {
//   Serial.begin(9600);
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW); // Ativa o driver
}

void loop() {
  RC_READ(); // Atualiza o valor de CH5 (ângulo desejado)

  float anguloDesejado = CH5;

  float delta = anguloDesejado - anguloAtual;

  if (abs(delta) > 3.8) { // Só move se a diferença for significativa
    bool sentidoHorario = delta > 0;
    int passos = abs(delta) / grausPorPasso;

    digitalWrite(dirPin, sentidoHorario ? HIGH : LOW);

    for (int i = 0; i < passos; i++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(1800);  // Velocidade do motor (ajustável)
      digitalWrite(stepPin, LOW);
      delayMicroseconds(1800);
    }

    // Atualiza o ângulo atual após o movimento
    anguloAtual += sentidoHorario ? passos * grausPorPasso : -passos * grausPorPasso;
  }

//   Serial.print("PWM CH5: "); Serial.print(CH5);
//   Serial.print(" | Angulo Atual: "); Serial.println(anguloAtual);

  delay(50); // Pequeno delay para estabilidade
}

void RC_READ(void) {
  CH5 = pulseIn(CH5_PIN, HIGH);
  CH5 = constrain(map(CH5, 995, 1987, -180, 180), -180, 180);
  if (abs(CH5) < 20) {
    CH5 = 0; // zona morta para evitar ruído
  }
}

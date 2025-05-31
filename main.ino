#include <IRremote.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial BTSerial(19, 18); //RX, TX

const int ENA_HD = 2;   // PWM PONTE H DIANTEIRA
const int ENB_HD = 3;   // PWM PONTE H DIANTEIRA
const int ENA_HT = 4;   // PWM PONTE H TRASEIRA
const int ENB_HT = 5;   // PWM PONTE H TRASEIRA

const int IN1_HT = 34;   // Controle motor esquerdo traseiro
const int IN2_HT = 43;
const int IN3_HT = 42;   // Controle motor direito traseiro
const int IN4_HT = 35;
const int IN1_HD = 30;   // Controle motor esquerdo dianteiro
const int IN2_HD = 31;
const int IN3_HD = 32;  // Controle motor direito dianteiro
const int IN4_HD = 33;

const int servoEsqFrentePin = 12;
const int servoDirFrentePin = 44;
const int servoEsqTrasPin = 45;
const int servoDirTrasPin = 46;
Servo servoEsqFrente;
Servo servoDirFrente;
Servo servoEsqTras;
Servo servoDirTras;

// const int trigFrente = 46;
// const int echoFrente = 44;
const int infraRedPin = 19;
const int redLedPin = 22;

// Configuração dos servos
const int servoCentroEsq = 110;
const int servoCentroDir = 90;
const int SERVO_ANGULO_MAX = 45; // Ângulo máximo de esterçamento

unsigned long ultimaAtualizacao = 0;

char data = '0';

// =================== CONFIGURAÇÃO INICIAL ===================
void setup() {
  pinMode(ENA_HD, OUTPUT);
  pinMode(ENB_HD, OUTPUT);
  
  pinMode(IN1_HD, OUTPUT);
  pinMode(IN2_HD, OUTPUT);
  pinMode(IN3_HD, OUTPUT);
  pinMode(IN4_HD, OUTPUT);
  pinMode(IN1_HT, OUTPUT);
  pinMode(IN2_HT, OUTPUT);
  pinMode(IN3_HT, OUTPUT);
  pinMode(IN4_HT, OUTPUT);
  
  pinMode(ENA_HT, OUTPUT);
  pinMode(ENB_HT, OUTPUT);


  // IrReceiver.begin(infraRedPin);
  servoEsqFrente.attach(servoEsqFrentePin);
  servoDirFrente.attach(servoDirFrentePin);
  servoEsqTras.attach(servoEsqTrasPin);
  servoDirTras.attach(servoDirTrasPin);

  alinharServos();

  piscarLED(redLedPin, 3, 200);

  Serial.begin(9600);
  BTSerial.begin(9600);

  Serial.println("Sistema Pronto - Aguardando ativacao (*)");
}

void loop() {
  if (BTSerial.available()) {
    data = BTSerial.read();
    
    switch(data) {
      case '0':
        Serial.println("Motores parados - Aguardando comenado");
        pararMotores();
        break;
      case 'A':
        Serial.println("Executar Passada");
        executarPassada();
        break;
      case 'B':
        Serial.println("Executar Teste dos Servos");
        executarTesteServos();
        break;
      case 'C':
        Serial.println("Executar Danca!");
        executarDanca();
        break;
    }
  }
    
    //   // Atualização da velocidade (executa a cada 50ms)
    // if (millis() - ultimaAtualizacao >= 50) {
    //     atualizarVelocidade();
    //     ultimaAtualizacao = millis();
    // }







    // if (BTSerial.available()) {
    //     String data = BTSerial.readStringUntil('\n');
    //     int commaIndex = data.indexOf(',');
        
    //   if (commaIndex > 0) {
    //       comandoTracao = data.substring(0, commaIndex).toInt();
    //       direcaoAtual = data.substring(commaIndex+1).toInt();
    //       direcaoAtual = constrain(direcaoAtual, 0, 180);
    //       servoDirecao.write(direcaoAtual);
    //   }
    // }
    
    //   // Atualização da velocidade (executa a cada 50ms)
    // if (millis() - ultimaAtualizacao >= 50) {
    //     atualizarVelocidade();
    //     ultimaAtualizacao = millis();
    // }
}

void executarPassada() {
  mover(150, 150);
  delay(3000);

  mover(-150, -150);
  delay(3000);

  data = '0';
}

void executarTesteServos() {
  girarEsquerda(90);
  delay(500);
  girarDireita(90);
  delay(500);

  girarEsquerda(90);
  delay(500);
  girarDireita(90);
  delay(500);

  alinharServos();
  delay(100);

  mover(200,200);
  delay(1000);

  curvaEsquerda(90, 3000);

  mover(200,200);
  delay(1000);

  curvaDireita(90, 3000);

  pararMotores();

  data = '0';
}

void executarDanca() {
  curvaDireita(90, 5000);
  curvaEsquerda(90, 5000);

  data = '0';
}

// =================== FUNÇÕES DE CONTROLE ===================
void mover(int velEsq, int velDir) {
  digitalWrite(IN1_HT, velEsq > 0);
  digitalWrite(IN2_HT, velEsq <= 0);
  digitalWrite(IN1_HD, velEsq > 0);
  digitalWrite(IN2_HD, velEsq <= 0);
  digitalWrite(IN3_HT, velDir > 0);
  digitalWrite(IN4_HT, velDir <= 0);
  digitalWrite(IN3_HD, velDir > 0);
  digitalWrite(IN4_HD, velDir <= 0);

  analogWrite(ENA_HD, abs(velEsq));
  analogWrite(ENB_HT, abs(velEsq));
  
  analogWrite(ENB_HD, abs(velDir));
  analogWrite(ENA_HT, abs(velDir));
}

void moverCurva(int velEsq, int velDir, int angulo) {
  digitalWrite(IN1_HT, velEsq > 0);
  digitalWrite(IN2_HT, velEsq <= 0);
  digitalWrite(IN1_HD, velEsq > 0);
  digitalWrite(IN2_HD, velEsq <= 0);
  digitalWrite(IN3_HT, velDir > 0);
  digitalWrite(IN4_HT, velDir <= 0);
  digitalWrite(IN3_HD, velDir > 0);
  digitalWrite(IN4_HD, velDir <= 0);
  
  int velocidadeMeio = (int)(cos(angulo) * velEsq);

  analogWrite(ENA_HD, abs(velocidadeMeio));
  analogWrite(ENB_HT, abs(velocidadeMeio));
  
  analogWrite(ENB_HD, abs(velEsq));
  analogWrite(ENA_HT, abs(velDir));
}

void pararMotores() {
  mover(0, 0);
}

// =================== FUNÇÕES DE DIREÇÃO ===================
void alinharServos() {
  servoEsqFrente.write(servoCentroEsq);
  servoDirFrente.write(servoCentroDir);

  servoEsqTras.write(servoCentroEsq);
  servoDirTras.write(servoCentroDir);
}

// void ajustarDirecao(int angulo) {
//   // Angulo positivo = direita, negativo = esquerda
//   angulo = constrain(angulo, SERVO_ANGULO_MAX, SERVO_ANGULO_MAX);
  
//   servoEsqFrente.write(servoCentroEsq + angulo);
//   servoDir.write(servoCentroDir + angulo);
// }

void girarDireita(int angulo) {
  servoEsqFrente.write(servoCentroEsq + angulo);
  servoDirFrente.write(servoCentroDir + angulo);

  servoEsqTras.write(servoCentroEsq - angulo);
  servoDirTras.write(servoCentroDir - angulo);
}

void girarEsquerda(int angulo) {
  servoEsqFrente.write(servoCentroEsq - angulo);
  servoDirFrente.write(servoCentroDir - angulo);

  servoEsqTras.write(servoCentroEsq + angulo);
  servoDirTras.write(servoCentroDir + angulo);

}

void curvaEsquerda(int angulo, int tempo) {
  girarEsquerda(angulo);
  delay(100);

  moverCurva(100, 100, -angulo);
  delay(tempo);

  alinharServos();
  delay(100);
}

void curvaDireita(int angulo, int tempo) {
  girarDireita(angulo);
  delay(100);

  moverCurva(100, 100, angulo);
  delay(tempo);

  alinharServos();
  delay(100);
}

// =================== FUNÇÕES DE SENSORIAMENTO ===================
// float medirDistancia(int trigPin, int echoPin) {
//   digitalWrite(trigPin, LOW);
//   delayMicroseconds(2);
//   digitalWrite(trigPin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trigPin, LOW);
  
//   long duracao = pulseIn(echoPin, HIGH);
//   return (duracao * 0.0343) / 2;  // Distância em cm
// }

// void verificarObstaculos() {
//   float distFrente = medirDistancia(trigFrente, echoFrente);
//   float distEsq = medirDistancia(trigEsq, echoEsq);
//   float distDir = medirDistancia(trigDir, echoDir);

//   // Obstáculo frontal crítico
//   if (distFrente < 15) {
//     Serial.println("Obstaculo frontal detectado!");
//     pararMotores();
//     delay(300);
    
//     if (distEsq > distDir) {
//       girarEsquerda(SERVO_ANGULO_MAX);
//     } else {
//       girarDireita(SERVO_ANGULO_MAX);
//     }
//     mover(100, 100);
//     delay(800);
//     alinharServos();
//   }
// }

// =================== CONTROLE POR IR ===================
// void verificarControleIR() {
//   if (IrReceiver.decode()) {
//     if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00) {
//       sistemaAtivo = true;
//       estadoAtual = MARCO_2;
//       piscarLED(LED_STATUS_PIN, 1, 500);
//       Serial.println("Sistema ATIVADO - Iniciando Marco 1");
//     }
//     else if (IrReceiver.decodedIRData.decodedRawData == 0xF20DFF00) {
//       sistemaAtivo = false;
//       estadoAtual = INICIO;
//       pararMotores();
//       piscarLED(LED_STATUS_PIN, 2, 200);
//       Serial.println("Sistema DESATIVADO");
//     }
//     IrReceiver.resume();
//   }
// }

// ==========================================================
// =================== FUNÇÕES AUXILIARES ===================
// ==========================================================
void piscarLED(int pin, int vezes, int intervalo) {
  for (int i = 0; i < vezes; i++) {
    digitalWrite(pin, HIGH);
    delay(intervalo);
    digitalWrite(pin, LOW);
    if (i < vezes - 1) delay(intervalo);
  }
}

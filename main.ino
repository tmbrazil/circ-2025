#include <IRremote.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial BTSerial(10, 11); //RX, TX

// const int ENA_HT = 2;   // PWM motor direito traseiro
// const int ENB_HT = 3;   // PWM motor esquerdo traseiro
// const int IN1_HT = 7;   // Controle motor esquerdo traseiro
// const int IN2_HT = 6;
// const int IN3_HT = 5;   // Controle motor direito traseiro
// const int IN4_HT = 4;
// const int IN1_HD = 8;   // Controle motor esquerdo dianteiro
// const int IN2_HD = 9;
// const int IN3_HD = 10;  // Controle motor direito dianteiro
// const int IN4_HD = 11;

const int servoEsqFrentePin = 4;
const int servoDirFrentePin = 5;
const int servoEsqTrasPin = 6;
const int servoDirTrasPin = 7;
Servo servoEsqFrente;
Servo servoDirFrente;
Servo servoEsqTras;
Servo servoDirTras;

const int trigFrente = 46;
const int echoFrente = 44;
const int infraRedPin = 19;
const int redLedPin = 22;

#define SerialRPi Serial

// Configuração dos servos
const int servoCentroEsq = 110;
const int servoCentroDir = 90;
const int SERVO_ANGULO_MAX = 45; // Ângulo máximo de esterçamento

unsigned long ultimaAtualizacao = 0;

String data = '0'

// =================== CONFIGURAÇÃO INICIAL ===================
void setup() {
  pinMode(ENA_HT, OUTPUT);
  pinMode(ENB_HT, OUTPUT);
  for (int i = 4; i <= 11; i++) pinMode(i, OUTPUT);

  pinMode(trigFrente, OUTPUT);
  pinMode(echoFrente, INPUT);
  pinMode(redLedPin, OUTPUT);

  IrReceiver.begin(infraRedPin);
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
    data = BTSerial.readStringUntil('\n');
    
    switch(data) {
      case '0':
        pararMotores();
        break;
      case 'A':
        executarPassada();
        break;
      case 'B':
        executarTesteServos();
        break;
      case 'C':
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
  delay(5000);

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

  analogWrite(ENB_HT, abs(velEsq));
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

  mover(100,100);
  delay(tempo);

  alinharServos();
  delay(100);
}

void curvaDireita(int angulo, int tempo) {
  girarDireita(angulo);
  delay(100);

  mover(100,100);
  delay(tempo);

  alinharServos();
  delay(100);
}

// =================== FUNÇÕES DE SENSORIAMENTO ===================
float medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duracao = pulseIn(echoPin, HIGH);
  return (duracao * 0.0343) / 2;  // Distância em cm
}

void verificarObstaculos() {
  float distFrente = medirDistancia(trigFrente, echoFrente);
  float distEsq = medirDistancia(trigEsq, echoEsq);
  float distDir = medirDistancia(trigDir, echoDir);

  // Obstáculo frontal crítico
  if (distFrente < 15) {
    Serial.println("Obstaculo frontal detectado!");
    pararMotores();
    delay(300);
    
    if (distEsq > distDir) {
      girarEsquerda(SERVO_ANGULO_MAX);
    } else {
      girarDireita(SERVO_ANGULO_MAX);
    }
    mover(100, 100);
    delay(800);
    alinharServos();
  }
}

// =================== CONTROLE POR IR ===================
void verificarControleIR() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00) {
      sistemaAtivo = true;
      estadoAtual = MARCO_2;
      piscarLED(LED_STATUS_PIN, 1, 500);
      Serial.println("Sistema ATIVADO - Iniciando Marco 1");
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xF20DFF00) {
      sistemaAtivo = false;
      estadoAtual = INICIO;
      pararMotores();
      piscarLED(LED_STATUS_PIN, 2, 200);
      Serial.println("Sistema DESATIVADO");
    }
    IrReceiver.resume();
  }
}

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

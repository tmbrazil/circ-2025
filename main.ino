#include <IRremote.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial BTSerial(10, 11); //RX, TX

// Motores
const int ENA_HT = 2;   // PWM motor direito traseiro
const int ENB_HT = 3;   // PWM motor esquerdo traseiro
const int IN1_HT = 7;   // Controle motor esquerdo traseiro
const int IN2_HT = 6;
const int IN3_HT = 5;   // Controle motor direito traseiro
const int IN4_HT = 4;
const int IN1_HD = 8;   // Controle motor esquerdo dianteiro
const int IN2_HD = 9;
const int IN3_HD = 10;  // Controle motor direito dianteiro
const int IN4_HD = 11;

// Servos para direção
const int servoEsqPin = 12;
const int servoDirPin = 45;
Servo servoEsq;
Servo servoDir;

// Modulos
const int trigFrente = 46;
const int echoFrente = 44;
const int infraRedPin = 19;
const int redLedPin = 22;

#define SerialRPi Serial

// Configuração dos servos
const int servoCentroEsq = 110;
const int servoCentroDir = 90;
const int SERVO_ANGULO_MAX = 45; // Ângulo máximo de esterçamento

#define VELOCIDADE_MAX 200
#define ACELERACAO 5     // Incremento de velocidade por ciclo
#define DESACELERACAO 10 // Decremento quando parando
#define CURVA_SUAVE 30   // Redução de velocidade nas curvas

int velocidadeAtual = 0;
int direcaoAtual = 90;
int comandoTracao = 0;
unsigned long ultimaAtualizacao = 0;

// =================== CONFIGURAÇÃO INICIAL ===================
void setup() {
  // Configuração dos pinos
  pinMode(ENA_HT, OUTPUT);
  pinMode(ENB_HT, OUTPUT);
  for (int i = 4; i <= 11; i++) pinMode(i, OUTPUT);

  pinMode(trigFrente, OUTPUT);
  pinMode(echoFrente, INPUT);
  pinMode(redLedPin, OUTPUT);
  IrReceiver.begin(infraRedPin);
  servoEsq.attach(servoEsqPin);
  servoDir.attach(servoDirPin);
  alinharServos();

  piscarLED(redLedPin, 3, 200);

  Serial.begin(9600);
  BTSerial.begin(9600);

  Serial.println("Sistema Pronto - Aguardando ativacao (*)");
}

void loop() {
    if (BTSerial.available()) {
        String data = BTSerial.readStringUntil('\n');
        int commaIndex = data.indexOf(',');
        
        if (commaIndex > 0) {
          comandoTracao = data.substring(0, commaIndex).toInt();
          direcaoAtual = data.substring(commaIndex+1).toInt();
          direcaoAtual = constrain(direcaoAtual, 0, 180);
          servoDirecao.write(direcaoAtual);
        }
      }
    
      // Atualização da velocidade (executa a cada 50ms)
      if (millis() - ultimaAtualizacao >= 50) {
        atualizarVelocidade();
        ultimaAtualizacao = millis();
      }
}

void atualizarMovimento() {
    // Altera a velocidade gradualmente
    if (comandoTracao == 1) { // Comando para frente
      if (velocidadeAtual < VELOCIDADE_MAX) {
        velocidadeAtual += ACELERACAO;
      }
    } 
    else if (comandoTracao == -1) { // Comando para ré
      if (velocidadeAtual > -VELOCIDADE_MAX) {
        velocidadeAtual -= ACELERACAO;
      }
    } 
    else { // Parar
      if (velocidadeAtual > 0) {
        velocidadeAtual = max(0, velocidadeAtual - DESACELERACAO);
      } else if (velocidadeAtual < 0) {
        velocidadeAtual = min(0, velocidadeAtual + DESACELERACAO);
      }
    }
  
    // Calcula velocidades considerando a direção
    int velEsq = velocidadeAtual;
    int velDir = velocidadeAtual;
    
    // Ajuste para curvas (exemplo simples)
    if (direcaoAtual < 80) { // Virando para esquerda
      velDir = map(direcaoAtual, 0, 80, velocidadeAtual - CURVA_SUAVE, velocidadeAtual);
    } 
    else if (direcaoAtual > 100) { // Virando para direita
      velEsq = map(direcaoAtual, 100, 180, velocidadeAtual, velocidadeAtual - CURVA_SUAVE);
    }
  
    // Garante limites mínimos
    velEsq = constrain(velEsq, -VELOCIDADE_MAX, VELOCIDADE_MAX);
    velDir = constrain(velDir, -VELOCIDADE_MAX, VELOCIDADE_MAX);
  
    // Chama sua função mover()
    mover(velEsq, velDir);
    
    // Debug
    Serial.print("Comando: ");
    Serial.print(comandoTracao == 1 ? "FRENTE" : comandoTracao == -1 ? "RÉ" : "PARAR");
    Serial.print(" | Vel: ");
    Serial.print(velocidadeAtual);
    Serial.print(" | Dir: ");
    Serial.print(direcaoAtual);
    Serial.print(" | Motores: ");
    Serial.print(velEsq);
    Serial.print(",");
    Serial.println(velDir);
}
  
  // Sua função existente - mantida exatamente como fornecida
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

void girarServo(int angulo) {
    angulo = constrain(0, 180);

    if (angulo < 90) {
        girarEsquerda(angulo);
    } else {
        girarDireita(angulo);
    }
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
  servoEsq.write(servoCentroEsq);
  servoDir.write(servoCentroDir);
}

// void ajustarDirecao(int angulo) {
//   // Angulo positivo = direita, negativo = esquerda
//   angulo = constrain(angulo, SERVO_ANGULO_MAX, SERVO_ANGULO_MAX);
  
//   servoEsq.write(servoCentroEsq + angulo);
//   servoDir.write(servoCentroDir + angulo);
// }

void girarDireita(int angulo) {
  servoEsq.write(servoCentroEsq + angulo);
  servoDir.write(servoCentroDir + angulo);
}

void girarEsquerda(int angulo) {
  servoEsq.write(servoCentroEsq - angulo);
  servoDir.write(servoCentroDir - angulo);

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

// =================== FUNÇÕES DE COMUNICAÇÃO ===================
void processarComandosRPi() {
  if (SerialRPi.available()) {
    String mensagem = SerialRPi.readStringUntil('\n');
    mensagem.trim();
    
    if(mensagem.startsWith("X") && mensagem.indexOf("Y") != -1) {
      int posY = mensagem.indexOf("Y");
      coneX = mensagem.substring(1, posY).toInt();
      coneY = mensagem.substring(posY+1).toInt();
      
      coneDetectado = true;
      tempoUltimoCone = millis();
      
      Serial.print("Cone X:");
      Serial.print(coneX);
      Serial.print(" Y:");
      Serial.println(coneY);
    }
  }
  
  if (coneDetectado && millis() - tempoUltimoCone > 1000) {
    coneDetectado = false;
  }
}

// =================== LÓGICA DOS MARCOS ===================
void executarMarcoReto() {
  static uint8_t etapa = 0;
  static unsigned long tempoInicio = 0;

  switch (etapa) {
    case 0:
      Serial.println("Marco 2: Iniciando corrida reta inicial...");
      mover(235, 255);
      tempoInicio = millis();
      etapa = 1;
      break;

    case 1:
    if ((millis() - tempoInicio >= 25000) && sistemaAtivo) {
      girarEsquerda(30);
      tempoInicio = millis();
      etapa = 2;
    }
      break;
    case 2:
    if ((millis() - tempoInicio >= 100) && sistemaAtivo) {
      mover(100, 100);
      tempoInicio = millis();
      etapa = 3;
    }
      break;
    case 3:
    if ((millis() - tempoInicio >= 800) && sistemaAtivo) {
      alinharServos();
      tempoInicio = millis();
      etapa = 4;
    }
      break;

    case 4:
    if ((millis() - tempoInicio >= 500) && sistemaAtivo) {
      mover(235, 255);
      tempoInicio = millis();
      etapa = 5;
    }
    break;
    case 5:
      if (((millis() - tempoInicio >= 10000) && (medirDistancia(46, 44)) < 20) && sistemaAtivo) {
        pararMotores();
        piscarLED(22, 4, 500);
        estadoAtual = MARCO_3;
      }
      break;
  }
}
void executarMarco2() {
  static uint8_t etapa = 0;
  static unsigned long tempoInicio = 0;

  switch (etapa) {
    case 0:
      verificarControleIR();
      delay(100);

      Serial.println("Marco 2: Iniciando corrida reta inicial...");
      mover(255, 255);
      tempoInicio = millis();
      etapa = 1;
      break;

    case 1:
      verificarControleIR();
      delay(100);

      if ((millis() - tempoInicio >= 8000) && sistemaAtivo) {
        pararMotores();
        tempoInicio = millis();
        etapa = 2;
      }
      break;

    case 2:
      verificarControleIR();
      delay(100);

      if ((millis() - tempoInicio >= 1000) && sistemaAtivo) {
        girarDireita(30);
        tempoInicio = millis();
        etapa = 3;
      }
      break;

    case 3:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 800) {
        mover(100, 100);
        tempoInicio = millis();
        etapa = 4;
      }
      break;

    case 4:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 1500) {
        pararMotores();
        tempoInicio = millis();
        etapa = 5;
      }
      break;

    case 5:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 500) {
        alinharServos();
        tempoInicio = millis();
        etapa = 6;
      }
      break;

    case 6:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 400) {
        mover(255, 255);
        tempoInicio = millis();
        etapa = 7;
      }
      break;

    case 7:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 12000) {
        girarEsquerda(20);
        tempoInicio = millis();
        etapa = 8;
      }
      break;

    case 8:
      if (millis() - tempoInicio >= 1000) {
        mover(100, 100);
        tempoInicio = millis();
        etapa = 9;
      }
      break;

    case 9:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 150) {
        alinharServos();
        mover(255, 255);
        tempoInicio = millis();
        etapa = 10;
      }
      break;

    case 10:
      verificarControleIR();
      delay(100);
      
      if ((millis() - tempoInicio) >= 25000 || medirDistancia(46, 44) <= 20) {
        pararMotores();
        piscarLED(22, 4, 500);
        estadoAtual = MARCO_3;
        etapa = 0;  // Reinicia etapas para o próximo marco
      }
      break;
  }
}

  // if (medirDistancia(46, 44) < 15) {
  //   pararMotores();
  //   piscarLED(22, 3, 500);
  //   coneDetectado = true;
  //   delay(2000);
  // }

  // if (coneDetectado) {
  //   pararMotores();
  //   delay(500);
  //   girarEsquerda(45);
  //   delay(500);
  //   mover(-100,-100);
  //   delay(3000);
  //   girarDireita(45);
  //   delay(500);
  //   alinharServos();
  //   delay(500);
  //   mover(255,255);
  //   delay(10000);

  //   pararMotores();
  //   delay(5000);
  // }
  // static uint8_t etapa = 0;
  // static unsigned long tempoInicio = 0;

  // switch (etapa) {
  //   case 0:
  //     Serial.println("Marco 2 - Etapa 0: Procurando cone...");
  //     tempoInicio = millis();
  //     etapa = 1;
  //     break;

  //   case 1:
  //     if (coneDetectado) {
  //       Serial.println("Cone detectado! Indo para alinhamento.");
  //       etapa = 2;
  //     } else if (millis() - tempoInicio > 10000) { // Timeout de 10s
  //       Serial.println("Timeout: cone não detectado.");
  //       pararMotores();
  //       estadoAtual = INICIO; // ou outro estado de erro
  //       etapa = 0;
  //     } else {
  //       mover(100, 100); // Anda pra frente procurando cone
  //     }
  //     break;

  //   case 2:
  //     // Alinhar com o cone baseado no X
  //     if (abs(coneX) > 30) { // margem de tolerância
  //       Serial.print("Alinhando com cone, coneX: ");
  //       Serial.println(coneX);
  //       if (coneX < 0) {
  //         girarEsquerdaLeve();
  //       } else {
  //         girarDireitaLeve();
  //       }
  //     } else {
  //       Serial.println("Alinhamento feito.");
  //       etapa = 3;
  //       tempoInicio = millis();
  //     }
  //     break;

  //   case 3:
  //     // Aproximação até o cone
  //     float distancia = medirDistancia(trigFrente, echoFrente);
  //     Serial.print("Distância ao cone: ");
  //     Serial.println(distancia);

  //     if (distancia < 30.0) {
  //       Serial.println("Cone próximo! Parando.");
  //       pararMotores();
  //       etapa = 4;
  //       tempoInicio = millis();
  //     } else {
  //       mover(60, 60); // Aproxima devagar
  //     }
  //     break;

  //   case 4:
  //     // Espera um pouco para garantir parada
  //     if (millis() - tempoInicio > 1000) {
  //       Serial.println("Marco 2 concluído.");
  //      pararMotores();
  //      delay(10000);
  //       etapa = 0;
  //     }
  //     break;
  // }


  
  // if (!giroCompleto) {
  //   // Gira 165° para a direita usando servos
  //   girarDireita(SERVO_ANGULO_MAX);
  //   mover(100, 100);
  //   delay(1650); // Ajuste este tempo para giro preciso
  //   giroCompleto = true;
  //   alinharServos();
  //   tempoInicioMovimento = millis();
  // }

  // // Alinhar com o cone
  // if (coneDetectado) {
  //   if (abs(coneX) > 30) {
  //     // Ainda não está alinhado → gira para alinhar
  //     if (coneX < 0) {
  //       // Cone à esquerda → girar levemente à esquerda
  //       girarEsquerdaLeve();
  //     } else {
  //       // Cone à direita → girar levemente à direita
  //       girarDireitaLeve();
  //     }
  //   } else if (coneY < 20) {
  //     // Alinhado e próximo → muda de marco
  //     piscarLED(LED_CONE_PIN, 3, 200);
  //     estadoAtual = MARCO_2;
  //     giroCompleto = false;
  //   } else {
  //     // Alinhado mas ainda distante → avança para o cone
  //     mover(120, 120);
  //   }
  // } else if (millis() - tempoInicioMovimento > 15000) {
  //   // Timeout de busca
  //   estadoAtual = INICIO;
  //   giroCompleto = false;
  // }


void executarMarco3() {
  static uint8_t etapa = 0;
  static unsigned long tempoInicio = 0;
  static unsigned long tempoEspera = 0;
  static bool obstaculoDetectado = false;

  switch (etapa) {
    case 0:
      girarEsquerda(45);
      tempoInicio = millis();
      etapa = 1;
      break;

    case 1:
      if (millis() - tempoInicio >= 100) {
        mover(-200, -200);
        tempoInicio = millis();
        etapa = 2;
      }
      break;

    case 2:
      if (millis() - tempoInicio >= 2000) {
        pararMotores();
        tempoInicio = millis();
        etapa = 3;
      }
      break;

    case 3:
      if (millis() - tempoInicio >= 100) {
        girarDireita(45);
        tempoInicio = millis();
        etapa = 4;
      }
      break;

    case 4:
      if (millis() - tempoInicio >= 2000) {
        mover(200, 200);
        tempoInicio = millis();
        etapa = 5;
      }
      break;

    case 5:
      if (millis() - tempoInicio >= 500) {
        alinharServos();
        tempoInicio = millis();
        etapa = 6;
      } break;

    case 6:
      if (millis() - tempoInicio >= 200) {
        mover(255, 255);
        tempoInicio = millis();
        etapa = 7;
      }
      break;

    case 7:
      verificarControleIR();
      delay(100);

      if ((millis() - tempoInicio >= 25000) && sistemaAtivo) {
        girarDireita(20);
        tempoInicio = millis();
        etapa = 8;
      }
      break;

    case 8:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 1000) {
        mover(100, 100);
        tempoInicio = millis();
        etapa = 9;
      } break;

    case 9:
    verificarControleIR();
      delay(100);

      if ((millis() - tempoInicio >= 500) && sistemaAtivo) {
        alinharServos();
        tempoInicio = millis();
        etapa = 10;
      }
      break;

    case 10:
      verificarControleIR();
      delay(100);

      if ((millis() - tempoInicio >= 500) && sistemaAtivo) {
        mover(255, 255);
        tempoInicio = millis();
        etapa = 11;
      }
      break;

    case 11:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 12000) {
        girarEsquerda(20);
        tempoInicio = millis();
        etapa = 12;
      } break;

    case 12:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 500) {
        alinharServos();
        tempoInicio = millis();
        etapa = 13;
      }
      break;

      case 13:
      verificarControleIR();
      delay(100);

      if (millis() - tempoInicio >= 500) {
        mover(255, 255);
        tempoInicio = millis();
        etapa = 14;
      }
      break;

    case 14:
      verificarControleIR();
      delay(100);
      
      if ((millis() - tempoInicio) >= 10000 || medirDistancia(46, 44) <= 15) {
        pararMotores();
        piscarLED(22, 4, 500);
        estadoAtual = MARCO_4;
        etapa = 0;  // Reinicia etapas para o próximo marco
      }
      break;
  }
}

void executarMarco4() {
  static uint8_t etapa = 0;
  static unsigned long tempoInicio = 0;

  switch (etapa) {
    case 0:
      girarEsquerda(20);
      mover(200, 200);
      tempoInicio = millis();
      etapa = 1;
      break;

    case 1:
      if (millis() - tempoInicio >= 4000) {
        alinharServos();
        tempoInicio = millis();
        etapa = 2;
      }
      break;

    case 2:
      if (millis() - tempoInicio >= 100) {
        mover(150, 150);
        tempoInicio = millis();
        etapa = 3;
      }
      break;

    case 3:
      if ((millis() - tempoInicio < 7000) && medirDistancia(46, 44) > 15) {
        // continua se não encontrou obstáculo
        break;
      } else {
        pararMotores();
        tempoInicio = millis();
        etapa = 4;
      }
      break;

    case 4:
      if (millis() - tempoInicio >= 500) {
        piscarLED(22, 4, 500);
        pararMotores();
        etapa = 0; // reinicia para permitir nova execução
        estadoAtual = FINAL; // se existir um marco seguinte
      }
      break;
  }
}


// void executarMarco4() {
//   // Navegação final
//   if (coneDetectado) {
//     if (coneY < 20 && abs(coneX) < 30) {
//       piscarLED(LED_CONE_PIN, 5, 200);
//       estadoAtual = FINAL;
//     } else {
//       navegarParaCone();
//     }
//   } else {
//     buscarCones();
//   }
// }

// =================== FUNÇÕES DE NAVEGAÇÃO ===================
void navegarParaCone() {
  if (!coneDetectado) {
    buscarCones();
    return;
  }

  // Controle proporcional para direção
  int anguloDirecao = map(coneX, 320, 320, SERVO_ANGULO_MAX, SERVO_ANGULO_MAX);
  // ajustarDirecao(anguloDirecao);
  
  // Velocidade base reduzida quando perto do cone
  int velocidade = constrain(coneY, 50, 150);
  mover(velocidade, velocidade);
}

void buscarCones() {
  static unsigned long ultimaBusca = 0;
  static bool direita = true;
  
  if (millis() - ultimaBusca > 2000) {
    if (direita) {
      girarDireita(SERVO_ANGULO_MAX);
    } else {
      girarEsquerda(SERVO_ANGULO_MAX);
    }
    direita = !direita;
    ultimaBusca = millis();
  }
  
  mover(80, 80);
}

// =================== CONTROLE POR IR ===================
void verificarControleIR() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00) { // *
      sistemaAtivo = true;
      estadoAtual = MARCO_2;
      piscarLED(LED_STATUS_PIN, 1, 500);
      Serial.println("Sistema ATIVADO - Iniciando Marco 1");
    }
    else if (IrReceiver.decodedIRData.decodedRawData == 0xF20DFF00) { // #
      sistemaAtivo = false;
      estadoAtual = INICIO;
      pararMotores();
      piscarLED(LED_STATUS_PIN, 2, 200);
      Serial.println("Sistema DESATIVADO");
    }
    IrReceiver.resume();
  }
}

// =================== FUNÇÕES AUXILIARES ===================
void piscarLED(int pin, int vezes, int intervalo) {
  for (int i = 0; i < vezes; i++) {
    digitalWrite(pin, HIGH);
    delay(intervalo);
    digitalWrite(pin, LOW);
    if (i < vezes - 1) delay(intervalo);
  }
}

void girarDireitaLeve() {
  girarDireita(10);  // vira o servo para 20 graus à direita
  mover(80, 80);   // anda para frente com ambos os motores
  delay(300);        // gira por um tempo curto
  pararMotores();    // para os motores
  alinharServos();   // volta os servos para a posição reta
}

void girarEsquerdaLeve() {
  girarEsquerda(10); // vira o servo para 20 graus à esquerda
  mover(80, 80);   // anda para frente
  delay(300);        // gira por um tempo curto
  pararMotores();    // para
  alinharServos();   // centraliza o servo novamente
}

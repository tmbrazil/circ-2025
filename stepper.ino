#define DIR_PIN 2
#define STEP_PIN 3

// #define CH1_PIN 7
// #define CH2_PIN 8
#define CH3_PIN 13
// #define CH5_PIN 12

// int CH1;
// int CH2;
int CH3;
// int CH5;

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);  

  digitalWrite(DIR_PIN, HIGH);
}

void loop() {
  if (CH3 > 60) {
    digitalWrite(DIR_PIN, HIGH);
    digitalWrite(STEP_PIN, LOW);
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(CH3);
  } else {
    //trás
    digitalWrite(DIR_PIN, LOW);
    digitalWrite(STEP_PIN, LOW);
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(CH3);
  }

  delay(50);
}

void RC_READ(void) {
  // CH1 = pulseIn(CH1_PIN, HIGH);
  // CH1 = constrain(map(CH1, 994, 1988, 60, 120), 60, 120);
  // if (abs(90 - CH1) <= 3) {
  //   CH1 = 90;
  // }

  // CH2 = pulseIn(CH2_PIN, HIGH);
  // CH2 = constrain(map(CH2, 987, 1973, -255, 255), -255, 255);
  // if (abs(CH2) < 30) {
  //   CH2 = 0;
  // }

  CH3 = pulseIn(CH3_PIN, HIGH);
  CH3 = constrain(map(CH3, 987, 1973, 60, 200), 60, 200);
  if (abs(CH3) < 100) {
    CH3 = 0;
  }

  // CH5 = (pulseIn(CH5_PIN, HIGH) - 1491);
}

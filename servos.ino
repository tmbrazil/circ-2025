// #include <Servo.h>
#define CH3_PIN 8
// Servo servo;

int CH3;

void setup() {
  Serial.begin(9600);
  pinMode(7, OUTPUT);

  // servo.attach(7);
  // servo.write(90);
}

void loop() {
  RC_READ();
  // Serial.println(CH3);

  // servo.write(CH3);

  delay(50);
}

void RC_READ(void) {
  CH3 = pulseIn(CH3_PIN, HIGH);
  CH3 = constrain(map(CH3, 995, 1973, 40, 120), 40, 120);
  if (abs(CH3) < 20) {
    CH3 = 90;
  }
  
}

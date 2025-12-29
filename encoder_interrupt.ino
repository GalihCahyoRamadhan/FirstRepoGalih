#define encoder1_a 36
#define encoder1_b 34
#define encoder2_a 39
#define encoder2_b 35

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile long pulsa1 = 0;
volatile long pulsa2 = 0;

void ARDUINO_ISR_ATTR onEncoder1() {
  portENTER_CRITICAL_ISR(&timerMux);
  int A = digitalRead(encoder1_a);
  int B = digitalRead(encoder1_b);
  if (A == B) {
    pulsa1--;
  } else {
    pulsa1++;
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

void ARDUINO_ISR_ATTR onEncoder2() {
  portENTER_CRITICAL_ISR(&timerMux);
  int A = digitalRead(encoder2_a);
  int B = digitalRead(encoder2_b);
  if (A == B) {
    pulsa2--;
  } else {
    pulsa2++;
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);
  
  // Note: Using INPUT instead of INPUT_PULLUP because pins 36, 39 are input-only
  // on ESP32 and don't have internal pull-ups. External pull-ups required.
  pinMode(encoder1_a, INPUT);
  pinMode(encoder1_b, INPUT);
  pinMode(encoder2_a, INPUT);
  pinMode(encoder2_b, INPUT);
  
  // Attach interrupts on CHANGE for 4x resolution (both edges)
  attachInterrupt(digitalPinToInterrupt(encoder1_a), onEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder1_b), onEncoder1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder2_a), onEncoder2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder2_b), onEncoder2, CHANGE);
  
  Serial.println("Encoder interrupt system initialized");
}

void loop() {
  portENTER_CRITICAL(&timerMux);
  long pulsa1_copy = pulsa1;
  long pulsa2_copy = pulsa2;
  portEXIT_CRITICAL(&timerMux);
  
  Serial.print("PULSA1: ");
  Serial.print(pulsa1_copy);
  Serial.print(" | PULSA2: ");
  Serial.println(pulsa2_copy);
  
  delay(100);
}

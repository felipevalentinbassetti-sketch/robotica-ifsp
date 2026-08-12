#include <QTRSensors.h>

#define NUM_SENSORS 8
#define EMITTER_PIN 13
#define LED 2
#define NUM_LAPS 2

QTRSensors qtr;
uint16_t sensorValues[NUM_SENSORS];

// Motores
const int ENA = 19;
const int ENB = 16;
const int IN1 = 4;
const int IN2 = 17;

// Velocidade
int vD, vE;
int vMax = 255;
int baseSpeed = 200;

// PID
float kp = 0.6;
float ki = 0.0005;
float kd = 8.0;

float erro = 0;
float lastError = 0;
float integral = 0;

// Tempo
unsigned long lastTime = 0;

// Linha
int line, presentLine, lastLine = 0;
int lap = 0;
bool chegada = false;

void setup() {
  pinMode(LED, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){26,25,33,32,35,34,39,36}, NUM_SENSORS);
  qtr.setEmitterPin(EMITTER_PIN);

  Serial.begin(115200);

  // Calibração
  digitalWrite(LED, HIGH);
  for (int i = 0; i < 400; i++) qtr.calibrate();
  digitalWrite(LED, LOW);

  delay(1000);

  lastTime = millis();
}

void loop() {

  uint16_t position = qtr.readLineWhite(sensorValues);

  // =========================
  // DETECÇÃO DE LINHA DE CHEGADA
  // =========================
  line = (sensorValues[0] < 700 && sensorValues[7] < 700) ? 1 : 0;

  if (line != lastLine) lastTime = millis();

  if ((millis() - lastTime) > 50) {
    if (line != presentLine) {
      presentLine = line;
      if (presentLine == 1) lap++;

      if (lap == NUM_LAPS) chegada = true;
    }
  }

  // =========================
  // PID COM TEMPO REAL
  // =========================
  float dt = (millis() - lastTime) / 1000.0;
  if (dt <= 0) dt = 0.01;

  erro = ((float)position - 3500) / 10.0;

  // Filtro de erro (suavização)
  erro = 0.7 * erro + 0.3 * lastError;

  // Integral com anti-windup
  integral += erro * dt;
  if (integral > 100) integral = 100;
  if (integral < -100) integral = -100;

  float deriv = (erro - lastError) / dt;

  lastError = erro;

  // =========================
  // FUZZY SIMPLES (ADAPTATIVO)
  // =========================
  float absErro = abs(erro);

  if (absErro < 10) {
    kp = 0.4; kd = 5;
  } else if (absErro < 50) {
    kp = 0.7; kd = 10;
  } else {
    kp = 1.2; kd = 15;
  }

  float correction = kp * erro + ki * integral + kd * deriv;

  // =========================
  // VELOCIDADE DINÂMICA
  // =========================
  int dynamicBase = baseSpeed - absErro * 0.8;
  if (dynamicBase < 120) dynamicBase = 120;

  vD = dynamicBase + correction;
  vE = dynamicBase - correction;

  // Limites
  vD = constrain(vD, 0, vMax);
  vE = constrain(vE, 0, vMax);

  // PWM mínimo (evita motor travar)
  if (vD > 0 && vD < 70) vD = 70;
  if (vE > 0 && vE < 70) vE = 70;

  // =========================
  // PERDA DE LINHA
  // =========================
  if (position < 100 || position > 6900) {
    if (lastError > 0) {
      vD = 200;
      vE = 0;
    } else {
      vD = 0;
      vE = 200;
    }
  }

  // =========================
  // CHEGADA
  // =========================
  if (chegada) {
    vD = 0;
    vE = 0;
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    Serial.println("CHEGADA");
    while (1);
  }

  lastLine = line;

  // =========================
  // SAÍDA MOTORES
  // =========================
  analogWrite(ENA, vD);
  analogWrite(ENB, vE);
}

#include <QTRSensors.h>
#include <Fuzzy.h>

/* ============================
  QTR CONFIG
   ============================ */
#define NUM_SENSORS 8
#define TIMEOUT 2500
#define EMITTER_PIN 2

QTRSensorsAnalog qtr(
  (unsigned char[]){A0, A1, A2, A3, A4, A5, A6, A7},
  NUM_SENSORS,
  TIMEOUT,
  EMITTER_PIN
);

unsigned int sensorValues[NUM_SENSORS];

/* ============================
  MOTORES
   ============================ */
int motorEsqPWM = 5;
int motorDirPWM = 6;
int baseSpeed = 120;

/* ============================
  PID
   ============================ */
float Kp = 0.5;
float Kd = 0.1;
int erroAnterior = 0;

/* ============================
  FUZZY
   ============================ */
Fuzzy *fuzzy = new Fuzzy();

/* --- Entradas Fuzzy --- */
// Erro (QTR retorna 0 a 7000 → centro ≈ 3500)
FuzzySet *erroNG = new FuzzySet(-3500, -3500, -2000, -800);
FuzzySet *erroNP = new FuzzySet(-1200, -600, -300, 0);
FuzzySet *erroZ  = new FuzzySet(-200, 0, 0, 200);
FuzzySet *erroPP = new FuzzySet(0, 300, 600, 1200);
FuzzySet *erroPG = new FuzzySet(800, 2000, 3500, 3500);

// Delta erro
FuzzySet *dNeg  = new FuzzySet(-2000, -2000, -500, 0);
FuzzySet *dZero = new FuzzySet(-100, 0, 0, 100);
FuzzySet *dPos  = new FuzzySet(0, 500, 2000, 2000);

/* --- Saídas Fuzzy --- */
// Kp
FuzzySet *kpBaixo = new FuzzySet(0.2, 0.3, 0.4, 0.5);
FuzzySet *kpMedio = new FuzzySet(0.4, 0.6, 0.6, 0.8);
FuzzySet *kpAlto  = new FuzzySet(0.7, 1.0, 1.3, 1.3);

// Kd
FuzzySet *kdBaixo = new FuzzySet(0.0, 0.05, 0.1, 0.15);
FuzzySet *kdMedio = new FuzzySet(0.1, 0.2, 0.2, 0.3);
FuzzySet *kdAlto  = new FuzzySet(0.25, 0.4, 0.6, 0.6);

/* ============================
  FUNÇÕES
   ============================ */
void setMotores(int esq, int dir) {
  esq = constrain(esq, 0, 255);
  dir = constrain(dir, 0, 255);
  analogWrite(motorEsqPWM, esq);
  analogWrite(motorDirPWM, dir);
}

/* ============================
  SETUP
   ============================ */
void setup() {
  Serial.begin(9600);
  pinMode(motorEsqPWM, OUTPUT);
  pinMode(motorDirPWM, OUTPUT);

  /* Calibração QTR */
  for (int i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(5);
  }

  /* Fuzzy Inputs */
  FuzzyInput *erroInput = new FuzzyInput(1);
  erroInput->addFuzzySet(erroNG);
  erroInput->addFuzzySet(erroNP);
  erroInput->addFuzzySet(erroZ);
  erroInput->addFuzzySet(erroPP);
  erroInput->addFuzzySet(erroPG);
  fuzzy->addFuzzyInput(erroInput);

  FuzzyInput *deltaInput = new FuzzyInput(2);
  deltaInput->addFuzzySet(dNeg);
  deltaInput->addFuzzySet(dZero);
  deltaInput->addFuzzySet(dPos);
  fuzzy->addFuzzyInput(deltaInput);

  /* Fuzzy Outputs */
  FuzzyOutput *kpOut = new FuzzyOutput(1);
  kpOut->addFuzzySet(kpBaixo);
  kpOut->addFuzzySet(kpMedio);
  kpOut->addFuzzySet(kpAlto);
  fuzzy->addFuzzyOutput(kpOut);

  FuzzyOutput *kdOut = new FuzzyOutput(2);
  kdOut->addFuzzySet(kdBaixo);
  kdOut->addFuzzySet(kdMedio);
  kdOut->addFuzzySet(kdAlto);
  fuzzy->addFuzzyOutput(kdOut);

  /* Regras */
  FuzzyRuleAntecedent *erroGrande = new FuzzyRuleAntecedent();
  erroGrande->joinSingle(erroPG);

  FuzzyRuleConsequent *kpAltoKdBaixo = new FuzzyRuleConsequent();
  kpAltoKdBaixo->addOutput(kpAlto);
  kpAltoKdBaixo->addOutput(kdBaixo);

  fuzzy->addFuzzyRule(new FuzzyRule(1, erroGrande, kpAltoKdBaixo));

  FuzzyRuleAntecedent *erroCentro = new FuzzyRuleAntecedent();
  erroCentro->joinSingle(erroZ);

  FuzzyRuleConsequent *kpMedioKdMedio = new FuzzyRuleConsequent();
  kpMedioKdMedio->addOutput(kpMedio);
  kpMedioKdMedio->addOutput(kdMedio);

  fuzzy->addFuzzyRule(new FuzzyRule(2, erroCentro, kpMedioKdMedio));
}

/* ============================
  LOOP
   ============================ */
void loop() {
  int position = qtr.readLineBlack(sensorValues);
  int erro = position - 3500;

  int deltaErro = erro - erroAnterior;
  erroAnterior = erro;

  fuzzy->setInput(1, erro);
  fuzzy->setInput(2, deltaErro);
  fuzzy->fuzzify();

  Kp = fuzzy->defuzzify(1);
  Kd = fuzzy->defuzzify(2);

  float correcao = Kp * erro + Kd * deltaErro;

  setMotores(baseSpeed + correcao, baseSpeed - correcao);

  Serial.print("Erro: ");
  Serial.print(erro);
  Serial.print(" | Kp: ");
  Serial.print(Kp);
  Serial.print(" | Kd: ");
  Serial.println(Kd);

  delay(5);
}

#include <Fuzzy.h>
#include <QTRSensors.h>

#define num_sensors 8
#define emiterPin 13
#define led 2

QTRSensors qtr;
uint16_t sensorvalues[num_sensors];

// Motores
const int PINO_ENA = 19; 
const int PINO_ENB = 16;
const int PINO_IN1 = 4;  
const int PINO_IN2 = 17;

int vD, vE;
int vMax = 255;

// ==========================
// FUZZY
// ==========================
Fuzzy *fuzzy = new Fuzzy();

// Entrada (posição 0–7000)
FuzzySet *esq_forte = new FuzzySet(0, 0, 800, 1500);
FuzzySet *esq = new FuzzySet(1200, 2000, 2000, 3000);
FuzzySet *centro = new FuzzySet(2800, 3500, 3500, 4200);
FuzzySet *dir = new FuzzySet(4000, 5000, 5000, 6000);
FuzzySet *dir_forte = new FuzzySet(5500, 6500, 7000, 7000);

// Saída motor esquerdo
FuzzySet *lentoE = new FuzzySet(0, 50, 50, 100);
FuzzySet *medioE = new FuzzySet(100, 150, 150, 200);
FuzzySet *rapidoE = new FuzzySet(200, 230, 255, 255);

// Saída motor direito
FuzzySet *lentoD = new FuzzySet(0, 50, 50, 100);
FuzzySet *medioD = new FuzzySet(100, 150, 150, 200);
FuzzySet *rapidoD = new FuzzySet(200, 230, 255, 255);

void setup() {

  Serial.begin(9600);

  pinMode(led, OUTPUT);
  pinMode(PINO_ENA, OUTPUT); 
  pinMode(PINO_ENB, OUTPUT);
  pinMode(PINO_IN1, OUTPUT);
  pinMode(PINO_IN2, OUTPUT);

  digitalWrite(PINO_IN1, LOW); 
  digitalWrite(PINO_IN2, LOW);

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){26,25,33,32,35,34,39,36}, num_sensors);
  qtr.setEmitterPin(emiterPin);

  // Calibração
  digitalWrite(led,HIGH);
  for (int i=0;i<400;i++) qtr.calibrate();
  digitalWrite(led,LOW);  
  delay(2000);

  // ==========================
  // CONFIG FUZZY
  // ==========================
  FuzzyInput *pos = new FuzzyInput(1);
  pos->addFuzzySet(esq_forte);
  pos->addFuzzySet(esq);
  pos->addFuzzySet(centro);
  pos->addFuzzySet(dir);
  pos->addFuzzySet(dir_forte);
  fuzzy->addFuzzyInput(pos);

  FuzzyOutput *motorE = new FuzzyOutput(1);
  motorE->addFuzzySet(lentoE);
  motorE->addFuzzySet(medioE);
  motorE->addFuzzySet(rapidoE);
  fuzzy->addFuzzyOutput(motorE);

  FuzzyOutput *motorD = new FuzzyOutput(2);
  motorD->addFuzzySet(lentoD);
  motorD->addFuzzySet(medioD);
  motorD->addFuzzySet(rapidoD);
  fuzzy->addFuzzyOutput(motorD);

  // ==========================
  // REGRAS FUZZY
  // ==========================

  // ESQUERDA FORTE
  {
    FuzzyRuleAntecedent *ifEsqForte = new FuzzyRuleAntecedent();
    ifEsqForte->joinSingle(esq_forte);

    FuzzyRuleConsequent *then = new FuzzyRuleConsequent();
    then->addOutput(lentoE);
    then->addOutput(rapidoD);

    fuzzy->addFuzzyRule(new FuzzyRule(1, ifEsqForte, then));
  }

  // ESQUERDA
  {
    FuzzyRuleAntecedent *ifEsq = new FuzzyRuleAntecedent();
    ifEsq->joinSingle(esq);

    FuzzyRuleConsequent *then = new FuzzyRuleConsequent();
    then->addOutput(medioE);
    then->addOutput(rapidoD);

    fuzzy->addFuzzyRule(new FuzzyRule(2, ifEsq, then));
  }

  // RETA
  {
    FuzzyRuleAntecedent *ifCentro = new FuzzyRuleAntecedent();
    ifCentro->joinSingle(centro);

    FuzzyRuleConsequent *then = new FuzzyRuleConsequent();
    then->addOutput(rapidoE);
    then->addOutput(rapidoD);

    fuzzy->addFuzzyRule(new FuzzyRule(3, ifCentro, then));
  }

  // DIREITA
  {
    FuzzyRuleAntecedent *ifDir = new FuzzyRuleAntecedent();
    ifDir->joinSingle(dir);

    FuzzyRuleConsequent *then = new FuzzyRuleConsequent();
    then->addOutput(rapidoE);
    then->addOutput(medioD);

    fuzzy->addFuzzyRule(new FuzzyRule(4, ifDir, then));
  }

  // DIREITA FORTE
  {
    FuzzyRuleAntecedent *ifDirForte = new FuzzyRuleAntecedent();
    ifDirForte->joinSingle(dir_forte);

    FuzzyRuleConsequent *then = new FuzzyRuleConsequent();
    then->addOutput(rapidoE);
    then->addOutput(lentoD);

    fuzzy->addFuzzyRule(new FuzzyRule(5, ifDirForte, then));
  }
}

void loop() {

  uint16_t position = qtr.readLineWhite(sensorvalues);

  fuzzy->setInput(1, position);
  fuzzy->fuzzify();

  vE = fuzzy->defuzzify(1);
  vD = fuzzy->defuzzify(2);

  vE = constrain(vE, 0, vMax);
  vD = constrain(vD, 0, vMax);

  analogWrite(PINO_ENA, vD);
  analogWrite(PINO_ENB, vE);

  Serial.print("Pos: ");
  Serial.print(position);
  Serial.print(" | vE: ");
  Serial.print(vE);
  Serial.print(" | vD: ");
  Serial.println(vD);
}

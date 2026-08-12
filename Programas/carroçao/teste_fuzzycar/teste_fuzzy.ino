#include <Fuzzy.h>
#include <QTRSensors.h>

#define num_sensors 8
#define emiterPin 13
#define led 2

QTRSensors qtr;
uint16_t sensorvalues[num_sensors];

// motores
const int PINO_ENA = 19; 
const int PINO_ENB = 16;
const int PINO_IN1 = 4;  
const int PINO_IN2 = 17;

int vD, vE;
int vMax = 255;

// ========================
// FUZZY
// ========================
Fuzzy *fuzzy = new Fuzzy();

// Entrada (posição da linha)
FuzzySet *esq_forte = new FuzzySet(0, 0, 500, 1500);
FuzzySet *esq = new FuzzySet(1000, 2000, 2000, 3000);
FuzzySet *centro = new FuzzySet(2500, 3500, 3500, 4500);
FuzzySet *dir = new FuzzySet(4000, 5000, 5000, 6000);
FuzzySet *dir_forte = new FuzzySet(5500, 6500, 7000, 7000);

// Saída motor esquerdo
FuzzySet *lento = new FuzzySet(0, 50, 50, 100);
FuzzySet *medio = new FuzzySet(100, 150, 150, 200);
FuzzySet *rapido = new FuzzySet(180, 220, 255, 255);

// Saída motor direito
FuzzySet *lento_d = new FuzzySet(0, 50, 50, 100);
FuzzySet *medio_d = new FuzzySet(100, 150, 150, 200);
FuzzySet *rapido_d = new FuzzySet(180, 220, 255, 255);

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

  // calibração
  digitalWrite(led,HIGH);
  for (int i=0;i<400;i++) qtr.calibrate();
  digitalWrite(led,LOW);  
  delay(2000);

  // ========================
  // FUZZY SETUP
  // ========================
  FuzzyInput *pos = new FuzzyInput(1);
  pos->addFuzzySet(esq_forte);
  pos->addFuzzySet(esq);
  pos->addFuzzySet(centro);
  pos->addFuzzySet(dir);
  pos->addFuzzySet(dir_forte);
  fuzzy->addFuzzyInput(pos);

  FuzzyOutput *motorE = new FuzzyOutput(1);
  motorE->addFuzzySet(lento);
  motorE->addFuzzySet(medio);
  motorE->addFuzzySet(rapido);
  fuzzy->addFuzzyOutput(motorE);

  FuzzyOutput *motorD = new FuzzyOutput(2);
  motorD->addFuzzySet(lento_d);
  motorD->addFuzzySet(medio_d);
  motorD->addFuzzySet(rapido_d);
  fuzzy->addFuzzyOutput(motorD);

  // ========================
  // REGRAS (ESSENCIAL)
  // ========================

  // ESQUERDA FORTE
  fuzzy->addFuzzyRule(new FuzzyRule(1,
    new FuzzyRuleAntecedent()->joinSingle(esq_forte),
    new FuzzyRuleConsequent()->addOutput(lento)->addOutput(rapido_d)));

  // ESQUERDA
  fuzzy->addFuzzyRule(new FuzzyRule(2,
    new FuzzyRuleAntecedent()->joinSingle(esq),
    new FuzzyRuleConsequent()->addOutput(medio)->addOutput(rapido_d)));

  // RETA
  fuzzy->addFuzzyRule(new FuzzyRule(3,
    new FuzzyRuleAntecedent()->joinSingle(centro),
    new FuzzyRuleConsequent()->addOutput(rapido)->addOutput(rapido_d)));

  // DIREITA
  fuzzy->addFuzzyRule(new FuzzyRule(4,
    new FuzzyRuleAntecedent()->joinSingle(dir),
    new FuzzyRuleConsequent()->addOutput(rapido)->addOutput(medio_d)));

  // DIREITA FORTE
  fuzzy->addFuzzyRule(new FuzzyRule(5,
    new FuzzyRuleAntecedent()->joinSingle(dir_forte),
    new FuzzyRuleConsequent()->addOutput(rapido)->addOutput(lento_d)));
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

  Serial.print("Pos: "); Serial.print(position);
  Serial.print(" | vE: "); Serial.print(vE);
  Serial.print(" | vD: "); Serial.println(vD);
}

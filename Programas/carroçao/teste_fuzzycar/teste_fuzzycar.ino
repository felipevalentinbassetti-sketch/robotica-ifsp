#include <Fuzzy.h>
#include <QTRSensors.h>

#define num_sensors 8
//#define timeout 2500
#define emiterPin 13
#define led 2

QTRSensors qtr;
uint16_t sensorvalues[num_sensors];

// config para big
//declaracao dos pinos utilizados para controlar a velocidade de rotacao
const int PINO_ENA = 16; 
const int PINO_ENB = 17;

//declaracao dos pinos utilizados para controlar o sentido do motor
const int PINO_IN1 = 19; 
const int PINO_IN2 = 21;
const int PINO_IN3 = 22;
const int PINO_IN4 = 23;

/* config para small
//declaracao dos pinos utilizados para controlar a velocidade de rotacao
const int PINO_ENA = 19; 
const int PINO_ENB = 16;

//declaracao dos pinos utilizados para controlar o sentido do motor
const int PINO_IN1 = 4;  
const int PINO_IN2 = 17;
*/

//variaveis de velocidade
int vD, vE, vDmax=250, vEmax=250;

// For scope, instantiate all objects you will need to access in loop()
// It may be just one Fuzzy, but for demonstration, this sample will print
// all FuzzySet pertinence

// Fuzzy
Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput
FuzzySet *fechada_esq = new FuzzySet(0, 0, 1200, 2000);
FuzzySet *suave_esq = new FuzzySet(1600, 2200, 2200, 3350);
FuzzySet *reta = new FuzzySet(3300, 3500, 3500, 3700);
FuzzySet *suave_dir = new FuzzySet(3650, 4800, 4800, 5400);
FuzzySet *fechada_dir = new FuzzySet(5000, 5800, 7000, 7000);

// FuzzyOutput
FuzzySet *slow_esq = new FuzzySet(100, 100, 140, 160);
FuzzySet *normal_esq = new FuzzySet(140, 185, 185, 210);
FuzzySet *fast_esq = new FuzzySet(200, 220, 255, 255);

// FuzzyOutput
FuzzySet *slow_dir = new FuzzySet(100, 100, 140, 160);
FuzzySet *normal_dir = new FuzzySet(140, 185, 185, 210);
FuzzySet *fast_dir = new FuzzySet(200, 220, 255, 255);

void setup()
{
  // Set the Serial output
  Serial.begin(9600);
  pinMode(led, OUTPUT); 

  //configuração dos pinos como saida
  pinMode(PINO_ENA, OUTPUT); 
  pinMode(PINO_ENB, OUTPUT);
  pinMode(PINO_IN1, OUTPUT);
  pinMode(PINO_IN2, OUTPUT);
  pinMode(PINO_IN3, OUTPUT);
  pinMode(PINO_IN4, OUTPUT);

  //inicia o codigo com os motores parados
  digitalWrite(PINO_IN1, LOW); 
  digitalWrite(PINO_IN2, LOW);
  digitalWrite(PINO_IN3, LOW);
  digitalWrite(PINO_IN4, LOW);
  digitalWrite(PINO_ENA, LOW);
  digitalWrite(PINO_ENB, LOW);

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){26,25,33,32,35,34,39,36}, num_sensors);
  qtr.setEmitterPin(emiterPin);

  //config da serial para debug
  Serial.begin(9600);
  //calibration routine
  digitalWrite(led,HIGH);
  for (int i=0;i<400;i++) qtr.calibrate();
  digitalWrite(led,LOW);  
  delay(2000);

    //configura os motores para o sentido horario
  digitalWrite(PINO_IN1, HIGH); 
  digitalWrite(PINO_IN2, LOW);
  digitalWrite(PINO_IN3, HIGH);
  digitalWrite(PINO_IN4, LOW);

  // Every setup must occur in the function setup()

  // FuzzyInput
  FuzzyInput *curva = new FuzzyInput(1);

  curva->addFuzzySet(fechada_esq);
  curva->addFuzzySet(suave_esq);
  curva->addFuzzySet(reta);
  curva->addFuzzySet(suave_dir);
  curva->addFuzzySet(fechada_dir);
  fuzzy->addFuzzyInput(curva);

  // FuzzyOutput
  FuzzyOutput *speed_esq = new FuzzyOutput(1);

  speed_esq->addFuzzySet(slow_esq);
  speed_esq->addFuzzySet(normal_esq);
  speed_esq->addFuzzySet(fast_esq);
  fuzzy->addFuzzyOutput(speed_esq);

  // FuzzyOutput
  FuzzyOutput *speed_dir = new FuzzyOutput(2);

  speed_dir->addFuzzySet(slow_dir);
  speed_dir->addFuzzySet(normal_dir);
  speed_dir->addFuzzySet(fast_dir);
  fuzzy->addFuzzyOutput(speed_dir);

  // Building FuzzyRule "IF curva = reta THEN speed_esq = normal_esq E speed_dir = normal_dir"
  FuzzyRuleAntecedent *ifCurvaReta = new FuzzyRuleAntecedent();
  ifCurvaReta->joinSingle(reta);
  FuzzyRuleConsequent *thenSpeedNormal = new FuzzyRuleConsequent();
  thenSpeedNormal->addOutput(normal_esq);
  thenSpeedNormal->addOutput(normal_dir);
  FuzzyRule *fuzzyRule01 = new FuzzyRule(1, ifCurvaReta, thenSpeedNormal);
  fuzzy->addFuzzyRule(fuzzyRule01);

  FuzzyRuleAntecedent *ifCurvaSuaveDir = new FuzzyRuleAntecedent();
  ifCurvaSuaveDir->joinSingle(suave_dir);
  FuzzyRuleConsequent *thenCurvaSuaveDir = new FuzzyRuleConsequent();
  thenCurvaSuaveDir->addOutput(normal_dir);
  thenCurvaSuaveDir->addOutput(slow_esq);
  FuzzyRule *fuzzyRule02 = new FuzzyRule(1, ifCurvaSuaveDir, thenCurvaSuaveDir);
  fuzzy->addFuzzyRule(fuzzyRule02);

  FuzzyRuleAntecedent *ifCurvaSuaveEsq = new FuzzyRuleAntecedent();
  ifCurvaSuaveEsq->joinSingle(suave_esq);
  FuzzyRuleConsequent *thenCurvaSuaveEsq = new FuzzyRuleConsequent();
  thenCurvaSuaveEsq->addOutput(normal_esq);
  thenCurvaSuaveEsq->addOutput(slow_dir);
  FuzzyRule *fuzzyRule03 = new FuzzyRule(1, ifCurvaSuaveEsq, thenCurvaSuaveEsq);
  fuzzy->addFuzzyRule(fuzzyRule03);

  FuzzyRuleAntecedent *ifCurvaFechadaDir = new FuzzyRuleAntecedent();
  ifCurvaFechadaDir->joinSingle(fechada_dir);
  FuzzyRuleConsequent *thenCurvaFechadaDir = new FuzzyRuleConsequent();
  thenCurvaFechadaDir->addOutput(fast_dir);
  thenCurvaFechadaDir->addOutput(slow_esq);
  FuzzyRule *fuzzyRule04 = new FuzzyRule(1, ifCurvaFechadaDir, thenCurvaFechadaDir);
  fuzzy->addFuzzyRule(fuzzyRule04);

  FuzzyRuleAntecedent *ifCurvaFechadaEsq = new FuzzyRuleAntecedent();
  ifCurvaFechadaEsq->joinSingle(fechada_esq);
  FuzzyRuleConsequent *thenCurvaFechadaEsq = new FuzzyRuleConsequent();
  thenCurvaFechadaEsq->addOutput(fast_esq);
  thenCurvaFechadaEsq->addOutput(slow_dir);
  FuzzyRule *fuzzyRule05 = new FuzzyRule(1, ifCurvaFechadaEsq, thenCurvaFechadaEsq);
  fuzzy->addFuzzyRule(fuzzyRule05);
}

void loop()
{
  // get entrance
  unsigned int sensors[8];
  uint16_t position = qtr.readLineWhite(sensorvalues);

  if ((position<7000) && (position>0)){
  fuzzy->setInput(1, position);

  fuzzy->fuzzify();
  float output1 = fuzzy->defuzzify(1);
  float output2 = fuzzy->defuzzify(2);

  vE=output1;
  vD=output2;

  if(vD>vDmax) vD=vDmax;
  if(vE>vEmax) vE=vEmax;
  if(vD<0) vD=0;
  if(vE<0) vE=0;
  }
  Serial.print("position: ");
  Serial.print(position);
  Serial.print("||  vel esq: ");
  Serial.print(vE);
  Serial.print("||  vel dir: ");
  Serial.println(vD);

  //*atualiza velocidade dos motores
  analogWrite(PINO_ENA,vD);
  analogWrite(PINO_ENB,vE);
}
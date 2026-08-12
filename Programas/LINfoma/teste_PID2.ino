#include <QTRSensors.h>

#define num_sensors 8
#define emiterPin 13
#define led 2
#define numlin 2

QTRSensors qtr;
uint16_t sensorvalues[num_sensors];

// motores
const int PINO_ENA = 19; 
const int PINO_ENB = 16;
const int PINO_IN1 = 4;  
const int PINO_IN2 = 17;

// velocidade base
int startD = 150; 
int startE = 150;

// controle
int vD, vE, motorspeed;
int vDmax=250, vEmax=250;

// PID
float erro = 0;
float erroFiltrado = 0;
float lasterror = 0;
float integral = 0;

double kp = 0.6;
double ki = 0.0005;
double kd = 8.0;

// linha
int line, presentline, lastline=0;
int lap=0;
bool chegada=false;

// tempo
unsigned long lastPIDTime = 0;
unsigned long debounceTime = 0;

void setup() {
  pinMode(led, OUTPUT); 

  pinMode(PINO_ENA, OUTPUT); 
  pinMode(PINO_ENB, OUTPUT);
  pinMode(PINO_IN1, OUTPUT);
  pinMode(PINO_IN2, OUTPUT);

  digitalWrite(PINO_IN1, LOW); 
  digitalWrite(PINO_IN2, LOW);
  digitalWrite(PINO_ENA, LOW);
  digitalWrite(PINO_ENB, LOW);

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){26,25,33,32,35,34,39,36}, num_sensors);
  qtr.setEmitterPin(emiterPin);

  Serial.begin(115200);

  // calibração
  digitalWrite(led,HIGH);
  for (int i=0;i<400;i++) qtr.calibrate();
  digitalWrite(led,LOW);  
  delay(2000);

  lastPIDTime = millis();
}

// =========================
// LOOP
// =========================
void loop() {

  uint16_t position = qtr.readLineWhite(sensorvalues);

  // =========================
  // DETECÇÃO DE LINHA
  // =========================
  line = (sensorvalues[0]<700 && sensorvalues[7]<700) ? 1 : 0;

  if(line != lastline){
    debounceTime = millis();
  }

  if((millis()-debounceTime)>50){
    if(line != presentline){
      presentline = line;
      if(presentline == 1) lap++;
      if(lap==numlin) chegada=true;
    } 
  }

  // =========================
  // PID REFINADO
  // =========================
  unsigned long now = millis();
  float dt = (now - lastPIDTime) / 1000.0;
  if(dt <= 0) dt = 0.01;

  // erro centralizado
  erro = ((float)position - 3500) / 10.0;

  // filtro (suaviza leitura)
  erroFiltrado = 0.7 * erro + 0.3 * lasterror;

  // integral com limite
  integral += erroFiltrado * dt;
  if(integral > 100) integral = 100;
  if(integral < -100) integral = -100;

  // derivativo
  float deriv = (erroFiltrado - lasterror) / dt;

  // PID
  motorspeed = kp*erroFiltrado + ki*integral + kd*deriv;

  lasterror = erroFiltrado;
  lastPIDTime = now;

  // =========================
  // VELOCIDADE
  // =========================
  vD = startD + motorspeed;
  vE = startE - motorspeed;

  vD = constrain(vD, 0, vDmax);
  vE = constrain(vE, 0, vEmax);

  // evita motor morto
  if(vD > 0 && vD < 60) vD = 60;
  if(vE > 0 && vE < 60) vE = 60;

  // =========================
  // CHEGADA
  // =========================
  if(chegada){
    vD=0;
    vE=0;
    delay(100);
    Serial.println("chegada");
  }

  lastline=line;

  // =========================
  // SAÍDA
  // =========================
  analogWrite(PINO_ENA,vD);
  analogWrite(PINO_ENB,vE);
}

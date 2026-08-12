#include <QTRSensors.h>

#define num_sensors 8
#define emiterPin 13
#define led 2
#define numlin 2

QTRSensors qtr;
uint16_t sensorvalues[num_sensors];

// pinos motores
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
float lasterror = 0;
float integral = 0;
double kp = 0.6, ki = 0.0005, kd = 8.0;

int line, presentline, lastline=0;
int lap=0;
bool chegada=false;

// tempo separado (IMPORTANTE)
unsigned long lasttime = 0;

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

  Serial.begin(9600);

  digitalWrite(led,HIGH);
  for (int i=0;i<400;i++) qtr.calibrate();
  digitalWrite(led,LOW);  
  delay(2000);

  digitalWrite(PINO_IN1, LOW); 
  digitalWrite(PINO_IN2, LOW);

  lasttime = millis(); // inicia tempo
}

void loop() {

  uint16_t position = qtr.readLineWhite(sensorvalues);

  // =========================
  // LINHA DE CHEGADA
  // =========================
  if((sensorvalues[0]<700) && (sensorvalues[7]<700)){
    line=1;
  }else{
    line=0;
  } 

  if(line != lastline){
    lasttime=millis();
  }

  if((millis()-lasttime)>50){
    if(line != presentline){
      presentline = line;
      if(presentline == 1) lap++;
      if(lap==numlin){
        chegada=true;
      }
    } 
  }

  // =========================
  // PID MELHORADO
  // =========================
  unsigned long now = millis();
  float dt = (now - lasttime) / 1000.0;
  if(dt <= 0) dt = 0.01;

  float erro = ((float)position - 3500) / 10.0;

  // integral com anti-windup
  integral += erro * dt;
  if(integral > 100) integral = 100;
  if(integral < -100) integral = -100;

  // derivativo com tempo
  float deriv = (erro - lasterror) / dt;

  motorspeed = kp*erro + ki*integral + kd*deriv;

  lasterror = erro;
  lasttime = now;

  // =========================
  // VELOCIDADE
  // =========================
  vD = startD + motorspeed;
  vE = startE - motorspeed;

  if(vD>vDmax) vD=vDmax;
  if(vE>vEmax) vE=vEmax;
  if(vD<0) vD=0;
  if(vE<0) vE=0;

  if(chegada){
    vD=0;
    vE=0;
    delay(100);
    Serial.println("chegada");
  }

  lastline=line;

  analogWrite(PINO_ENA,vD);
  analogWrite(PINO_ENB,vE);
}

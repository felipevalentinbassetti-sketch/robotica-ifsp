#include <QTRSensors.h>

#define num_sensors 8
//#define timeout 2500
#define emiterPin 13
#define led 2
#define numlin 2

QTRSensors qtr;

//declaracao dos pinos utilizados para controlar a velocidade de rotacao
const int PINO_ENA = 16; 
const int PINO_ENB = 17;

//declaracao dos pinos utilizados para controlar o sentido do motor
const int PINO_IN1 = 19; 
const int PINO_IN2 = 21;
const int PINO_IN3 = 22;
const int PINO_IN4 = 23;

uint16_t sensorvalues[num_sensors];
//valores de referencia de velocidade carregados qdo movimento em linha reta
int startD = 190; 
int startE = 190;


//variaveis de velocidade passadas à biblioteca da shield
int vD, vE, motorspeed;
int vDmax=250, vEmax=250;
float lasterror=0;
double kp=.7, ki=0., kd=.1;

int line, presentline, lastline=0;
int lap=0;
bool chegada=false;
unsigned long lasttime = 0;

void setup() {
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

}

void loop() {
  unsigned int sensors[8];
  uint16_t position = qtr.readLineWhite(sensorvalues);
 // Rotina de deboucing da linha de chegada
  if((sensorvalues[0]<500) && (sensorvalues[7]<500)){
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

// Rotina de atualização do PID
  float erro = float(position)/10-350;
  float integ = integ + erro;
  float deriv=(erro-lasterror);
  lasterror=erro;
  float motorspeed = kp*erro + ki*integ + kd*deriv;

  vD=startD + int(motorspeed);
  vE=startE - int(motorspeed);

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

 //*atualiza velocidade dos motores
  analogWrite(PINO_ENA,vD);
  analogWrite(PINO_ENB,vE);
}
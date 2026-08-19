/*
conexão NRF24l01
CSN - d8
CE  - d9
MO  - d11
MI  - d12
SCK - d13

olhando lado solda
______________________
|gnd  mi  sck ce  vcc|
|gnd  irq mo  csn vcc|
|                    |
|                    |
|                    |
|                    |
|

joy mao direita: direita e esquerda
  direita: 1023
  repouso: 498
  esquerda: 0
joy mao esquerda: frente e re
  frente: 1023
  repouso : 517
  ré: 0
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>

RF24 radio(9,8); //CE,CSN

bool role=false; //RX role
bool radioNumber=0;

const byte endereco[6]="090657";

struct dadoscontrole{
  int y;
  int ync; //não utilizado
  int xnc; //não utilizado
  int x;
};

dadoscontrole dados;

//pinos controle motores
int ena=5;
int enb=6;
int in1=2;
int in2=3;
int in3=4;
int in4=7;
int va, vb, ve, vd, direcao;

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  
  // print example's introductory prompt
  Serial.println(F("RF24/examples/GettingStarted"));
  
  radio.setPALevel(RF24_PA_LOW);
  if(radio.isChipConnected()==1){
    Serial.println("modulo RF ok");
  }else Serial.println("modulo RF falhou");
  delay(2000);
  // set the TX address of the RX node for use on the TX pipe (pipe 0)
  radio.stopListening(endereco);  // put radio in TX mode

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(0, endereco); 
  
  // additional setup specific to the node's RX role
  if (!role) {
    radio.startListening();  // put radio in RX mode
  }
  
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(ena,OUTPUT);
  pinMode(enb,OUTPUT);
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  digitalWrite(ena,LOW);
  digitalWrite(enb,LOW);
}

void forward(){
     digitalWrite(in1,LOW);
      digitalWrite(in2,HIGH);
      digitalWrite(in3,HIGH);
      digitalWrite(in4,LOW);
}
void backward(){
      digitalWrite(in1,HIGH);
      digitalWrite(in2,LOW);
      digitalWrite(in3,LOW);
      digitalWrite(in4,HIGH);
}
void loop() {
  if(radio.available()){
    radio.read(&dados,sizeof(dadoscontrole));
    Serial.print(dados.x);
    Serial.print(" ");
    Serial.print(dados.y);
    Serial.println();




    if(dados.y<506){
      vd=map(dados.y,506,0,0,255);
      ve=map(dados.y,506,0,0,255);
      direcao=1; // frente
    }
    else if(dados.y>528){
      vd=map(dados.y,528,1023,0,255);
      ve=map(dados.y,528,1023,0,255);
      direcao=0; // ré
    }
    else{
      vd=0;
      ve=0;
    }

    // ajuste vel para movimento esquerda
    if(dados.x<490){
      int xmap=map(dados.x,490,0,0,255);
      vd=vd-xmap;
      ve=ve+xmap;
      if(vd>255) vd=255;
      if(ve<0)   ve=0;
    }

    // ajuste vel para movimento direita
    if(dados.x>509){
      int xmap=map(dados.x,509,1023,0,255);
      vd=vd+xmap;
      ve=ve-xmap;
      if(vd<0)   vd=0;
      if(ve>255) ve=255;
    }

  ve=ve/2;
  vd=vd/2;

/*

    if(dados.x1 <= 507) va=map(dados.x1,0,507,255,0);
    else if(dados.x1 > 507) va=map(dados.x1,507,1023,0,255);
    if(dados.y2 <= 520) vb=map(dados.y2,0,520,255,0);
    else if(dados.y2 > 520) vb=map(dados.y2,520,1023,0,255);
    
    if(dados.y2 >= 540) {
      digitalWrite(in1,LOW);
      digitalWrite(in2,HIGH);
      digitalWrite(in3,HIGH);
      digitalWrite(in4,LOW);
    }
    else if(dados.y2 < 500) {
      digitalWrite(in1,HIGH);
      digitalWrite(in2,LOW);
      digitalWrite(in3,LOW);
      digitalWrite(in4,HIGH);
    }

  if(dados.x1 < 500){
    vd=round((vb+va)/2);
    ve=round((vb+(255-va))/2);
  }else if(dados.x1 > 515){
    vd=round((vb+(255-va))/2);
    ve=round((vb+va)/2);
  }else{
    ve=vb;
    vd=vb;
  }
  
 */ if(direcao==0){
      backward();
    }
    else forward();
    
    analogWrite(ena,ve);
    analogWrite(enb,vd);
  }
}

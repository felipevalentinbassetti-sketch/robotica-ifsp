/*
joy mao direita: direita e esquerda
direita: 0
repouso: 507
esquerda: 1023
joy mao esquerda: frente e re
frente: 0
repouso : 520
ré: 1023
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>

RF24 radio(8,9);//9,8); //CE,CSN

const byte endereco[6]="090657";

struct dadoscontrole{
  int x1;
  int y1;
  int x2;
  int y2;
};

dadoscontrole dados;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(endereco);
  radio.setPALevel(RF24_PA_LOW);
  if(radio.isChipConnected()==1){
    Serial.println("modulo RF ok");
  }else Serial.println("modulo RF falhou");
  delay(2000);
  radio.stopListening();
}

void loop() {
  dados.x1 = analogRead(A0); //frente-tras
  dados.y2 = analogRead(A1); //direita-esquerda
//  dados.x1 = analogRead(A2);
//  dados.y2 = analogRead(A3);
  Serial.print(dados.x1);
  Serial.print(" ");
  Serial.print(dados.y2);
  Serial.println();
  radio.write(&dados,sizeof(dadoscontrole));
  delay(10);
}

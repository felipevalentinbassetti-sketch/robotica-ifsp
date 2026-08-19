/* Este Código foi desenvolvido pelo prof. Heiton do IFSP Sorocaba, a fim de ser utilizado pelos Alunos que 
 *  participarão do TRIF. A venda deste código é PROÍBIDA!
 *  
 *  Robôs de Sumô e robôs seguidores de linha, da categoria perseguidores, precisam iniciar e parar através
 *  do acionamento remoto, e os Juizes utilizam um controle IR da SONY com os seguintes botões:
 *    controle SONY
 *    botão 1 - READY  codigo 0x0
 *    botão 2 - READY  codigo 0x1
 *    botão 3 - READY  codigo 0x2
 *    
 *  Normalmente se utiliza um dispositivo chamado de MicroStart da Jsumo, porém ele é caro e precisa ser importado
 *  MicroStart Jsumo pode ser encontrado aqui : https://www.jsumo.com/microstart-sumo-minisumo-robot-start-module
 *  porém é possivel utilizar um módulo receptor IR simples e 2 leds para para fazer seu próprio MicroStart:
 *  modulo receptor IR recomendado : https://www.wjcomponentes.com.br/receptor-ir
 *  
 *  Duvidas podem ser enviadas para o e-mail: heiton.gomes@ifsp.edu.br
 */

#include <IRremote.hpp> // include the library

#define IR_RECEIVE_PIN   7  // pino em que vc conectou o receptor IR
#define LED_PIN_1 A3         // R pino para um led (indica ready - vermelho)
#define LED_PIN_2 A4         // G pino para um segundo led (indica Start - verde)
#define LED_PIN_3 A5         // B pino para um terceiro led (indica stop - azul)

const int trigPin = 13;  // Trigger pin
const int echoPin = 6;  // Echo pin
long distance;
int i=0;
long total, med;
int readings [10];
const int inimigo=50;

//The Arduino Nano provides 6 PWM-capable digital pins: D3, D5, D6, D9, D10, and D11.
int ind=12;//3;  // pino direção do motor direito
int veld=10; //pino velocidade do motor direito
int ine=11;  // pino direção do motor esquerdo
int vele=9; //pino velocidade do motor esquerdo

int sd=2;   // sensor IR direita (branco=1, preto=0)
int se=3;//12;   // sensor IR esquerda (branco=1, preto=0)

//variaveis de para intervalo de tempo sem delay
const long interval=5; // tempo de giro sobre o eixo
unsigned long previousMillis = 0;
int State = LOW;
const long faixa=10; // tempo de giro sobre o eixo
unsigned long antMillis = 0;

int terezo = 0; // variavel de controle (em homenagem ao prof. heiton)
volatile int estado = 0;

void setup() {
  Serial.begin(9600);
  // aqui estão as declarações de pino para que o receptor e os leds funcionem adequadamente, não se esqueça de acrecentar as suas declarações
  pinMode(ind, OUTPUT);
  pinMode(ine, OUTPUT);
  pinMode(veld,OUTPUT);
  pinMode(vele,OUTPUT);
  pinMode(sd,INPUT_PULLUP);
  pinMode(se,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(se), meuISRe, RISING);
  attachInterrupt(digitalPinToInterrupt(sd), meuISRd, RISING);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  while(1){  // aqui é onde o código espera o comandos do controle IR.
   if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.command == 0x0) { // espera o comando do botão Ready
      terezo=1;
      digitalWrite(LED_PIN_1,HIGH);
      digitalWrite(LED_PIN_2,LOW);
      Serial.print("ready");
    } 
    else if (IrReceiver.decodedIRData.command == 0x1 && terezo==1) {  // espera o comando do botão Start
          terezo=2;
          digitalWrite(LED_PIN_1,LOW);
          digitalWrite(LED_PIN_2,HIGH);
          Serial.print("hunting");
          break;
          }
          else if (IrReceiver.decodedIRData.command == 0x2) { // espera o comando do botão Stop
                digitalWrite(LED_PIN_1,LOW);
                digitalWrite(LED_PIN_2,HIGH);
                digitalWrite(LED_PIN_3,HIGH);
                Serial.print("end");
                }
    IrReceiver.resume();    
    }
}
IrReceiver.resume(); 
}

void le_stop(){  // caso o botão STOP seja precionado, o robô precisa parar a qualquer momento! este void cuida disso
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.command == 0x3) {
      terezo=3;
      //coloque aqui a rotina para parar seus motores
      para();
      digitalWrite(LED_PIN_1, LOW);
      digitalWrite(LED_PIN_2, LOW);
      digitalWrite(LED_PIN_3, HIGH);
      Serial.print("reset please");
      while(1);
    }
  }
  IrReceiver.resume(); 
}

void meuISRd() {
  estado = 1; // Inverte o estado
}
void meuISRe() {
  estado = 2; // Inverte o estado
}
void trasdir(){
  digitalWrite(ind,HIGH);
  analogWrite(veld,120);
  digitalWrite(ine,HIGH);
  analogWrite(vele,180);
}
void trasesq(){
  digitalWrite(ind,HIGH);
  analogWrite(veld,180);
  digitalWrite(ine,HIGH);
  analogWrite(vele,120);
}
void tras(){
  digitalWrite(ind,HIGH);
  analogWrite(veld,100);
  digitalWrite(ine,HIGH);
  analogWrite(vele,100); 
}
void frente(){
  digitalWrite(ind,LOW);
  analogWrite(veld,200);
  digitalWrite(ine,LOW);
  analogWrite(vele,200);
}
void para(){
  digitalWrite(ind,LOW);
  digitalWrite(veld,LOW);
  digitalWrite(ine,LOW);
  digitalWrite(vele,LOW);
}
void eixo(){
  digitalWrite(ind,HIGH);
  analogWrite(veld,180);
  digitalWrite(ine,LOW);
  analogWrite(vele,100); 
}

void loop() {
  le_stop(); // chama a rotina que para o robô quando o comando STOP é pressionado
/////////////////////////////////////////////////////////////////// 
// coloque aqui seu código de acionamento dos motores e estratégias
///////////////////////////////////////////////////////////////////
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);
  med = duration * 0.034 / 2;
  if(med<70) distance=med;
/*
if(med<150){
  total = total - readings[i];
  readings[i] = med;
  total = total + readings[i];
  }
  i = i + 1;
  if (i >= 10) {
    i = 0;
  }
  // calculate the average:
  distance = total / 10;
*/
  //Serial.println(distance);
  Serial.println(distance);

// se localizar o inimigo, o robo avança
  if(abs(distance) < inimigo){
    frente();
  }
  else if(abs(distance) > inimigo){ // rotina de busca do inimigo
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      if (State == LOW) {
        State = HIGH;
        eixo();
      } else {
        State = LOW;
        para();
      }
    }
  }
  if(estado==1||estado==2){
    //Serial.println("bera1");
    para();
    delay(100);
    tras();
    delay(1500);
    para();
    if(estado==1) trasesq();
    if(estado==2) trasdir();

    delay(1000);
    tras();
    estado=0;
  }
// se um dos sensores IR identificar faixa branca, o robo para
 /*if((digitalRead(sd)==1)||(digitalRead(se)==1)){
  Serial.println("bera1");
  para();
  delay(100);
  tras();
  delay(500);
  para();
  eixo();
  delay(100);
  para();
 }
 if((digitalRead(sd)==1)&&(digitalRead(se)==0)){
  Serial.println("bera2");
  para();
  delay(100);
  trasdir();
  delay(500);
  para();
 }
 if((digitalRead(sd)==0)&&(digitalRead(se)==1)){
  Serial.println("bera3");
  para();
  delay(100);
  trasesq();
  delay(500);
  para();
 }*/
}

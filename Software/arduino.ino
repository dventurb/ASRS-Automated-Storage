/* 
* Atenção: Num sistema de coordenadas tridimensional, por norma, o eixo vertical é nomeado de Eixo Z
* Neste caso, o eixo Vertical é o eixo Y, e o eixo responsável por acionar e remover materiais é o Eixo Z.
*/

// BIBLIOTECAS
#include <AccelStepper.h>

// CONSTANTES
#define MAX 9
#define IO 2
#define ENABLE_PIN 8
#define SWITCH_X 53
#define SWITCH_Z 52
#define SWITCH_Y 51
#define VETOINHA 24

// STRUCTS
typedef struct{
  int x, y;
}ST_POSICAO;

// ARRAYS
const ST_POSICAO compartimentos[MAX] = {
  {0, -1600},          // Passos necessários para cada compartimento
  {500, -1600},
  {1000, -1600},
  {0, -4000},
  {500, -4000},
  {1000, -4000},
  {0, -6500},
  {500, -6500},
  {1000, -6500},
};

const ST_POSICAO estacoes[IO] = {
  {1800, -4000},   // Estação Saída
  {1800, -1600}    // Estação Entrada
};

// VARIÁVEIS GLOBAIS
int n;
int potXValor, potYValor, potZValor;
float temperatura;
volatile char ch;
bool MotorXRun = false;
bool MotorYRun = false;
bool MotorZRun = false;
bool modoFifo = false;

// FUNÇÕES
void adicionarMaterial(const ST_POSICAO compartimentos[], const ST_POSICAO estacoes[], int n);
void removerMaterial(const ST_POSICAO compartimentos[], const ST_POSICAO estacoes[], int n);
void PosicaoInicial(void);
void pegarPeca(void);
void pousarPeca(void);
void verificarTemperatura(void);
void menu(void);

// CONFIGURAÇÕES DOS MOTORES
AccelStepper MotorX (1, 2, 5);
AccelStepper MotorY (1, 3, 6);
AccelStepper MotorZ (1, 4, 7);

void setup() {
  Serial.begin(115200);
  
 // COMUNICAÇÃO COM ESP32
  Serial1.begin(9600);
  
 // PARAMETROS DOS MOTORES
  MotorX.setMaxSpeed(2500);
  MotorX.setAcceleration(1000);
  MotorY.setMaxSpeed(1000);
  MotorY.setAcceleration(100);
  MotorZ.setMaxSpeed(2500);
  MotorZ.setAcceleration(200);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);

 // LIMIT SWITCH
  pinMode(SWITCH_X, INPUT_PULLUP);
  pinMode(SWITCH_Y, INPUT_PULLUP);
  pinMode(SWITCH_Z, INPUT_PULLUP);
  
  // VETOINHA
  pinMode(VETOINHA, OUTPUT);

  PosicaoInicial();
  menu();
}

void loop() {
  if (modoFifo){
    if (Serial1.available() > 0){
      ch = Serial1.read();
      if(ch >= 'A' && ch <= 'I'){
        n = ch - 'A';
        adicionarMaterial(compartimentos, estacoes, n);
        }else if(ch >= 'J' && ch <= 'R'){
          n = ch - 'J';
          removerMaterial(compartimentos, estacoes, n);
        }
    }
  }else if (!modoFifo){
    potXValor = analogRead(A9);
    potYValor = analogRead(A10);
    potZValor = analogRead(A11);
    int motorXPassos = map(potXValor, 0, 1023, -1800, 1800);   // Valor do Potenciômetro numa faixa entre os passos necessários para percorrer o eixo X total. 
    int motorYPassos = map(potYValor, 0, 1023, -6500, 6500);
    int motorZPassos = map(potZValor, 0, 1023, -600, 600);
    MotorX.move(motorXPassos);
    MotorX.run();
    MotorY.move(motorYPassos);
    MotorY.run();
    MotorZ.move(motorZPassos);
    MotorZ.run();
  }
  verificarTemperatura();
}

// Movimentar os Motores Até aos LimitSwitchs
void PosicaoInicial(void){
    while(digitalRead(SWITCH_Z)){
    MotorZ.moveTo(-600);
    MotorZ.run();
  }
  while(digitalRead(SWITCH_X)){
    MotorX.move(-500);
    MotorX.run();
  }
  while(digitalRead(SWITCH_Y)){
    MotorY.move(1000);
    MotorY.run();
  }
  MotorX.setCurrentPosition(0);
  MotorY.setCurrentPosition(0);
  MotorZ.setCurrentPosition(0);
}

// Aguardar até o Utilizador Selecione o Modo de Operação
void menu(void){
  do {
    if (Serial1.available() > 0){
      ch = Serial1.read();
      if (ch == '1'){
        modoFifo = true;
      }else if (ch == '2'){
        modoFifo = false;
      }
    }
  }while(ch != '1' && ch != '2');
}

// Movimentar Motores da Estação de Entrada até ao Compartimento Indicado 
void adicionarMaterial(const ST_POSICAO compartimentos[], const ST_POSICAO estacoes[], int n){
  MotorX.moveTo(estacoes[1].x);
  while(MotorX.distanceToGo() != 0){
  	MotorX.run();
  }
  MotorY.moveTo(estacoes[1].y);
  while(MotorY.distanceToGo() != 0){
  	MotorY.run();
  }
  pegarPeca();
  ST_POSICAO destino = compartimentos[n];
  int passosX = destino.x - estacoes[1].x;
  int passosY = destino.y - estacoes[1].y;
  
  MotorX.moveTo(MotorX.currentPosition() + passosX);
  while(MotorX.distanceToGo() != 0){
  	MotorX.run();
  }
  MotorY.moveTo(MotorY.currentPosition() + passosY);
  while(MotorY.distanceToGo() != 0){
  	MotorY.run();
  }
  pousarPeca();
  PosicaoInicial();
}

// Movimentar Motores até ao Compartimento Indicado até a Estação de Saída
void removerMaterial(const ST_POSICAO compartimentos[], const ST_POSICAO estacoes[], int n){
  MotorX.moveTo(compartimentos[n].x);
  while(MotorX.distanceToGo() != 0){
  	MotorX.run();
  }
  MotorY.moveTo(compartimentos[n].y);
  while(MotorY.distanceToGo() != 0){
  	MotorY.run();
  }
  pegarPeca();
  ST_POSICAO destino = compartimentos[n];
  int passosX = estacoes[0].x - destino.x;
  int passosY = estacoes[0].y - destino.y;
                                                       
  
  MotorX.moveTo(MotorX.currentPosition() + passosX);
  while(MotorX.distanceToGo() != 0){
  	MotorX.run();
  }
  MotorY.moveTo(MotorY.currentPosition() + passosY);
  while(MotorY.distanceToGo() != 0){
  	MotorY.run();
  }
  pousarPeca();
  PosicaoInicial();
}

// Movimento para Pegar o Material
void pegarPeca(void){
   MotorZ.moveTo(600);
   while(MotorZ.distanceToGo() != 0) {
    MotorZ.run();
  }
   MotorY.moveTo(-300);
   while(MotorY.distanceToGo() != 0) {
    MotorY.run();
    if(digitalRead(SWITCH_Y) == LOW){
     MotorY.stop();
     break;
    }
  }
  MotorZ.moveTo(-600);
  while(MotorZ.distanceToGo() != 0){
    MotorZ.run();
  }
}

// Movimento para Pousar o Material
void pousarPeca(void){
    MotorZ.moveTo(600);
    while(MotorZ.distanceToGo() != 0){
      MotorZ.run();
    }
    MotorY.moveTo(600);
    while(MotorY.distanceToGo() != 0){
      MotorY.run();  
    }
    MotorZ.moveTo(-600);
    while(MotorZ.distanceToGo() != 0){
      MotorZ.run();
    }
}

// Verificação da Temperatura do LM35
void verificarTemperatura(void){
     temperatura = (float(analogRead(A8))* 5/(1023.0)) / 0.01;
     if (temperatura >= 30){
     	digitalWrite(VETOINHA, HIGH);
     }else if (temperatura <= 15){
     	digitalWrite(VETOINHA, LOW);
     }
     delay(1000);
}

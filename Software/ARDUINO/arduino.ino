// BIBLIOTECAS
#include <AccelStepper.h>

// CONSTANTES
#define MAX 9
#define IO 2
#define SWITCH_X 53
#define SWITCH_Z 52
#define SWITCH_Y 51
#define EMERGENCY_PIN 26
#define VETOINHA 24

// STRUCTS
typedef struct{
  int x, y;
}ST_POSICAO;

// ARRAYS
const ST_POSICAO compartimentos[MAX] = {
  {0, -1600},
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
  {1800, -4000},   // Saída
  {1800, -1600}    // Entrada
};

// VARIÁVEIS GLOBAIS
int n;
float temperatura;
char receivedChar;
bool MotorXRun = false;
bool MotorYRun = false;
bool MotorZRun = false;
volatile bool btnEmergencia = false;

// FUNÇÕES
void adicionarMaterial(const ST_POSICAO compartimentos[], const ST_POSICAO estacoes[], int n);
void removerMaterial(const ST_POSICAO compartimentos[], const ST_POSICAO estacoes[], int n);
void PosicaoInicial(void);
void pegarPeca(void);
void pousarPeca(void);
void interrupEmergencia(void);
void verificarTemperatura(void);

// CONFIGURAÇÕES DOS MOTORES
AccelStepper MotorX (1, 2, 5);
AccelStepper MotorY (1, 3, 6);
AccelStepper MotorZ (1, 4, 7);
int enPin = 8;

void setup() {
  Serial.begin(115200);
  
 // COMUNICAÇÃO COM ESP32
  Serial1.begin(9600);
  
 // PARAMETROS DOS MOTORES
  MotorX.setMaxSpeed(2000);
  MotorX.setAcceleration(1000);
  MotorY.setMaxSpeed(1000);
  MotorY.setAcceleration(100);
  MotorZ.setMaxSpeed(500);
  MotorZ.setAcceleration(100);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  
  // BOTÃO DE EMERGENCIA
  pinMode(EMERGENCY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EMERGENCY_PIN), interrupEmergencia, FALLING);

 // LIMIT SWITCH
  pinMode(SWITCH_X, INPUT_PULLUP);
  pinMode(SWITCH_Y, INPUT_PULLUP);
  pinMode(SWITCH_Z, INPUT_PULLUP);
  
  // VETOINHA
  pinMode(VETOINHA, OUTPUT);

  PosicaoInicial();
}


void loop() {
    if(Serial1.available() > 0) {
      receivedChar = Serial1.read();
      if(receivedChar >= '1' && receivedChar <= '9'){
        n = (receivedChar - '0') - 1;
        adicionarMaterial(compartimentos, estacoes, n);
      }else if(receivedChar >= 'A' && receivedChar <= 'I'){
       switch(receivedChar){ 
        case 'A': n = 0; break;
        case 'B': n = 1; break;
        case 'C': n = 2; break;
        case 'D': n = 3; break;
        case 'E': n = 4; break;
        case 'F': n = 5; break;
        case 'G': n = 6; break;
        case 'H': n = 7; break;
        case 'I': n = 8; break;
        default: break;
       }
       removerMaterial(compartimentos, estacoes, n);
      }
  }
  if (btnEmergencia) {
  	btnEmergencia = false;
  	PosicaoInicial();
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

// Movimentar Motores da Estação de Entrada até ao Compartimento Indicado 
void adicionarMaterial(const ST_POSICAO compartimentos[], const ST_POSICAO estacoes[], int n){
  MotorX.moveTo(estacoes[1].x);
  while(MotorX.isRunning()){
  	MotorX.run();
  }
  MotorY.moveTo(estacoes[1].y);
  while(MotorY.isRunning()){
  	MotorY.run();
  }
  pegarPeca();
  ST_POSICAO destino = compartimentos[n];
  int passosX = destino.x - estacoes[1].x;
  int passosY = destino.y - estacoes[1].y;
  
  MotorX.moveTo(MotorX.currentPosition() + passosX);
  while(MotorX.isRunning()){
  	MotorX.run();
  }
  MotorY.moveTo(MotorY.currentPosition() + passosY);
  while(MotorY.isRunning()){
  	MotorY.run();
  }
  pousarPeca();
  PosicaoInicial();
}

// Movimentar Motores até ao Compartimento Indicado até a Estação de Saída
void removerMaterial(const ST_POSICAO compartimentos[], const ST_POSICAO estacoes[], int n){
  MotorX.moveTo(compartimentos[n].x);
  while(MotorX.isRunning()){
  	MotorX.run();
  }
  MotorY.moveTo(compartimentos[n].y);
  while(MotorY.isRunning()){
  	MotorY.run();
  }
  pegarPeca();
  ST_POSICAO destino = compartimentos[n];
  int passosX = estacoes[0].x - destino.x;
  int passosY = estacoes[0].y - destino.y;
                                                       
  
  MotorX.moveTo(MotorX.currentPosition() + passosX);
  while(MotorX.isRunning()){
  	MotorX.run();
  }
  MotorY.moveTo(MotorY.currentPosition() + passosY);
  while(MotorY.isRunning()){
  	MotorY.run();
  }
  pousarPeca();
  PosicaoInicial();
}

// Movimento para Pegar o Material
void pegarPeca(void){
   MotorZ.move(550);
   while(MotorZ.distanceToGo() != 0) {
    MotorZ.run();
  }
   MotorY.move(-300);
   while(MotorY.distanceToGo() != 0) {
    MotorY.run();
    if(digitalRead(SWITCH_Y) == LOW){
     MotorY.stop();
     break;
    }
  }
  MotorZ.move(-550);
  while(MotorZ.distanceToGo() != 0){
    MotorZ.run();
  }
}

// Movimento para Pousar o Material
void pousarPeca(void){
    MotorZ.move(550);
    while(MotorZ.distanceToGo() != 0){
      MotorZ.run();
    }
    MotorY.move(600);
    while(MotorY.distanceToGo() != 0){
      MotorY.run();  
    }
    MotorZ.move(550);
    while(MotorZ.distanceToGo() != 0){
      MotorZ.run();
    }
}

// Verificação da Temperatura do LM35
void verificarTemperatura(void){
     temperatura = (float(analogRead(A8))* 5/(1023.0)) / 0.01;
     if (temperatura >= 25){
     	digitalWrite(VETOINHA, HIGH);
     }else if (temperatura <= 15){
     	digitalWrite(VETOINHA, LOW);
     }
     delay(100);
}

void interrupEmergencia() {
  btnEmergencia = true;
}


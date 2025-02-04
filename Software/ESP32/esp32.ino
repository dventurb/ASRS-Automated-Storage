// BIBLIOTECAS
#include <SPI.h>
#include "Wire.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "webserver.h"
#include <AsyncTCP.h>
#include <Arduino.h>
#include "HX711.h"
#include <MFRC522.h>
#include "LiquidCrystal_PCF8574.h"
#include <HTTPClient.h>
#include <EEPROM.h>

// CONSTANTES
#define MAX 9
#define IO 2
#define MAX_STRING 100
#define BTN_ENQUEUE 16
#define BTN_DEQUEUE 4
#define RFID_CS 25
#define LOADCELL_DT 2 
#define LOADCELL_SCK 15
#define BUZZER_PIN 13
#define LCD_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLUMS 16
#define BACKLIGHT 90
#define EMERGENCY_PIN 12
#define EEPROM_SIZE 512

// DEFINÍÇÃO DA FILA
int tail = 0, head = 0, contador = 0;   // Variáveis da FILA 
ST_FUNCIONARIO funcionarios[1] = {{"João Rui", {0x1B, 0xD8, 0xAC, 0x6F}}};
unsigned int peso = 0;
ST_COMPARTIMENTO compartimentos[MAX] = {
  {false, 0, NULL},
  {false, 0, NULL},
  {false, 0, NULL},
  {false, 0, NULL},
  {false, 0, NULL},
  {false, 0, NULL},
  {false, 0, NULL},
  {false, 0, NULL},
  {false, 0, NULL}
};

// VARIÁVEIS GLOBAIS
const char *ssid = "ProjetoFinal";
const char *password = "danielventura";
volatile bool btnEnqueuePressionado = false;
volatile bool btnDequeuePressionado = false;
volatile bool acessoAutorizado = false;
volatile bool btnEmergencia = false;

// FUNÇÕES
void enqueue(void);
void dequeue(void);
void adiconarMaterial(int tail);
void removerMaterial(int head);
void iniciarBotoes(void);
void welcome(void);
void menu(void);
bool checkID(void);
void saveEEPROM(void);
void carregarEEPROM(void);
void IRAM_ATTR enqueueISR(void);
void IRAM_ATTR dequeueISR(void);
void IRAM_ATTR emergenciaISR(void);

// CONFIGURAÇÕES
AsyncWebServer server(80);
HX711 scale;
MFRC522 rfid(RFID_CS, 34);
LiquidCrystal_PCF8574 lcdI2C(LCD_ADDRESS);

void setup(void) {
  Serial.begin(115200);
  
  // ACESS POINT
  WiFi.softAP(ssid, password);
  iniciarWebServer();

// COMUNICAÇÃO COM ARDUINO
  Serial2.begin(9600, SERIAL_8N1, -1, 17); // RX não é utilizado

// EEPROM
  EEPROM.begin(512);
  carregarEEPROM();

// LOADCELL
  scale.begin(LOADCELL_DT, LOADCELL_SCK);
  scale.set_scale(390); // Valor da Calibração
  scale.tare();  // Zerar a Balança
  delay(1000);

// BUZZER
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

// BOTÕES 
  pinMode(BTN_ENQUEUE, INPUT_PULLUP);
  pinMode(BTN_DEQUEUE, INPUT_PULLUP);
  pinMode(EMERGENCY_PIN, INPUT_PULLUP);

// LCD 
  Wire.begin(26, 27);
  lcdI2C.begin(LCD_COLUMS, LCD_ROWS);
  lcdI2C.setBacklight(BACKLIGHT);
  welcome();

// INTERRUPÇÕES
  attachInterrupt(BTN_ENQUEUE, enqueueISR, RISING);
  attachInterrupt(BTN_DEQUEUE, dequeueISR, RISING);
  attachInterrupt(EMERGENCY_PIN, emergenciaISR, FALLING);
  
// RFID
  SPI.begin(33, 35, 32, RFID_CS); // Inicia Comunicação SPI
  pinMode(RFID_CS, OUTPUT);
  digitalWrite(RFID_CS, HIGH); // Desativa o Chip Select
  delay(1000); 
  rfid.PCD_Init();
  
  menu();
}

void loop(void) {
  if (btnEmergencia) {
    lcdI2C.clear();
    lcdI2C.setCursor(0, 0);
    lcdI2C.print("Emergencia detectada!");
    while (digitalRead(EMERGENCY_PIN) == LOW){
    	delay(100);
    }
    btnEmergencia = false;
    lcdI2C.clear();
  }
  if (!acessoAutorizado) {
    lcdI2C.clear();
    lcdI2C.setCursor(0, 0);
    lcdI2C.print("Passe o cartão.");
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
      if(checkID()){
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);
        digitalWrite(BUZZER_PIN, LOW);
        lcdI2C.clear();
        lcdI2C.setCursor(0, 0);
        lcdI2C.print(funcionarios[0].nome);
        acessoAutorizado = true;
      } else {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(1000);
        digitalWrite(BUZZER_PIN, LOW);
        lcdI2C.clear();
        lcdI2C.setCursor(0, 0);
        lcdI2C.print("Cartão Não Autorizado.");
      }
      rfid.PICC_HaltA();
    }
    delay(500);
  }else {
    iniciarBotoes();
  }
}

// FIFO - Inserir Compartimento
void enqueue(void){
  if(contador != MAX){
    peso = scale.get_units(5);
    lcdI2C.setCursor(0, 1);
    lcdI2C.print(peso);
    compartimentos[tail].estado = true;
    compartimentos[tail].peso = peso;
    compartimentos[tail].funcionario = &funcionarios[0];
    adicionarMaterial(tail);
    tail++;
    contador++;
    if (tail == MAX){
      tail = 0;
    }
    saveEEPROM();
  }else {
    lcdI2C.clear();
    lcdI2C.setCursor(0, 0);
    lcdI2C.print("Armazem cheio.");
    delay(500); 	
  }
  acessoAutorizado = false;
}

// FIFO - Remover Compartimento
void dequeue(void){
  if(contador != 0){
    compartimentos[head].estado = false;
    removerMaterial(head);
    head++;
    contador--;
    if (head == MAX){
      head = 0;
    }
    saveEEPROM();
  }else {
    lcdI2C.clear();
    lcdI2C.setCursor(0, 0);
    lcdI2C.print("Armazem Vazio.");
    delay(500); 
  }
  acessoAutorizado = false;
}

// Enviar o Compartimento a Adicionar
void adicionarMaterial(int tail){
  switch(tail){
    case 0: Serial2.write('1'); break;
    case 1: Serial2.write('2'); break;
    case 2: Serial2.write('3'); break;
    case 3: Serial2.write('4'); break;
    case 4: Serial2.write('5'); break;
    case 5: Serial2.write('6'); break;
    case 6: Serial2.write('7'); break;
    case 7: Serial2.write('8'); break;
    case 8: Serial2.write('9'); break;
    default: break;
  }
}

// Enviar o Compartimento a Remover
void removerMaterial(int head){
  switch(head){
    case 0: Serial2.write('A'); break;
    case 1: Serial2.write('B'); break;
    case 2: Serial2.write('C'); break;
    case 3: Serial2.write('D'); break;
    case 4: Serial2.write('E'); break;
    case 5: Serial2.write('F'); break;
    case 6: Serial2.write('G'); break;
    case 7: Serial2.write('H'); break;
    case 8: Serial2.write('I'); break;
    default: break;
  }
}

// Verificar os Botões
void iniciarBotoes(void){
    if(btnEnqueuePressionado){ 
      btnEnqueuePressionado = false;    // Reset Flag
      enqueue();
    }

    if(btnDequeuePressionado){
      btnDequeuePressionado = false;    // Reset Flag
      dequeue();
    }
    delay(50);
}

// Verificar o RFID Card 
bool checkID(void) {
  if (rfid.uid.size != sizeof(funcionarios[0].ID)) {
    return false;
  }
  for (byte i = 0; i < sizeof(funcionarios[0].ID); i++) {
    if (rfid.uid.uidByte[i] != funcionarios[0].ID[i]) {
      return false;
    }
  }
  return true;
}

void welcome(void){
  lcdI2C.clear();
  lcdI2C.print("");
  lcdI2C.print("    Bem-Vindo    ");
  char string[] = "Sistema de Armazenamento";
  int contador = strlen(string);
  for (int i = 0; i <= contador - 16; i++){
    lcdI2C.setCursor(0, 1);
    lcdI2C.print(string + i);
    delay(500);
  }
  delay(1000);
}

void menu(void){
  lcdI2C.clear();
  lcdI2C.setCursor(0, 0);
  lcdI2C.print("   [1] - FIFO");
  lcdI2C.setCursor(0, 1);
  lcdI2C.print("  [2] - Manual");
  delay(500);
}

void saveEEPROM(void){
    int addr = 0;
    EEPROM.put(addr, contador);
    addr += 4;
    EEPROM.put(addr, head);
    addr += 4;
    EEPROM.put(addr, tail);
    addr += 4;
    for (int i = 0; i < MAX; i++){
      EEPROM.write(addr++, compartimentos[i].estado);   // 1 BYTE
      EEPROM.put(addr, compartimentos[i].peso);
      addr += 4;
    }
    EEPROM.commit();
}

void carregarEEPROM(void){
    int addr = 0;
    EEPROM.get(addr, contador);
    addr += 4;
    EEPROM.get(addr, head);
    addr += 4;
    EEPROM.get(addr, tail);
    addr += 4;
    for (int i = 0; i < MAX; i++){
      compartimentos[i].estado = EEPROM.read(addr++);
      EEPROM.get(addr, compartimentos[i].peso);
      addr += 4;
    }
}

void IRAM_ATTR enqueueISR(void){
   btnEnqueuePressionado = true;   // Flag
}

void IRAM_ATTR dequeueISR(void){
  btnDequeuePressionado = true;    // Flag
}

void IRAM_ATTR emergenciaISR(void){
  btnEmergencia = true;
}


/* Nota: As funções de enviar os dados para o Google Sheets através do Google Drive estão como definidas como comentários no final deste código.
*  No entanto, estas funções foram implementadas com sucesso em uma parte separada do projeto.
*  As funções permitem a integração com o Google Sheets para armazenar os dados do Peso, Data e Hora, RFID Card e Compartimento.
*  Caso o utilizador queira utilizar essa funcionalidade, será necessário remover a declaração dos comentários das funções.
*  Sendo ainda necessário a inserção dos próprios dados de SSID, Password e URL do Google Sheets.
*/

// BIBLIOTECAS
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <HX711.h>
#include <MFRC522.h>
#include <Keypad.h>
// #include <WiFi.h>
// #include <HTTPClient.h>

// CONSTANTES
#define MAX 9   // Total de Compartimentos
#define IO 2    // Estação de Entrada e Saída
#define BTN_ENQUEUE 16
#define BTN_DEQUEUE 4
#define RFID_CS 25
#define LOADCELL_DT 2 
#define LOADCELL_SCK 15
#define BUZZER_PIN 13
#define BTNEMERG 12

// VARIÁVEIS GLOBAIS
int compartimentos[MAX], tail = 0, head = 0, contador = 0;   // Array e Variáveis para o Algoritmo First In - First Out
unsigned char funcionarioID[4] = {0x1B, 0xD8, 0xAC, 0x6F};
int peso;
volatile bool btnEnqueuePressionado = false;
volatile bool btnDequeuePressionado = false;
volatile bool acessoAutorizado = false;
volatile bool modoFifo = false;  

// FUNÇÕES
void enqueue();
void dequeue();
void adiconarMaterial(int tail);
void removerMaterial(int head);
void verificarRFIDCard(void)
bool checkID(void);
void enqueueISR(void);   // Rotina de Interrupções Botão 1
void dequeueISR(void);   // Rotina de Interrupções Botão 2
void menu(void);
void welcome(void);
void enviarPeso(void);
void verificarBotaoEmergencia(void);

// CONFIGURAÇÕES
HX711 scale;
MFRC522 rfid(RFID_CS, 34);
Wire.begin(26, 27);
LiquidCrystal_I2C lcdI2C(0x27, 16, 2);
char keys[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[4] = {23, 22, 1, 3};
byte colPins[3] = {21, 19, 18};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);

void setup() {
  Serial.begin(115200);

// COMUNICAÇÃO COM ARDUINO
  Serial2.begin(9600, SERIAL_8N1, -1, 17); // RX não é utilizado
  
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
  
// BOTÃO DE EMERGÊNCIA
  pinMode(BTNEMERG, INPUT);
  
// INTERRUPÇÕES
  attachInterrupt(BTN_ENQUEUE, enqueueISR, FALLING);
  attachInterrupt(BTN_DEQUEUE, dequeueISR, FALLING);
  
// RFID
  SPI.begin(33, 35, 32, RFID_CS); // Inicia Comunicação SPI
  pinMode(RFID_CS, OUTPUT);
  digitalWrite(RFID_CS, HIGH); // Desativa o Chip Select
  delay(1000); 
  rfid.PCD_Init();
  
  welcome();
  menu();
  verificarRFIDCard();
}

void loop() {
  if (!acessoAutorizado) {
    verificarRFIDCard();
  }
  verificarBotaoEmergencia();
}

// FIFO - Inserir Compartimento
void enqueue(){
  if(contador != MAX){
    peso = scale.get_units(10);
    enviarPeso(peso)
    // dataHoraAtual(&dataHoraAno);
    // stringJsonData(&jsonData);
    // enviarSheets(jsonData);
    adicionarMaterial(tail);
    tail++;
    contador++;
    if (tail == MAX){
      tail = 0;
    }
  }
  return;
}

// FIFO - Remover Compartimento
void dequeue(){
  if(contador != 0){
    removerMaterial(head);
    head++;
    contador--;
    if (head == MAX){
      head = 0;
    }
  }
  return;
}

// Enviar o Compartimento a Adicionar
void adicionarMaterial(int tail){
  switch(tail){
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

// Enviar o Compartimento a Remover
void removerMaterial(int head){
  switch(head){
    case 0: Serial2.write('J'); break;
    case 1: Serial2.write('K'); break;
    case 2: Serial2.write('L'); break;
    case 3: Serial2.write('M'); break;
    case 4: Serial2.write('N'); break;
    case 5: Serial2.write('O'); break;
    case 6: Serial2.write('P'); break;
    case 7: Serial2.write('Q'); break;
    case 8: Serial2.write('R'); break;
    default: break;
  }
}

// Aguardar até o Utilizador Selecionar o Modo de Operação
void menu(void){
  char key;
  do { 
    lcdI2C.clear();
    lcdI2C.setCursor(0, 0);
    lcdI2C.print("   [1] - FIFO");
    lcdI2C.setCursor(0, 1);
    lcdI2C.print("  [2] - Manual");
    key = keypad.getKey();
    if (key == '1') {
      modoFifo = true;
    }else if (key == '2'){
      modoFifo = false;
    }
  }while(key != '1' && key != '2');
}

// Verificação do cartão RFID 
void verificarRFIDCard(void){
  while (!acessoAutorizado) { 
    lcdI2C.clear();
    lcdI2C.setCursor(0, 0);
    lcdI2C.print("Cartão RFID...");
    while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
      delay(100);
    }
    if (checkID()) {
      lcdI2C.clear();
      lcdI2C.setCursor(0, 0);
      lcdI2C.print("Acesso Autorizado");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      acessoAutorizado = true;
      delay(1000);
    }else {
      lcdI2C.clear();
      lcdI2C.setCursor(0, 0);
      lcdI2C.print("Acesso Negado");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(1200);
      digitalWrite(BUZZER_PIN, LOW);
      acessoAutorizado = false;
      rfid.PICC_HaltA();
      delay(1000);
    }
  }
}

// Verificação se corresponde ao ID do Funcionário
bool checkID(void){ 
  if (rfid.uid.size != sizeof(funcionarioID)){
    return false;
  }
  for (byte i = 0; i < sizeof(funcionarioID); i++){
    if (rfid.uid.uidByte[i] != funcionarioID[i]){
      return false;
    }
  }
  return true;
}

// Rotina de Interrupções dos Botões
void enqueueISR(void){
  if(modoFifo && acessoAutorizado && digitalRead(BTNEMERG)){
    enqueue();
    acessoAutorizado = false;
  }
}

void DequeueISR(void){
  if(modoFifo && acessoAutorizado && digitalRead(BTNEMERG)){
    dequeue();
    acessoAutorizado = false;
  }
}

// Enviar Peso do Material para LCD 16x2
void enviarPeso(int peso){
  lcdI2C.clear();
  lcdI2C.print("");
  lcdI2C.setCursor(0, 0);
  lcdI2C.print("Peso: ");
  lcdI2C.print(peso);
  lcdI2C.print(" g");
}

// Mensagem de Boas-Vindas ao Utilizador
void welcome(void){
  lcdI2C.clear();
  lcdI2C.setCursor(0, 0);
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

void verificarBotaoEmergencia(void){
  if (digitalRead(BTNEMERG) == LOW){
    lcdI2C.clear();
    lcdI2C.setCursor(0, 0);
    lcdI2C.print("Emergência");
    while(digitalRead(BTNEMERG) == LOW){
      delay(100);
    }
    lcdI2C.clear();
  }
}

// FUNCIONALIDADE DO GOOGLE SHEETS
/*

// STRUCTS 
typedef struct {
  unsigned int minuto;
  unsigned int hora;
  unsigned int dia;
  unsigned int mes;
  unsigned int ano;
}ST_DATA;

const char *ssid = "SSID";
const char *password = "PASSWORD";
const char *ntpServer = "pool.ntp.org";

// WIFI & TIME
  WiFi.begin(ssid, password);
  configTime(0, 3600, ntpServer);

// Enviar Dados para o Google Sheets
void enviarSheets(String jsonData){
  HTTPClient http;
  http.begin("URL DO GOOGLE SHEETS");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(jsonData);
  http.end();
}

// Formatar String com os Dados para Enviar
void stringJsonData(String *jsonData){
  *jsonData = "{\"id_rfid\":\"" + String(funcionarioID[0], HEX) + String(funcionarioID[1], HEX) + 
                      String(funcionarioID[2], HEX) + String(funcionarioID[3], HEX) + "\",\"peso\":" + String(peso) + 
                      ",\"localizacao\":\"" + (head == MAX ? "tail " + String(tail+1) : "head " + String(head+1)) + 
                      "\",\"datahora\":\"" + String(dataHoraAno.dia) + "/" + String(dataHoraAno.mes) + "/" + 
                      String(dataHoraAno.ano) + " " + String(dataHoraAno.hora) + ":" + String(dataHoraAno.minuto) + "\"}";
}

// Obter Data e Hora Atuais
void dataHoraAtual(ST_DATA *dataHoraAno){
  struct tm data;
  dataHoraAno->minuto = data.tm_min;
  dataHoraAno->hora = data.tm_hour;
  dataHoraAno->dia = data.tm_mday;
  dataHoraAno->mes = data.tm_mon + 1;
  dataHoraAno->ano = data.tm_year + 1900;
}

*/

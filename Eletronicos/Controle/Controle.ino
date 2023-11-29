#include "esp_now.h"
#include "WiFi.h"

//Joystick
int VRx = 34; // diferente dos fios, mas é pq o codigo no tank é diferente
int VRy = 33;
int joyButtonPin = 17;

int deadZone = 45;

typedef struct {
  int joyButton;
  int joyX;
  int joyY;
}
InformacoesEnvio;
InformacoesEnvio dados;

// ESP NOW - transmissão de dados
uint8_t endereco[] = {0xA4,0xCF,0x12,0x81,0x06,0x98};
esp_now_peer_info_t peerInfo;

void setup()
{
  Serial.begin(9600);
  Serial.println("Iniciando Controle");

  // Buscando o MAC da placa
  WiFi.mode(WIFI_MODE_STA);
  //Serial.println(WiFi.macAddress());

  // Iniciando ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Registrando callback
  esp_now_register_send_cb(DadosEnviados);

  // Register peer
  memcpy(peerInfo.peer_addr, endereco, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{
  LerJoystick();
  PrintClass();
  
  esp_err_t resultado = esp_now_send(endereco, (uint8_t*)&dados, sizeof(dados));

  // if (resultado == ESP_OK) {
  //   Serial.println("Enviado");
  // }
  // else {
  //   Serial.println("Erro");
  // }

  delay(100);
}

void LerJoystick()
{
  dados.joyButton = digitalRead(joyButtonPin);
  dados.joyX = map(analogRead(VRx), 0, 4095, -512, 512);
  dados.joyY = map(analogRead(VRy), 0, 4095, -512, 512);

  // Aplicando deadzone
  if (abs(dados.joyX) < deadZone)
  {
    dados.joyX = 0;
  }
  if (abs(dados.joyY) < deadZone)
  {
    dados.joyY = 0;
  }
}

void PrintClass()
{
  Serial.print("X: ");
  Serial.print(dados.joyX);
  Serial.print(" | Y: ");
  Serial.print(dados.joyY);
  Serial.print(" | Button: ");
  Serial.println(dados.joyButton);
}

// Callback
void DadosEnviados(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

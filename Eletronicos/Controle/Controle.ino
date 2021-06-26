#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

// Radio
int CE_PIN = 9;
int CSN_PIN = 10;
const byte slaveAddress[5] = { 'R','x','A','A','A' };

RF24 radio(CE_PIN, CSN_PIN);


//Joystick
int VRx = A0;
int VRy = A1;
int joyButtonPin = 2;
int deadZone = 10;


typedef struct {
  int joyButton;
  int joyX;
  int joyY;
}
InformacoesEnvio;

InformacoesEnvio dados;

void setup()
{
  Serial.begin(9600);
  Serial.println("Iniciando Controle");

  // Joystick
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(joyButtonPin, INPUT_PULLUP);

  // Radio
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(3, 5); // delay, count
  radio.openWritingPipe(slaveAddress);
}

void loop()
{
  LerJoystick();
  //PrintClass();
  EnviarDados();

  delay(100);
}

void LerJoystick()
{
  dados.joyButton = digitalRead(joyButtonPin);
  dados.joyX = map(analogRead(VRx), 0, 1023, -512, 512);
  // Invertendo o Y pois o joystick tem os valores invertidos
  dados.joyY = map(analogRead(VRy), 0, 1023, 512, -512);

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

void EnviarDados()
{
  // Enviando informações
  if (radio.write(&dados, sizeof(dados)))
  {
    // Serial.println("  Acknowledge received");
  }
  else 
  {
    Serial.println("  Tx failed");
  }

  while (radio.available()) // Empty received auto-ACKs from PTX's RX FIFOs.
  {
    uint64_t blank;
    radio.read(&blank, sizeof(blank));
  }
}

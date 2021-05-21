#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

// Radio
int CE_PIN = 9;
int CSN_PIN = 10;
const byte slaveAddress[5] = { 'R','x','A','A','A' };
char dataToSend[10] = "Testedegr";

RF24 radio(CE_PIN, CSN_PIN);


//Joystick
int VRx = A3;
int VRy = A2;
int joyButtonPin = 2;


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
  radio.setPALevel(RF24_PA_MIN);
}

void loop()
{
  LerJoystick();
  PrintClass();
  // Enviando informações
  if (radio.write(&dataToSend, sizeof(dataToSend)))
  {
    Serial.println("  Acknowledge received");
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

  delay(100);
}

void LerJoystick()
{
  dados.joyButton = digitalRead(joyButtonPin);
  dados.joyX = map(analogRead(VRx), 0, 1023, -512, 512);
  dados.joyY = map(analogRead(VRy), 0, 1023, -512, 512);
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

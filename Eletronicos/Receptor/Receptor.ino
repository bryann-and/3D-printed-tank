#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Radio
int CE_PIN = 9;
int CSN_PIN = 10;
const byte thisSlaveAddress[5] = { 'R','x','A','A','A' };
char dataToSend[10] = "";

RF24 radio(CE_PIN, CSN_PIN);

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

    Serial.println("Receptor iniciando");
    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MIN);
    radio.openReadingPipe(1, thisSlaveAddress);    
    radio.startListening();
}

void loop() 
{
    while (radio.available())
    {
        Serial.print("teste");
        radio.read(&dataToSend, sizeof(dataToSend));
        PrintClass();
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

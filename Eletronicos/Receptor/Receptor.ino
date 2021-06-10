#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Radio
int CE_PIN = 9;
int CSN_PIN = 10;
const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};

RF24 radio(CE_PIN, CSN_PIN);

typedef struct
{
	int joyButton;
	int joyX;
	int joyY;
} InformacoesEnvio;

InformacoesEnvio dados;

#pragma region ESCs
Servo EscEsquerdo;
Servo EscDireito;
const int pinMotorEsquerdo = 2;
const int pinMotorDireito = 3;
int aceleradorEsquerdo = 0;
int aceleradorDireito = 0;
#pragma endregion

const int steeringReducer = 2;

void setup()
{
	Serial.begin(9600);
	Serial.println("Iniciando Receptor");

	EscEsquerdo.attach(pinMotorEsquerdo, 1000, 2000);
	EscDireito.attach(pinMotorDireito, 1000, 2000);

	resetarAcelerador();
	escreverESC();	
	
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	radio.openReadingPipe(1, slaveAddress);
	radio.startListening();
}

void loop()
{
	uint32_t timer = millis();
	while (radio.available())
	{
		// Detectando se ocorreu timeout
		if (millis() - timer > 1000)
		{
			radio.failureDetected = true;
			resetarAcelerador();
			// escreverESC();
			Serial.println("Timeout!");

			break;
		}

		radio.read(&dados, sizeof(dados));
		//PrintClass();
		processarAceleradorTanque();
		escreverESC();
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

void resetarAcelerador()
{
	aceleradorEsquerdo = aceleradorDireito = dados.joyX = dados.joyY = 0;
}

void processarAceleradorTanque()
{
	aceleradorEsquerdo = aceleradorDireito = dados.joyY;
	// Neutral steering
	if (dados.joyY == 0)
	{
		if (dados.joyX > 0)
		{
			aceleradorEsquerdo = dados.joyX;
			aceleradorDireito = -dados.joyX;
		}
		else if (dados.joyX < 0)
		{
			aceleradorEsquerdo = dados.joyX;
			aceleradorDireito = -dados.joyX;
		}
	}
	// Moving forward
	else if (dados.joyY > 0)
	{
		// Turning right
		if (dados.joyX > 0)
		{
			aceleradorEsquerdo = dados.joyY;
			aceleradorDireito = dados.joyY - dados.joyX / steeringReducer;
		}
		// Turning left
		else if (dados.joyX < 0)
		{
			aceleradorEsquerdo = dados.joyY - abs(dados.joyX) / steeringReducer;
			aceleradorDireito = dados.joyY;
		}
	}
	// Moving backwards
	else
	{
		// Turning right
		if (dados.joyX > 0)
		{
			aceleradorEsquerdo = dados.joyY;
			aceleradorDireito = dados.joyY + dados.joyX / steeringReducer;
		}
		// Turning left
		else if (dados.joyX < 0)
		{
			aceleradorEsquerdo = dados.joyY - dados.joyX / steeringReducer;
			aceleradorDireito = dados.joyY;
		}
	}
}

void escreverESC()
{
	EscEsquerdo.write(map(aceleradorEsquerdo, -512, 512, 0, 180));
	EscDireito.write(map(aceleradorDireito, -512, 512, 0, 180));
}
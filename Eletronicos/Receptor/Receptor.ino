#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Radio
int CE_PIN = 9;
int CSN_PIN = 10;
const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};
unsigned long lastDataReceived;

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
int escMin = 0;
int escMax = 180;
#pragma endregion

int aceleradorEsquerdo = 0;
int aceleradorDireito = 0;
int modoBaixaVelocidade = 0;
const int steeringReducer = 2;
unsigned long lastAnalogBtnPress = millis();

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

	lastDataReceived = millis();
}

void loop()
{
	while (radio.available())
	{
		lastDataReceived = millis();
		radio.read(&dados, sizeof(dados));
		//PrintClass();
		#pragma region Processando o botão do analogico
		// Só vai alterar caso já tenha passado 1 segundo do ultimo click
		if (dados.joyButton == 1 && millis() - lastAnalogBtnPress > 1000)
		{
			lastAnalogBtnPress = millis();			
			modoBaixaVelocidade = (modoBaixaVelocidade == 0 ? 1 : 0);

			if (modoBaixaVelocidade == 0)
			{
				escMin = 0;
				escMax = 180;
			}
			else
			{
				escMin = 60;
				escMax = 180 - 60;
			}			
		}
		#pragma endregion
		
		processarAceleradorTanque();
		escreverESC();
	}
	// timeout do controle
	if (millis() - lastDataReceived > 2000)
	{
		resetarAcelerador();
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
	EscEsquerdo.write(map(aceleradorEsquerdo, -512, 512, escMin, escMax));
	EscDireito.write(map(aceleradorDireito, -512, 512, escMin, escMax));
}
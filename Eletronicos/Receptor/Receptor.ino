#include "ESP32Servo.h"
#include "esp_now.h"
#include "WiFi.h"

typedef struct
{
	int joyButton;
	int joyX;
	int joyY;
} InformacoesEnvio;

InformacoesEnvio dados;

unsigned long ultimoSinal;

#pragma region ESCs
const int pinMotorEsquerdo = 32;
const int pinMotorDireito = 33;
int escMin = 205;
int escMax = 410;
#pragma endregion

int aceleradorEsquerdo = 0;
int aceleradorDireito = 0;
int modoBaixaVelocidade = 0;
const int steeringReducer = 2;
unsigned long lastAnalogBtnPress;

void setup()
{
	// while (!Serial);
	Serial.begin(9600);
	Serial.println("Iniciando Receptor");

	const int freq = 50;
	// ESC esquerdo
	ledcSetup(1, freq, 12);
	ledcAttachPin(pinMotorEsquerdo, 1);
	// ESC direito
	ledcSetup(2, freq, 12);
	ledcAttachPin(pinMotorDireito, 2);

	resetarAcelerador();
	escreverESC();

	ultimoSinal = lastAnalogBtnPress = millis();

	// Buscando o MAC da placa
	WiFi.mode(WIFI_MODE_STA);
	Serial.println(WiFi.macAddress());

	// Iniciando ESP-NOW
	if (esp_now_init() != ESP_OK) {
		Serial.println("Error initializing ESP-NOW");
		return;
	}
	// Registrando callback
	esp_now_register_recv_cb(DadosRecebidos);
}

void loop()
{	
	// 	PrintClass();
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
	
	// lerAceleradorViaSerial();
	processarAceleradorTanque();
	escreverESC();
	PrintClass();

	// timeout do controle
	if (millis() - ultimoSinal > 2000)
	{
		Serial.println("Timeout!");
		resetarAcelerador();
		escreverESC();
	}

	// delay(100);
}

void DadosRecebidos(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&dados, incomingData, sizeof(dados));  
  ultimoSinal = millis();
  PrintClass();
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

void lerAceleradorViaSerial ()
{
	if (Serial.available())
	{
		String leitura = Serial.readString();
		
		dados.joyX = leitura.toInt();
		dados.joyY = leitura.toInt();
	}
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
	//EscEsquerdo.write(map(aceleradorEsquerdo, -512, 512, escMin, escMax));
	ledcWrite(1, map(aceleradorEsquerdo, -512, 512, escMin, escMax));
	//EscDireito.write(map(aceleradorDireito, -512, 512, escMin, escMax));
	ledcWrite(2, map(aceleradorDireito, -512, 512, escMin, escMax));
}
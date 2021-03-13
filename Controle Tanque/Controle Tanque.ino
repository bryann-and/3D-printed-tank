/*
 Name:		Controle_Tanque.ino
 Created:	12/03/2021 17:02:23
 Author:	bryan
*/

#include <WiFiUdp.h>
#include <WiFiType.h>
#include <WiFiServer.h>
#include <WiFiScan.h>
#include <WiFiMulti.h>
#include <WiFiGeneric.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ETH.h>
#include <WiFi.h>

#pragma region Dados de Conexão
const char* ssid = "Conecte se for capaz";
const char* senha = "Conectado";
const char* host = "192.168.0.198";
const uint16_t porta = 11000;
#pragma endregion

void setup() 
{
	Serial.begin(9600);

	WiFi.begin(ssid, senha);
	Serial.println("Conectando...");
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(500);
	}

	Serial.print("Conectado com o IP: ");
	Serial.println(WiFi.localIP());
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	WiFiClient cliente;
	if (cliente.connect(host, porta))
	{
		Serial.println("Conectado com o servidor");
		cliente.print("ESP32");
		cliente.stop();
	}
	else 
	{
		Serial.println("Conexão com o servidor falhou!");
		delay(1000);
	}
}

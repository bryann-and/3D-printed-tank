/*
 Name:    Controle_Tanque.ino
 Created: 12/03/2021 17:02:23
 Author:  bryan
*/

#include <ESP32Tone.h>
#include <ESP32PWM.h>
#include <analogWrite.h>
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
#include <ESP32Servo.h>

#include<stdio.h>
#include <string.h>


#pragma region Dados de Conexão
const char* ssid = "Conecte se for capaz";
const char* senha = "Conectado";
const char* host = "192.168.0.198";
const uint16_t porta = 11000;
#pragma endregion

WiFiClient cliente;

int aceleradorEsquerda = 90;
int aceleradorDireita = 90;

#pragma region ESCs
Servo EscEsquerdo;
Servo EscDireito;
const int pinMotorEsquerdo = 19;
const int pinMotorDireito = 18;
#pragma endregion


void setup()
{
    Serial.begin(9600);

    delay(2000);

    WiFi.begin(ssid, senha);
    Serial.println("Conectando...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.print("Conectado com o IP: ");
    Serial.println(WiFi.localIP());

    conectarSocket();

    EscEsquerdo.attach(pinMotorEsquerdo, 1000, 2000);
    EscDireito.attach(pinMotorDireito, 1000, 2000);
}

// the loop function runs over and over again until power down or reset
void loop()
{
    if (cliente.connected())
    {
        while (cliente.available())
        {
            String string = cliente.readStringUntil(';');

            char* cstr = new char[string.length() + 1];
            strcpy(cstr, string.c_str());

            // Extract the first token
            char* token = strtok(cstr, "|");
            // loop through the string to extract all other tokens
            for (int i = 0; i < 2; i++)
            {
                //printf(" %s\n", token); //printing each token
                if (i == 0)
                {
                    aceleradorEsquerda = atoi(token);
                }
                else
                {
                    aceleradorDireita = atoi(token);
                }

                token = strtok(NULL, "|");
            }
        }
    }
    else
    {
        resetar();
        Serial.println("Conexão com o servidor perdida!");
        aceleradorEsquerda = aceleradorDireita = 0;
        delay(1000);
        conectarSocket();
    }

    EscEsquerdo.write(map(aceleradorEsquerda, -32768, 32767, 0, 180));
    EscDireito.write(map(aceleradorDireita, -32768, 32767, 0, 180));
}

void conectarSocket()
{
    Serial.println("Conectando ao servidor");
    while (!cliente.connect(host, porta))
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("Conectado com o servidor");
    cliente.print("ESP32");
    cliente.print("<EOF>");
}

void resetar()
{
    aceleradorEsquerda = 90;
    aceleradorDireita = 90;
}

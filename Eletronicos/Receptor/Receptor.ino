// SimpleRx - the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define CE_PIN 26
#define CSN_PIN 5

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

char dataReceived[10]; // this must match dataToSend in the TX
bool newData = false;

//===========

void setup() {

    Serial.begin(9600);

    Serial.println("SimpleRx Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
}

//=============

void loop() {
    while ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        Serial.println(dataReceived);
    }
}

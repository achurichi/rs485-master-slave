#include <DHT_U.h>
#include <DHT.h>

#define DHTTYPE DHT11 // DHT 11

#define LIGHT_CONVERSION(value) 2e7 * pow(value, -1.449)
#define ADC_TO_RESISTANCE(adc) -(1e5 * adc) / (adc - 1023)

const int enTxPin = 2; // HIGH:TX y LOW:RX
const int myDir = 102; //dir del esclavo

const int DHTPin = 7;
DHT dht(DHTPin, DHTTYPE);
const int lightSensorPin = A1;

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(100); //establecemos un tiempo de espera de 100ms

    pinMode(enTxPin, OUTPUT);
    digitalWrite(enTxPin, LOW); //RS485 como receptor

    dht.begin();
    pinMode(lightSensorPin, INPUT);
}

void loop()
{
    if (Serial.available()) // Si hay datos nuevos
    {
        // Se lee una línea
        String msj = Serial.readString();
        if (msj.startsWith(String("<" + String(myDir))) && msj.endsWith(">")) //Si el inicio y fin del mensaje son correctos y la dirección corresponde a este esclavo
        {
            msj = msj.substring(4, msj.length() - 1); // Se descarta la información que no sea de los sensores a leer

            delay(30);
            digitalWrite(enTxPin, HIGH); //RS485 como emisor
            Serial.print("<100");
            while (msj.length() != 0)
            {
                if (msj.charAt(0) == 'S') // Se lee todos los sensores solicitados
                {
                    switch (msj.charAt(1))
                    {
                    case '1':
                        Serial.print("S1");
                        Serial.print(int(dht.readHumidity()));
                        break;
                    case '2':
                        Serial.print("S2");
                        int light = ADC_TO_RESISTANCE(analogRead(lightSensorPin));
                        light = LIGHT_CONVERSION(light);
                        Serial.print(light);
                        break;
                    }

                    msj = msj.substring(2, msj.length());
                }
                else
                    msj = "";
            }
            Serial.print(">");
            Serial.flush(); //Esperamos hasta que se envíen los datos

            digitalWrite(enTxPin, LOW); //RS485 como receptor
        }
    }
}

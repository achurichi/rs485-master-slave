const int enTxPin = 2; // HIGH:TX y LOW:RX
const int myDir = 102; //dir del esclavo

const int waterSensorPin = A0;
const int lightSensorPin = A1;

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(100); //establecemos un tiempo de espera de 100ms

    pinMode(enTxPin, OUTPUT);
    digitalWrite(enTxPin, LOW); //RS485 como receptor

    pinMode(waterSensorPin, INPUT);
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
                        Serial.print(analogRead(waterSensorPin));
                        break;
                    case '2':
                        Serial.print("S2");
                        Serial.print(analogRead(lightSensorPin));
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

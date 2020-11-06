const int enTxPin = 2; // HIGH:TX y LOW:RX
const int myDir = 101; //dir del esclavo

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(100); //establecemos un tiempo de espera de 100ms
    pinMode(enTxPin, OUTPUT);
    digitalWrite(enTxPin, LOW); //RS485 como receptor
}

void loop()
{
    if (Serial.available())
    {
        if (Serial.read() == 'I') //Si recibimos el inicio de trama
        {
            int dir = Serial.parseInt(); //recibimos la dir
            if (dir == myDir)            //Si dir es la nuestra
            {
                char funcion = Serial.read(); //leemos el carácter de función

                if (funcion == 'S') //Si el caracter es L se solicita datos del sensor
                {
                    if (Serial.read() == 'F') //Si el fin de trama es el correcto
                    {
                        int lectura = analogRead(0); //realizamos  la lectura del sensor
                        digitalWrite(enTxPin, HIGH); //rs485 como transmisor
                        Serial.print("i");           //inicio de trama
                        Serial.print(myDir);         //dir
                        Serial.print(",");
                        Serial.print(lectura);      //valor del sensor
                        Serial.print("f");          //fin de trama
                        Serial.flush();             //Esperamos hasta que se envíen los datos
                        digitalWrite(enTxPin, LOW); //RS485 como receptor
                    }
                }
            }
        }
    }
    delay(10);
}

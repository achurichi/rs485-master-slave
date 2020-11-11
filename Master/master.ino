#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define ONBOARD_LED 2

const int EnTxPin = 0; // D3 - HIGH:TX y LOW:RX

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

int slave1 = 101;
int slave2 = 102;

byte degree[] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000};

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(100); //establecemos un tiempo de espera de 100ms
    // inicializamos los pines
    pinMode(EnTxPin, OUTPUT);
    digitalWrite(EnTxPin, HIGH); //RS485 como Transmisor

    pinMode(ONBOARD_LED, OUTPUT);
    digitalWrite(ONBOARD_LED, LOW);

    // LCD
    lcd.init(); // initialize the lcd
    lcd.backlight();
    lcd.createChar(0, degree);
    lcd.setCursor(0, 0);
    lcd.print("Esclavo:");
    lcd.setCursor(0, 1);
    lcd.print("S1:");
    lcd.setCursor(8, 1);
    lcd.print("S2:");
}

void loop()
{
    Serial.print("<");    // Inicio de trama
    Serial.print(slave1); // Dirección del esclavo
    Serial.print("S1");   // Datos del sensor 1
    Serial.print("S2");   // Datos del sensor 2
    Serial.print(">");    // Fin de trama
    Serial.flush();       //Esperamos hasta que se envíen los datos

    digitalWrite(EnTxPin, LOW); //RS485 como receptor
    delay(500);

    //----Leemos la respuesta del Esclavo-----
    if (Serial.available()) // Si hay datos nuevos
    {
        String msj = Serial.readString();
        if (msj.startsWith("<100") && msj.endsWith(">")) // Si la respuesta se recibió bien
        {
            msj = msj.substring(5, msj.length() - 1); // Eliminamos datos no necesarios
            String sensorData;

            lcd.setCursor(8, 0);
            lcd.print("1");

            while (msj.length() != 0) //Se leen, almacenan y muestran los datos
            {
                if (msj.charAt(0) != 'S')
                    sensorData += msj.charAt(0);
                else
                {
                    lcd.setCursor(3, 1);
                    lcd.print("     ");
                    lcd.setCursor(3, 1);
                    lcd.print(sensorData);
                    sensorData = "";
                }

                msj = msj.substring(1, msj.length());
            }
            lcd.setCursor(11, 1);
            lcd.print("     ");
            lcd.setCursor(11, 1);
            lcd.print(sensorData);
        }

        digitalWrite(EnTxPin, HIGH); //RS485 como Transmisor
    }

    // digitalWrite(ONBOARD_LED, HIGH);
    // delay(500);
    // digitalWrite(ONBOARD_LED, LOW);
    // delay(500);
}
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(EnTxPin, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(EnTxPin, HIGH); //RS485 como Transmisor

    // LCD
    lcd.init(); // initialize the lcd
    lcd.backlight();
    lcd.createChar(0, degree);
    lcd.setCursor(0, 0);
    lcd.print("Esclavo:");
    // lcd.setCursor(0, 1);
    // lcd.print("Set:");

    // Show default setted value
    // lcd.setCursor(5, 1);
    // lcd.print(setTemp);
    // lcd.write(0);
}

void loop()
{
    Serial.print("<");    // Inicio de trama
    Serial.print(slave1); // Dirección del esclavo
    Serial.print("S1");   // Datos del sensor 1
    Serial.print("S2");   // Datos del sensor 2
    Serial.print(">");    // Fin de trama
    Serial.flush();       //Esperamos hasta que se envíen los datos
    delay(100);

    //----Leemos la respuesta del Esclavo-----
    digitalWrite(EnTxPin, LOW);       //RS485 como receptor
                                      // if (Serial.find("i"))
                                      // {
    String msj = Serial.readString(); // Se lee una línea
    lcd.setCursor(0, 1);
    lcd.print("S1:");
    lcd.print(msj);
    // }

    // lcd.setCursor(8, 1);
    // lcd.print("S2:");

    // Serial.println(msj);

    // }

    // if (Serial.find("i"))       //esperamos el inicio de trama
    // {
    //     int esclavo = Serial.parseInt();            //recibimos la direccion del esclavo
    //     int dato = Serial.parseInt();               //recibimos el dato
    //     if (Serial.read() == 'f' && esclavo == 101) //si fin de trama y direccion son los correctos
    //     {
    //         funcion(dato); //realizamos la acción con el dato recibido
    //     }
    // }
    digitalWrite(EnTxPin, HIGH); //RS485 como Transmisor
                                 //----------fin de la respuesta----------
}
void funcion(int dato)
{
    if (dato > 500)
        digitalWrite(LED_BUILTIN, HIGH);
    else
        digitalWrite(LED_BUILTIN, LOW);
}

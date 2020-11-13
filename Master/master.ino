#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include "html/index_html.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

ESP8266WiFiMulti wifiMulti;  // Instancia para conectarse al WiFi
ESP8266WebServer server(80); // Instancia de la clase WebServer que responde a peticiones HTTP en el puerto 80

LiquidCrystal_I2C lcd(0x27, 16, 2); // Pantalla LCD de 16x2 en la Dirección I2C 0x27

const int EnTxPin = 0;  // D3 - HIGH:TX y LOW:RX
const int slave1 = 101; // Dirección del esclavo 1
const int slave2 = 102; // Dirección del esclavo 2
bool act = false;       // True: Pedir datos y False: No pedir datos
String slave = "1";     // El próximo esclavo a leer
String sensor1;         // Valor en el sensor 1 de esclavo leído
String sensor2;         // Valor en el sensor 2 de esclavo leído
unsigned long act_time; // Contador para saber cuando leer

void setup(void)
{
    // Comunicación Serial
    Serial.begin(9600);
    Serial.setTimeout(100); //establecemos un tiempo de espera de 100ms

    // inicialización de los pines
    pinMode(EnTxPin, OUTPUT);
    digitalWrite(EnTxPin, HIGH); //RS485 como Transmisor

    // LCD
    lcd.init(); // initialize the lcd
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Esclavo:");
    lcd.setCursor(0, 1);
    lcd.print("S1:");
    lcd.setCursor(8, 1);
    lcd.print("S2:");

    // Configuracón del servidor
    wifiMulti.addAP("La casa de Avra y Piru", "laquevosquieras"); // Elegir a que red WiFi conectarse
    while (wifiMulti.run() != WL_CONNECTED)                       // Se conecta a la red
        delay(250);
    server.on("/", HTTP_GET, handleRoot); // Se llama "handleRoot" cuando se solicita la URL "/"
    server.onNotFound(handleNotFound);    // Si se solicita una URL desconocida se llama a "handleNotFound"
    server.begin();                       // Inicializar el servidor

    act_time = millis(); // Inicializamos el contador
}

void loop(void)
{
    server.handleClient(); // Se escuchan las peticiones HTTP de los clientes

    if (act == true && act_time + 3000 == millis() && slave == "1")
    {
        sendSlave(slave1);           // Se piden datos al esclavo 1
        readSlave(slave, "%", "LX"); // Se leen los datos del esclavo 1 y se muestran por pantalla
        slave = "2";                 // El próximo esclabo a leer es el 2
        act_time = millis();         // Se actualiza el contador
    }
    else if (act == true && act_time + 3000 == millis() && slave == "2")
    {
        sendSlave(slave2);           // Se piden datos al esclavo 2
        readSlave(slave, "%", "LX"); // Se leen los datos del esclavo 2 y se muestran por pantalla
        slave = "1";                 // El próximo esclabo a leer es el 1
        act_time = millis();         // Se actualiza el contador
    }
}

void handleRoot()
{ // Las URL que se pueden solicitar junto con los verbos HTTP y la función a la que llaman
    server.send(200, "text/html", INDEX_HTML);
    server.on("/act_estado", HTTP_POST, post_act_estado);
    server.on("/consultar", HTTP_GET, get_consultar);
}

void post_act_estado()
{ // Actualiza el estado para leer o no a los esclavos
    StaticJsonDocument<500> jsonDoc;
    auto error = deserializeJson(jsonDoc, server.arg("plain"));

    if (error)
        server.send(400);
    else if (server.method() == HTTP_POST)
    {
        if (jsonDoc["msg"] == "iniciar")
        {
            act = true;
            act_time = millis();
        }
        else if (jsonDoc["msg"] == "detener")
            act = false;
        server.sendHeader("Location", "/act_estado/");
        server.send(200);
    }
}

void get_consultar()
{ // Se envía al cliente la información sobre el último sensor leido
    String jsonString;
    DynamicJsonDocument jsonDoc(1024);

    if (slave == "1")
        jsonDoc["slave"] = "2";
    if (slave == "2")
        jsonDoc["slave"] = "1";
    jsonDoc["sensor1"] = sensor1;
    jsonDoc["sensor2"] = sensor2;

    serializeJson(jsonDoc, jsonString);
    server.send(200, "application/javascript", jsonString);
}

void handleNotFound()
{                                                     
    server.send(404, "text/plain", "404: Not found"); // Si se solicita una URL desconocida se muestra un error 404
}

void sendSlave(int slaveDir)
{                           // Se le solicita al esclavo información sobre los sensores 1 y 2
    Serial.print("<");      // Inicio de trama
    Serial.print(slaveDir); // Dirección del esclavo
    Serial.print("S1");     // Datos del sensor 1
    Serial.print("S2");     // Datos del sensor 2
    Serial.print(">");      // Fin de trama
    Serial.flush();         // Esperamos hasta que se envíen los datos
}

void readSlave(String slaveNumber, String S1_unit, String S2_unit)
{                               // Se leen los datos de los sensores del esclavo, se almacenan y se muestran en la pantalla LCD
    digitalWrite(EnTxPin, LOW); //RS485 como receptor
    delay(500);

    if (Serial.available()) // Si hay datos nuevos
    {
        String msj = Serial.readString();
        if (msj.startsWith("<100") && msj.endsWith(">")) // Si la respuesta se recibió bien
        {
            msj = msj.substring(4, msj.length() - 1); // Eliminamos datos no necesarios
            String sensorData;

            lcd.setCursor(8, 0);
            lcd.print(slaveNumber);

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
                    lcd.print(S1_unit);
                    sensor1 = sensorData + S1_unit;
                    sensorData = "";
                    msj = msj.substring(1, msj.length());
                }

                msj = msj.substring(1, msj.length());
            }
            lcd.setCursor(11, 1);
            lcd.print("     ");
            lcd.setCursor(11, 1);
            lcd.print(sensorData);
            lcd.print(S2_unit);
            sensor2 = sensorData + S2_unit;
        }

        digitalWrite(EnTxPin, HIGH); //RS485 como Transmisor
    }
}
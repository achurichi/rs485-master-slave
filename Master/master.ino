#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include "html/index_html.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

ESP8266WiFiMulti wifiMulti;  // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const int EnTxPin = 0; // D3 - HIGH:TX y LOW:RX
const int slave1 = 101;
const int slave2 = 102;
bool act;
String slave = "1";
String sensor1;
String sensor2;
unsigned long act_time;

void setup(void)
{
    // Comunicación Serial
    Serial.begin(9600);
    Serial.setTimeout(100); //establecemos un tiempo de espera de 100ms

    // inicializamos los pines
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
    wifiMulti.addAP("La casa de Avra y Piru", "laquevosquieras"); // Elegimos la red

    int i = 0;
    while (wifiMulti.run() != WL_CONNECTED)
    { // Se conecta a la red
        delay(250);
        Serial.print('.');
    }

    server.on("/", HTTP_GET, handleRoot); // Se llama 'handleRoot' cuando se solicita la URI "/"
    server.onNotFound(handleNotFound);    // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

    server.begin(); // Actually start the server

    act_time = millis();
}

void loop(void)
{
    server.handleClient(); // Listen for HTTP requests from clients

    if (act == true && act_time + 3000 == millis() && slave == "1")
    {
        sendSlave(slave1);
        readSlave(slave, "%", "LX");
        slave = "2";
        act_time = millis();
    }
    else if (act == true && act_time + 3000 == millis() && slave == "2")
    {
        sendSlave(slave2);
        readSlave(slave, "%", "LX");
        slave = "1";
        act_time = millis();
    }
}

void handleRoot()
{ // When URI / is requested, send a web page with a button to toggle the LED
    server.send(200, "text/html", INDEX_HTML);
    server.on("/act_estado", HTTP_POST, post_act_estado);
    server.on("/consultar", HTTP_GET, get_consultar);
}

void post_act_estado()
{
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
{
    String jsonString;
    DynamicJsonDocument jsonDoc(1024);

    jsonDoc["slave"] = slave;
    jsonDoc["sensor1"] = sensor1;
    jsonDoc["sensor2"] = sensor2;

    serializeJson(jsonDoc, jsonString);
    server.send(200, "application/javascript", jsonString);
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void sendSlave(int slaveDir)
{
    Serial.print("<");      // Inicio de trama
    Serial.print(slaveDir); // Dirección del esclavo
    Serial.print("S1");     // Datos del sensor 1
    Serial.print("S2");     // Datos del sensor 2
    Serial.print(">");      // Fin de trama
    Serial.flush();         // Esperamos hasta que se envíen los datos
}

void readSlave(String slaveNumber, String S1_unit, String S2_unit)
{
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
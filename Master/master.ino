#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include "html/index_html.h"
#include <ArduinoJson.h>

ESP8266WiFiMulti wifiMulti; // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80); // Create a webserver object that listens for HTTP request on port 80

const int led = 2;

void handleRoot(); // function prototypes for HTTP handlers
void handleLED();
void handleNotFound();

void setup(void)
{
    Serial.begin(115200); // Start the Serial communication to send messages to the computer
    delay(10);
    Serial.println('\n');

    pinMode(led, OUTPUT);

    wifiMulti.addAP("La casa de Avra y Piru", "laquevosquieras"); // add Wi-Fi networks you want to connect to

    Serial.println("Connecting ...");
    int i = 0;
    while (wifiMulti.run() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(250);
        Serial.print('.');
    }
    Serial.println('\n');
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID()); // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer

    server.on("/", HTTP_GET, handleRoot); // Call the 'handleRoot' function when a client requests URI "/"
    server.onNotFound(handleNotFound);    // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

    server.begin(); // Actually start the server
    Serial.println("HTTP server started");
}

void loop(void)
{
    server.handleClient(); // Listen for HTTP requests from clients
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
    {
        server.send(400);
    }
    else if (server.method() == HTTP_POST)
    {
        if (jsonDoc["msg"] == "iniciar")
            Serial.println("Iniciado");
        else if (jsonDoc["msg"] == "detener")
            Serial.println("Detenido");
        server.sendHeader("Location", "/act_estado/");
        server.send(200);
    }
}

void get_consultar()
{
    String jsonString;
    DynamicJsonDocument jsonDoc(1024);

    jsonDoc["slave"] = 1;
    jsonDoc["sensor1"] = 2;
    jsonDoc["sensor2"] = 3;

    serializeJson(jsonDoc, jsonString);
    server.send(200, "application/javascript", jsonString);
}

void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
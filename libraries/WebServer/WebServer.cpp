/*
  WebServer.cpp - Library create a Web Server.
  Created by Churichi A, Pentácolo T. and Gonzalez E. November 18, 2019.
  Released into the public domain.
*/

#include "WebServer.h"

// Variables usadas por la libreria (solo deben ser usadas por la libreria)

ESP8266WebServer server(80);
datos_resource *ws_d_resource;
datos_usuario_resource *ws_d_usuario_resource;
const char *ws_ssid;
const char *ws_password;
volatile uint8_t estadoPaginaWeb = 0;

bool qwert = false;

// Funciones

// Inicializa el serivdor web
void webServerInit(datos_resource *d_resource, datos_usuario_resource *d_usuario_resource, const char *ssid, const char *password)
{
    ws_d_resource = d_resource;
    ws_d_usuario_resource = d_usuario_resource;
    ws_ssid = ssid;
    ws_password = password;

    //WiFi.config(ip, gateway, subnet);
    WiFi.mode(WIFI_AP);
    WiFi.hostname("ESP8266");
    WiFi.softAP(ws_ssid, ws_password);
    MDNS.begin("ESP8266");
    MDNS.addService("http", "tcp", 80);

    IPAddress myIP = WiFi.softAPIP();
    server.on("/", handleRoot);
    server.begin();
}

// Hace el trabajo necesario para que se pueda mostrar la página
void ESPhandleClient()
{
    server.handleClient();
}
// Convierte un paquete json a estructura datos_usuario (solo debe ser usada por la libreria)

void json_to_resource_datos_usuario(StaticJsonDocument<500> &jsonDoc)
{
    int16_t lat_i[3];
    int16_t long_i[3];
    int16_t lat_f[3];
    int16_t long_f[3];

    lat_i[0] = jsonDoc["lat_inicial_grad"];
    lat_i[1] = jsonDoc["lat_inicial_min"];
    lat_i[2] = jsonDoc["lat_inicial_seg"];
    long_i[0] = jsonDoc["long_inicial_grad"];
    long_i[1] = jsonDoc["long_inicial_min"];
    long_i[2] = jsonDoc["long_inicial_seg"];

    lat_f[0] = jsonDoc["lat_final_grad"];
    lat_f[1] = jsonDoc["lat_final_min"];
    lat_f[2] = jsonDoc["lat_final_seg"];
    long_f[0] = jsonDoc["long_final_grad"];
    long_f[1] = jsonDoc["long_final_min"];
    long_f[2] = jsonDoc["long_final_seg"];

    ws_d_usuario_resource->latitudInicialSegundos = equivalent(lat_i);
    ws_d_usuario_resource->longitudInicialSegundos = equivalent(long_i);
    ws_d_usuario_resource->latitudFinalSegundos = equivalent(lat_f);
    ws_d_usuario_resource->longitudFinalSegundos = equivalent(long_f);

    ws_d_usuario_resource->concentracion = jsonDoc["concentracion"];
    ws_d_usuario_resource->declinacion = jsonDoc["declinacion"];
}

// Recibe un paquete json y actualiza el estado del equipo (solo debe ser usada por la libreria)
void json_to_resource_act_estado(StaticJsonDocument<500> &jsonDoc)
{
    if (jsonDoc["msg"] == "iniciar")
    {
        ws_d_resource->explorando = true; //Esto es solo de prueba
        ws_d_resource->detenido = false;  //Esto es solo de prueba
        // enviarDatosMaster(0x08,ws_d_resource->explorando,I2C_Iniciar1);
        // delay(10);
        qwert = true;
        estadoPaginaWeb = iniciar;
    }
    else if (jsonDoc["msg"] == "detener")
    {
        Serial.println("pase...");
        ws_d_resource->explorando = true; //Esto es solo de prueba
        ws_d_resource->detenido = true;   //Esto es solo de prueba
        estadoPaginaWeb = detener;
    }
    else if (jsonDoc["msg"] == "continuar")
    {
        ws_d_resource->explorando = true; //Esto es solo de prueba
        ws_d_resource->detenido = false;  //Esto es solo de prueba
        estadoPaginaWeb = continuar;
    }
    else if (jsonDoc["msg"] == "abortar")
    {
        ws_d_resource->explorando = false; //Esto es solo de prueba
        ws_d_resource->detenido = true;    //Esto es solo de prueba
        estadoPaginaWeb = abortar;
    }
}

// Levanta el INDEX_HTML y declara los verbos HTTP con sus funciones correspondientes (solo debe ser usada por la libreria)
void handleRoot()
{
    server.send(200, "text/html", INDEX_HTML);
    server.on("/datos_usuario", HTTP_PUT, put_datos_usuario);
    server.on("/act_estado", HTTP_POST, post_act_estado);
    server.on("/consultar", HTTP_GET, get_consultar);
}

// Función correspondiente al verbo HTTP PUT (solo debe ser usada por la libreria)
void put_datos_usuario()
{
    StaticJsonDocument<500> jsonDoc;
    auto error = deserializeJson(jsonDoc, server.arg("plain"));

    if (error)
    {
        server.send(400);
    }
    else if (server.method() == HTTP_PUT)
    {
        json_to_resource_datos_usuario(jsonDoc);
        server.sendHeader("Location", "/datos_usuario/");
        server.send(200);
    }
}

// Función correspondiente al verbo HTTP POST (solo debe ser usada por la libreria)
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
        json_to_resource_act_estado(jsonDoc);
        server.sendHeader("Location", "/act_estado/");
        server.send(200);
    }
}

// Función correspondiente al verbo HTTP GET (solo debe ser usada por la libreria)
void get_consultar()
{
    String jsonString;
    DynamicJsonDocument jsonDoc(1024);

    jsonDoc["concentracion"] = ws_d_resource->concentracion;
    jsonDoc["latitud"] = ws_d_resource->latitud;
    jsonDoc["longitud"] = ws_d_resource->longitud;
    jsonDoc["t_transcurrido"] = ws_d_resource->t_transcurrido;
    jsonDoc["t_disponible"] = ws_d_resource->t_disponible;
    jsonDoc["listo_para_explorar"] = ws_d_resource->listo_para_explorar;
    jsonDoc["explorando"] = ws_d_resource->explorando;
    jsonDoc["detenido"] = ws_d_resource->detenido;
    jsonDoc["inclinacion"] = ws_d_resource->inclinacion;
    jsonDoc["error"] = ws_d_resource->error;

    serializeJson(jsonDoc, jsonString);
    server.send(200, "application/javascript", jsonString);
}

uint8_t getEstadoPaginaWeb()
{
    return estadoPaginaWeb;
}

bool inicioWeb()
{
    return qwert;
}

void setWeb()
{
    qwert = false;
}
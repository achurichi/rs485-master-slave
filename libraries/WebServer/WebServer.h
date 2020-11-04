/*
  WebServer.h - Library create a Web Server.
  Created by Churichi A, Pentácolo T. and Gonzalez E. November 18, 2019.
  Released into the public domain.
*/

#ifndef WebServer_h
#define WebServer_h

#include "Arduino.h"
#include "index_html.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <stdio.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>

// Definición de funciones

void webServerInit(datos_resource *d_resource, datos_usuario_resource *d_usuario_resource, const char *ssid, const char *password);
void ESPhandleClient();
void json_to_resource_datos_usuario(StaticJsonDocument<500> &jsonDoc);
void json_to_resource_act_estado(StaticJsonDocument<500> &jsonDoc);
void handleRoot();
void put_datos_usuario();
void post_act_estado();
void get_consultar();
uint8_t getEstadoPaginaWeb();
bool inicioWeb();
void setWeb();

#endif

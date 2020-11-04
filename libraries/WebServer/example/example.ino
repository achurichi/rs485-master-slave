#include <WebServer.h>
#include <Datatype.h>

unsigned long tiempo;

const char *ssid = "ESPWebServer"; //ssid del servidor web
const char *password = "password"; //contraseña del servidor web

datos_resource d_resource;
datos_usuario_resource d_usuario_resource;

void setup()
{

    d_resource.concentracion = 100;        //Valor de concentracion leido por el sensor
    d_resource.latitud = 30.6485;          //Latitud leida por el sensor
    d_resource.longitud = 60.4528;         //Longitud leida por el sensor
    d_resource.t_transcurrido = 2000;      //Tiempo transcurrido desde que se inició la búsqueda
    d_resource.t_disponible = 5000;        //Tiempo de bateria disponible
    d_resource.listo_para_explorar = true; //Booleano que esta en true si el equipo esta listo
    d_resource.explorando = false;         //Booleano para saber si el robot se encuentra explorando
    d_resource.detenido = true;            //Booleano para saber si el robot se encuentra detenido (puede estar explorando pero detenido)
    d_resource.inclinacion = 45;
    d_resource.error = 0; //No hay Error

    webServerInit(&d_resource, &d_usuario_resource, ssid, password);

    Serial.begin(115200);

    tiempo = millis();
}

void loop()
{
    ESPhandleClient();

    if (millis() >= tiempo + 1000)
    {
        d_resource.concentracion++;
        d_resource.latitud += 0.0001;
        d_resource.longitud += 0.0001;
        d_resource.t_transcurrido++;
        // d_resource.t_disponible += 60;
        tiempo = millis();

        Serial.print("Latitud inicial: ");
        Serial.print(d_usuario_resource.lat_inicial[0]);
        Serial.print("º ");
        Serial.print(d_usuario_resource.lat_inicial[1]);
        Serial.print("' ");
        Serial.print(d_usuario_resource.lat_inicial[2]);
        Serial.println("''");
        Serial.print("Longitud inicial: ");
        Serial.print(d_usuario_resource.long_inicial[0]);
        Serial.print("º ");
        Serial.print(d_usuario_resource.long_inicial[1]);
        Serial.print("' ");
        Serial.print(d_usuario_resource.long_inicial[2]);
        Serial.println("''");

        Serial.print("Latitud final: ");
        Serial.print(d_usuario_resource.lat_final[0]);
        Serial.print("º ");
        Serial.print(d_usuario_resource.lat_final[1]);
        Serial.print("' ");
        Serial.print(d_usuario_resource.lat_final[2]);
        Serial.println("''");
        Serial.print("Longitud final: ");
        Serial.print(d_usuario_resource.long_final[0]);
        Serial.print("º ");
        Serial.print(d_usuario_resource.long_final[1]);
        Serial.print("' ");
        Serial.print(d_usuario_resource.long_final[2]);
        Serial.println("''");

        Serial.print("Concentración:");
        Serial.println(d_usuario_resource.concentracion);
        Serial.print("Declinación:");
        Serial.println(d_usuario_resource.declinacion);

        Serial.print("Explorando:");
        Serial.println(d_resource.explorando ? "SI" : "NO");
        Serial.print("Detenido:");
        Serial.println(d_resource.detenido ? "SI" : "NO");
        Serial.print("\n");

        //     // datos_resource.explorando = !datos_resource.explorando;

        //     // datos_resource.listo_para_explorar = true;
        //Serial.print(sizeof(bool));
    }
}

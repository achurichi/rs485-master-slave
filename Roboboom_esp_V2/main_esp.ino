#include <I2C_Master.h>
#include <Brujula.h>
#include <Datatype.h>
#include <WebServer.h>

#define DEBUG

bool configurandoDispositivo();
void pedirDatos();
void actualizarEstadoPaginaWeb();
void normalFuncionamiento();

datos_resource d_resource;
datos_usuario_resource d_usuario_resource;
datos_resource pruebaEstructura;
punto pto;

Coordenadas GPS_C;
estadoWeb estadoPaginaWebESP;

const char *ssid = "ESPWebServer_exe"; //ssid del servidor web
const char *password = "password";     //contraseña del servidor web

unsigned long tiempo;
int cant = 0;
float asdasd = 0;
volatile float prueba = 0;
volatile uint8_t j = 0;
volatile uint32_t tiempo_de_detenido;
bool detenido_bool = false;
bool continuado = false;

void setup()
{
  I2C_MasterInit(&d_resource, &d_usuario_resource);
  webServerInit(&d_resource, &d_usuario_resource, ssid, password);
  BrujulaInit(&d_resource);

#ifdef DEBUG
  Serial.begin(9600);

  Serial.print("latitud");
  Serial.println(GPS_C.latitud);

  Serial.print("el tamaño de la struct es ");
  Serial.println(sizeof(GPS_C));

  Serial.print("el tamaño de float es ");
  Serial.println(sizeof(float));

#endif
  GPS_C.latitud = 3;
  GPS_C.longitud = 5;

  d_resource.t_disponible = 0; //Tiempo de bateria disponible
 
  delay(3000);
  tiempo = millis();
}

void loop()
{
  while (!configurandoDispositivo())
  {
  }

  while (true)
  {
    ESPhandleClient();
    actualizarEstadoPaginaWeb();
    switch (getEstadoPaginaWeb())
    {
    case iniciar:
      //antes enviar los datos de usuario
      normalFuncionamiento();

      break;
    case detener:
      detenerTiempo();
      //pausar tiempo
      break;
    case continuar:
      normalFuncionamiento();
      break;
    case abortar:
      abortarSistema();
      //terminar
      break;
    }


  }
}

void abortarSistema()
{
  uint8_t finalizar = 33;
  uint8_t confirmacionFinalizar = 0;
  enviarDatosMaster(0x08, finalizar, I2C_reset);
  recibirDatosMaster(0x08, &confirmacionFinalizar, I2C_reset_confirmacion);
  if (finalizar == confirmacionFinalizar)
  {
    delay(1000);
    ESP.restart();
  }
  else
  {
     d_resource.error=7; //ERROR: no se pudo cancelar el recorrido
  }
}
void detenerTiempo()
{
  if (!detenido_bool)
  {
    tiempo_de_detenido = millis();
    detenido_bool = true;
    continuado = false;
  }
}

bool configurandoDispositivo()
{
  static bool bateriaLeida = false;
  ESPhandleClient();
  if (d_resource.t_disponible == 0 && !d_resource.listo_para_explorar)
  {
    if (recibirDatosMaster(0x08, &d_resource.t_disponible, I2C_bateria) != 0)
    {
#ifdef DEBUG
      Serial.print("bateria: ");
      Serial.println(d_resource.t_disponible);
#endif
      bateriaLeida = true;
      delay(25);
    }
  }
  else
  {
#ifdef DEBUG
    Serial.println("esperando informacion de la bateria...");
#endif
  }

  if (!d_resource.listo_para_explorar)
  {
    recibirDatosMaster(0x08, &d_resource.listo_para_explorar, I2C_listo);
#ifdef DEBUG
    Serial.print("listo para explorar: ");
    Serial.println(d_resource.listo_para_explorar);
#endif
    delay(25);
  }

  else
  {
#ifdef DEBUG
    Serial.println("esperando confirmacion del gps...");
#endif
  }

  return bateriaLeida && d_resource.listo_para_explorar;
}

void pedirDatos()
{
  uint8_t conc = 0;
  uint8_t err = 0;
  recibirDatosMaster(0x08, &conc, I2C_concentracion);
  d_resource.concentracion = conc;
  delay(50);
  recibirDatosMaster(0x08, &GPS_C, I2C_leerGPS);
  d_resource.latitud = GPS_C.latitud;
  d_resource.longitud = GPS_C.longitud;
  delay(50);
  recibirDatosMaster(0x08, &err, I2C_error);
  d_resource.error = err;
  delay(50);
 
}

void actualizarEstadoPaginaWeb()
{
  static uint8_t estadoActualPaginaWeb = 0;
  if (estadoActualPaginaWeb != getEstadoPaginaWeb())
  {
    estadoActualPaginaWeb = getEstadoPaginaWeb();
    estadoPaginaWebESP.explorandoWEB = d_resource.explorando;
    estadoPaginaWebESP.detenidoWEB = d_resource.detenido;
    estadoPaginaWebESP.estadoWEB = getEstadoPaginaWeb();
    Serial.println("enviando estado pagina web");
    enviarDatosMaster(0x08, estadoPaginaWebESP, I2C_estadoWeb);
     }
  
}

void normalFuncionamiento()
{
  static bool iniciado = false;
  static bool led = 0;
  static uint16_t k = 0;
  static uint32_t offsetTime = 0;

  if (getEstadoPaginaWeb() == iniciar && !iniciado)
  {
    // enviarDatosMaster(0x08,&d_usuario_resource,I2C_datosUsuario);
    pto.latitud = d_usuario_resource.latitudInicialSegundos;
    pto.longitud = d_usuario_resource.longitudInicialSegundos;
    enviarDatosMaster(0x08, pto, I2C_punto_I);
    delay(50);
    pto.latitud = d_usuario_resource.latitudFinalSegundos;
    pto.longitud = d_usuario_resource.longitudFinalSegundos;
    enviarDatosMaster(0x08, pto, I2C_punto_F);
    delay(50);
    enviarDatosMaster(0x08, d_usuario_resource.concentracion, I2C_concentracionUsuario);
    delay(50);
 
    iniciado = true;
    d_resource.t_transcurrido = 0;
    offsetTime = millis();
  }
  if (getEstadoPaginaWeb() == continuar && !continuado)
  {

    continuado = true;
    offsetTime += (millis() - tiempo_de_detenido);
    detenido_bool = false;
  }
  if (iniciado && millis() >= tiempo + 39)
  {
 

    d_resource.t_transcurrido = (millis() - offsetTime) / 1000;
    k++;

    uint16_t bruj = leerBrujula();
    Serial.print("Brujula: ");
    Serial.println(bruj);
    enviarDatosMaster(0x08, bruj, I2C_brujula);

#ifdef DEBUG
    Serial.println(bruj);
#endif
    if (k == 17)
    {
      uint16_t conc = 0;

      recibirDatosMaster(0x08, &conc, I2C_concentracion);
      d_resource.concentracion = conc;
    }
    if (k == 27)
    {

      recibirDatosMaster(0x08, &GPS_C, I2C_leerGPS);
      d_resource.latitud = GPS_C.latitud;
      d_resource.longitud = GPS_C.longitud;

#ifdef DEBUG
      Serial.println("Latitud: " + String(d_resource.latitud, 4) + " || longitud: " + String(d_resource.longitud, 4));
 
#endif
      k = 0;
    }
   

    tiempo = millis();
  }
}

void enviarDatosUsuario()
{
  
  Serial.print("Concentración:");
  Serial.println(d_usuario_resource.concentracion); 
  delay(100);
}

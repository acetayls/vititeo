/*
Visual Studio Code ave Platformio
Simple station pour Vitipole

TODO :
  [X] sleep mode NODE
  [ ] test couverture
  [ ] implementation OTA
  [X] date
*/


#define GATEWAY 1

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>


#define DEBUG 1 //0 ou 1 pour activer la sortie sur port série
#define SERIAL_BAUDRATE 9600

// Lora conection sur cette carte (TTGO)
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DI0 26

#define BAND 866E6

#if GATEWAY
  const String gatewayName = "GateWay 1";
  const char* wifi_ssid = "AOP";
  const char* wifi_pass = "AuroreOctavePierrick";
  const char* serverName = "https://stations.protechebdo.fr/api/records";
  #include "gateway.cpp"
#else
  
  String node_id = "node1"; //indentifiant du noeud
  float node_lat = 0.1; //latitude
  float node_lon = 0.2; //longitude
  float node_elevation = 100; //altitude

  float node_temp = -1.1; //temperature (°C)
  float node_hygro = -1.1; //Hgrometrie (%)

  float node_battery = 50; // battery (non implementée)
  unsigned int temps_entre_message_min = 1;
  #include "node.cpp"
#endif

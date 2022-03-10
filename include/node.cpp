#include "DHT.h"



StaticJsonDocument<200> lora_json;
unsigned long time_between_message_ms = temps_entre_message_min*60*1000;

unsigned int last_millis;

//Configuration capteur temperature
#define DHTPIN 16
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

// Fonction lire la temperature
void read_temp() {
  node_hygro = dht.readHumidity();
  // Read temperature as Celsius (the default)
  node_temp = dht.readTemperature();

}


//Fonction création message
void create_message() {
  lora_json["node"] = node_id;
  lora_json["lat"]= node_lat;
  lora_json["lon"] = node_lon;
  lora_json["alt"] =node_alt;
  lora_json["bat"] = node_battery;
  lora_json["temp"] = node_temp;
  lora_json["hyg"] = node_hygro;
}



//Fonction setup appelée à l'initialisation
void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  while (!Serial);

  Serial.println("LoRa Sender");

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);  
  }
#if DEBUG
  Serial.println("end setup of Node");
  Serial.println("start temp sensor");
#endif
  

  
  dht.begin();
}

void loop() {
  // Si le temps écoulé est > à time_between_message --> on envois
  //if ((millis()- last_millis) > time_between_message ){ 
    #if DEBUG
    Serial.print("Sending packet: ");
    #endif
    read_temp();
    create_message();
    String msg_to_send;
    serializeJson(lora_json, msg_to_send);
    #if DEBUG
    Serial.println(msg_to_send);
    #endif

    // send packet
    LoRa.beginPacket();
    LoRa.print(msg_to_send);
    LoRa.endPacket();

    // go to deep sleep for x microseconds
    esp_sleep_enable_timer_wakeup(time_between_message_ms*1000);
    esp_deep_sleep_start();
  //}
  // Sinon on attend 1s
  //else {
  //  delay(1000);
  //}
}



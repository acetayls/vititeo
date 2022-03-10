#include "WiFi.h"
#include "HTTPClient.h"
#include "ESPDateTime.h"

StaticJsonDocument<200> json_received;


void LoRa_rxMode(){
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void send_json_to_server() {
    #if DEBUG
    Serial.println("Send to Server");
    #endif
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      Serial.println("make data");
      // Data to send from json
      String httpRequestData;
      serializeJson(json_received, httpRequestData);
      // Send HTTP POST request
      Serial.print("send data : ");
      Serial.println(httpRequestData);
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(httpRequestData);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}

void parse_message(String received){
    DeserializationError error = deserializeJson(json_received, received);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }

  else {
      json_received["gateway"] = gatewayName;
      json_received["mesured_at"] = "2022-03-01 10:10";
      send_json_to_server();
  }
}



void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
  #if DEBUG
    Serial.print("Gateway Receive: ");
    Serial.println(message);
  #endif
  parse_message(message);
}

void setupDateTime() {

  DateTime.setServer("fr.pool.ntp.org");
  DateTime.setTimeZone("CET-1CEST");
  DateTime.begin();
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
  } else {
    Serial.printf("Date Now is %s\n", DateTime.toISOString().c_str());
    Serial.printf("Timestamp is %ld\n", DateTime.now());
  }
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  while (!Serial);

  #if DEBUG
  Serial.println("Start lora gateway");
  #endif
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);  
  }

  #if DEBUG
  Serial.println("Start Wifi");
  #endif
  WiFi.begin(wifi_ssid, wifi_pass);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  #if DEBUG
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("end setup of gateway");
    #endif

  LoRa.onReceive(onReceive);
  LoRa_rxMode();
}

void loop() {

}
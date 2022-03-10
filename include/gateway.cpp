#include "WiFi.h"
#include "HTTPClient.h"
#include "ESPDateTime.h"

StaticJsonDocument<2048> json_received;


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
      WiFiClientSecure client;
      HTTPClient http;
      Serial.println("make data");
      // Data to send from json
      String httpRequestData;
      serializeJson(json_received, httpRequestData);
      // Send HTTP POST request
      Serial.print("send data : ");
      Serial.println(httpRequestData);
      client.setInsecure();
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(httpRequestData);
      String httpResponseString = http.getString();

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("HTTP Response String");
      Serial.println(httpResponseString);
        
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
    String now =  DateTime.toString();
    json_received["measured_at"] =now;    
    send_json_to_server();
  }
}





void setupDateTime() {

  DateTime.setServer("fr.pool.ntp.org");
  DateTime.setTimeZone("CET-1CEST");
  DateTime.begin();
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
  } else {
    Serial.printf("Date Now is %s\n", DateTime.toString().c_str());
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

  setupDateTime();

}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String message;
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }
    #if DEBUG
      Serial.print("Gateway Receive: ");
      Serial.println(message);
      Serial.print("RSSI :");
      Serial.println(LoRa.packetRssi());
    #endif
    parse_message(message);
  }
}
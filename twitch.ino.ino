#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

/**
 * Portuguese:
 * Detector de estado de Streams para o ESP8266
 * Este programa foi feito durante uma stream de 2h (com muito paleio á mistura)
 * Agradecimentos especiais ao icezek0
 * 
 * 
 * English:
 * Stream detector for the ESP8266
 * This program was made in a 2h coding stream (with lots of talk in the middle)
 * Special Thanks 
 * 
 */

//Zona de configurações | Config Zone 
const char* ssid = "SSID"; //Nome do teu router | Name of your router
const char* password = "PASSWORD"; // Password do teu router | Password of your router
const char* streamer = "STREAMER-NAME"; //Nome do Streamer que queiras se notificado | The Streamer that you want to be notified
const int espera = 1; //Numero de minutos de espera em cada verificação | Number of minutes for each status update
const char* ClientID = ""; //ClientID, obtem através da deste website www.glass.twitch.tv | ClientID, get from this website www.glass.twitch.tv 

//NÃO MEXER! | Do not edit 
const char* host = "api.twitch.tv";
const int httpsPort = 443;



void setup() {
  
  pinMode(2, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(2, HIGH);  // Turn the LED off by making the voltage HIGH 
}

void loop() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  
  String url = "/helix/streams?user_login=" + String(streamer);
  //String para apanhar o estado da stream
  Serial.print("requesting URL: ");
  Serial.println(url);
   client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Client-ID: " + String(ClientID) + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  
  //O nosso querido parse de JSON
  
  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(line);
  if (!root.success()){
    Serial.print("parseObject(");
    Serial.print(line);
    Serial.println(") failed");
  }else{

String streamerStatus = root["data"];

if (streamerStatus.length()>2) {  // 'on' or 'onn' ??
    digitalWrite(2, LOW);
    Serial.println("LED ON");
}
else{
    digitalWrite(2, HIGH);
    Serial.println("led OFF");
}}
  jsonBuffer.clear();
 
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
  WiFi.disconnect(); 
  delay(1000*60* espera);

}

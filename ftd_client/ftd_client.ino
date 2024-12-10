#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const int bpin = 5;  // GPIO5 (D1)
WiFiUDP Client;

bool isFirstMessage = true; // Used for our identification protocol on the server side
uint8_t selfIndex = 1; // Identifier on the server

void setup() {
  Serial.begin(115200);            
  pinMode(bpin, INPUT_PULLUP); 

}

void loop() {
  // Connect to server hotspot if not connected to it
  if (WiFi.isConnected()) {
    connectToWifi();
  }

  // Read button state
  int bstate = digitalRead(bpin); 
  if (bstate == LOW && isFirstMessage) {
    sendMessage((char*) "message");
  } else if (bstate == LOW && !isFirstMessage) {
    sendMessage((char*) &selfIndex);
  }

  delay(100);
}

void connectToWifi() {
  const char* ssid = "ESPsoftAP_01";
  const char* password = "ESP-WIFI-PW";

  WiFi.begin(ssid, password); 
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.println(++i); 
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void sendMessage(char *message) {
  int udpPort = 9999;
  Client.begin(udpPort);
  Serial.print(F("UDP Client : ")); Serial.println(WiFi.localIP());

  Client.beginPacket("192.168.4.1", udpPort);
  Serial.print("Sent : "); Serial.println(message);

  Client.write(message);
  Client.write("\r\n");
  Client.endPacket();
}

void receiveMessage() {
  // TODO: Handle message from server with UDP protocol
}

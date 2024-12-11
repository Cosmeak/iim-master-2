#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Servo.h>

// Board pins
const int bpin = 5;  // GPIO5 (D1)
Servo door;

// Communication
WiFiUDP Client;
bool isFirstMessage = true; // Used for our identification protocol on the server side
uint8_t selfIndex = 1; // Identifier on the server
char packetBuffer[255];

// Others
bool ballCanGo = true; // Used to know if we can launch the circuit

void setup() {
  Serial.begin(115200);            
  pinMode(bpin, INPUT_PULLUP);
  door.attach(4); // GPIO4 (D2)
}

void loop() {
  // Connect to server hotspot if not connected to it
  // if (!WiFi.isConnected()) {
  //   connectToWifi();
  // }

  // Check if packet is received and parse it if exists
  int packetSize = Client.parsePacket();
  if (packetSize) {
    Serial.println("A message is received.");
    int len = Client.read(packetBuffer, 255);
    if (len > 0) {
      // Circuit can be launched
      if (packetBuffer[0] == 1 && !ballCanGo) ballCanGo = true;
      // An error occured
      if (packetBuffer[0] == 0) { } // TODO: handle error 
    };
    Serial.print(packetBuffer);
    Serial.println("\n");   
  }

  // Read button state and send message to server
  int bstate = digitalRead(bpin); 
  if (bstate == LOW && isFirstMessage) {
    sendMessage((char*) &selfIndex);
  } else if (bstate == LOW && !isFirstMessage && ballCanGo) {
    sendMessage((char*) 1);
  }

  // Handle circuit events
  if (ballCanGo) {
    // Open the door and close it before detach
    door.write(8);
    delay(800);
    door.write(180);
    delay(800);
    door.detach();

    // Wait for ultrasonic sensor to detect the ball
    // TODO
  }

  delay(5000);
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

long microsecondsToCentimeters(long microseconds) {
  return (microseconds / 2) / 29.1 ;
}
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <map>

// Soft Access Point (AP)
const char* ssid = "ESPsoftAP_01";
const char* password = "ESP-WIFI-PW";

// UDP
const uint16_t udpPort = 9999;
WiFiUDP udp;

// Button
const uint8_t resetBtnPin = 2;
bool reset = false;

// Handle Data
unsigned char packetBuffer[255];

// Runtime variable
std::map<uint8_t, IPAddress> clients;
int marbleCircuitIndex = -1;

void setup() {
  Serial.begin(9600);

  Serial.println("\n\nSetup AP ESP_8266...");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed");
  Serial.printf("Soft-AP IP Address = %s\n", WiFi.softAPIP().toString().c_str());

  Serial.println("\nSetup UDP...");
  Serial.println(udp.begin(udpPort) ? "Ready" : "Failed");
  Serial.printf("UDP Port = %i\n", (int)udpPort);

  pinMode(resetBtnPin, INPUT_PULLUP);
}

void loop() {

  // Read Pkg
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 255);

    // Client id 
    IPAddress address = udp.remoteIP();
    uint8_t index;

    // First connection
    if(IsFirstConnection(address, &index))
    {
      udp.beginPacket(address, udpPort);
      if(len == 0)
        udp.write(0);
      else
      {
        index = packetBuffer[0];
        Serial.printf("\nFirst Connection : %s => %i\n\n", address.toString().c_str(), (int)index);
        clients.emplace(index, address);

        if(index == -1 || index < marbleCircuitIndex)
          marbleCircuitIndex = index;

        udp.write(1);
      }
      udp.endPacket();
    }
    else if (len > 0)
    {
      bool finishCircuit = packetBuffer[0] != 0;
      Serial.printf("Receive msg from %s (%i) : %s", address.toString().c_str(), (int)index, finishCircuit ? " Circuit Finished" : " Circuit NOT Finished");

      // Correct clients who send the pkg
      if(index == marbleCircuitIndex)
      {
        // Find client
        std::map<uint8_t, IPAddress>::iterator it = clients.find(marbleCircuitIndex);
        if(it != clients.end())
        {
          // Send receive and processed pkg
          udp.beginPacket(address, udpPort);
          udp.write(0);
          udp.endPacket();

          // Next client in line
          ++it;
            // Start the next client marble
          if(it != clients.end())
            StartMarbleCircuit(it->first, it->second);
        }
        else
        {
          for(std::map<uint8_t, IPAddress>::iterator it = clients.begin(); it != clients.end(); ++it)
          {
            if(it->first > index)
            {
              StartMarbleCircuit(it->first, it->second);
              break;
            }
          }
        }
      }
    }
  }

  // Reset btn
  if(!digitalRead(resetBtnPin))
  {
    if(!reset)
    {
      clients.clear();
      marbleCircuitIndex = -1;
      reset = true;

      Serial.println("\n[ BOARD RESET ]\n");
    }
  }
  else
    reset = false;
}

bool IsFirstConnection(const IPAddress& address, uint8_t* outIndex)
{
  for(std::map<uint8_t, IPAddress>::iterator it = clients.begin(); it != clients.end(); ++it)
  {
    if(it->second == address)
    {
      (*outIndex) = it->first;
      return false;
    }
  }

  outIndex = 0;
  return true;
}

void StartMarbleCircuit(uint8_t index, IPAddress address)
{
  marbleCircuitIndex = index;
  Serial.printf(">>> Start Marble Circuit [%i]\n", (int)index);

  udp.beginPacket(address, udpPort);
  udp.write(1);
  udp.endPacket();
}
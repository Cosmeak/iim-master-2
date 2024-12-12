#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <map>

// Design by ASM for Futur Tech Design 11/12/2024
// Free of use and modification

// ----------------------------- PROTOCOLE -----------------------------
// 1 - First Connection
// When a client send its first UDP package to the server,
// the server register the client IPAdress and its Marble
// Index (MI). The MI is in the UDP package on 1 octet (uint8_t),
// min value : 0; max value : 255.
// 
// If for a reason the UDP package is empty the server return an
// other UDP package with 1 null octet, which means an error.
//
// At this point the client wait the marble start from the server.
//
// 2 - Send Marble Start
// When a client marble circuit need to start, the server send an
// UDP package of 1 octet : 0000 0001 (the value 1 in binary).
//
// After that the server wait this same client to send the finish
// signal (see "Client Marble Finished" section).
//
// 3- Client Marble Finished
// If the client finish its marble circuit, he send a signal of 1
// octet to the server with the value 0000 0001 (the value 1 in
// binary).
//
// Then, the server can send the Marble Start signal (see "Send
// Marble Start" scetion) to the next client Marble Circuit, and
// so on.
//
// 
// PS : When the server is out of client, the board wait the user
// start the first circuit (button).
// 
// ---------------------------------------------------------------------


// Soft Access Point (AP)
const char* ssid = "ESPsoftAP_01";
const char* password = "ESP-WIFI-PW";

// UDP
const uint16_t udpPort = 9999;
WiFiUDP udp;

// Button
const uint8_t pinStartCircuit = 2;  // D4 PIN
const uint8_t pinStatusLed = 4;     // D2 PIN

// Handle Data
unsigned char packetBuffer[255];

// Runtime variable
std::map<uint8_t, IPAddress> clients; // MI and IPAdress
int marbleCircuitIndex = -1;
bool started = false;

bool ledActive = false;
bool countLedActive = true;
uint8_t clientCounterHandle = 0;

void setup()
{
  Serial.begin(9600);

  // INIT WIFI
  Serial.println("\n\nSetup AP ESP_8266...");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed");
  Serial.printf("Soft-AP IP Address = %s\n", WiFi.softAPIP().toString().c_str());

  // INIT UDP
  Serial.println("\nSetup UDP...");
  Serial.println(udp.begin(udpPort) ? "Ready" : "Failed");
  Serial.printf("UDP Port = %i\n", (int)udpPort);

  // BUTTON
  pinMode(pinStartCircuit, INPUT_PULLUP);

  // LEDs
  pinMode(pinStatusLed, OUTPUT);
  digitalWrite(pinStatusLed, LOW);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  if(!started && !digitalRead(pinStartCircuit) && clients.size() > 0)
  {
    started = true;
    std::map<uint8_t, IPAddress>::iterator start = clients.begin();
    StartMarbleCircuit(start->first, start->second);
    digitalWrite(pinStatusLed, LOW);

    digitalWrite(LED_BUILTIN, HIGH);
    bool countLedActive = true;
    uint8_t clientCounterHandle = 0;

    Serial.println(">> Circuit Begin <<\nClients :");
    for(std::map<uint8_t, IPAddress>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
      Serial.printf("\t[%i]\t%s", it->first, it->second.toString().c_str());
    }
    Serial.write('\n');
  }

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
      if(len == 0)
      {
        udp.beginPacket(address, udpPort);
        udp.write(0);
        udp.endPacket();
      }
      else
      {
        index = packetBuffer[0];
        Serial.printf("\nFirst Connection : %s => %i\n", address.toString().c_str(), (int)index);
        if(started)
        {
          if(marbleCircuitIndex < index)
            clients.emplace(index, address);
          else
          {
            // Denied
            Serial.println("ACCESS DENIED. Circuit already started");

            udp.beginPacket(address, udpPort);
            udp.write(0);
            udp.endPacket();
          }
        }
        else
        {
          clients.emplace(index, address);
          digitalWrite(pinStatusLed, HIGH);

          if(index == -1 || index < marbleCircuitIndex)
            marbleCircuitIndex = index;
        }
        Serial.write('\n');
      }
    }
    else if (len > 0 && started)
    {
      bool finishCircuit = packetBuffer[0] != 0;
      Serial.printf("Receive msg from %s (%i) : %s\n", address.toString().c_str(), (int)index, finishCircuit ? " Circuit Finished" : " Circuit NOT Finished");

      // Correct clients who send the pkg
      if(index == marbleCircuitIndex)
      {
        // Find client
        std::map<uint8_t, IPAddress>::iterator it = clients.find(marbleCircuitIndex);
        if(it != clients.end())
        {
          // Next client in line
          ++it;
          // Start the next client marble
          if(it != clients.end())
            StartMarbleCircuit(it->first, it->second);
          else
          {
            Serial.println(">> Marble Circuit Finished <<\n");

            marbleCircuitIndex = clients.begin()->first;
            started = false;
            ledActive = false;
          }
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

  // Blink Led
  if(started)
  {
    ledActive = !ledActive;
    digitalWrite(pinStatusLed, ledActive);
  }
  else if(clients.size() > 0)
  {
    if(clientCounterHandle >= 20)
    {
      countLedActive = !countLedActive;
      digitalWrite(LED_BUILTIN, countLedActive);
      Serial.println("BLINK");

      if((clientCounterHandle - 20) > (3 * 2))
      {
        clientCounterHandle = 0;
        Serial.println("\n");
      }
    }
    ++clientCounterHandle;
  }

  delay(150);
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
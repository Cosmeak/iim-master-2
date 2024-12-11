# rube_goldberg_machine

# Documentation technique

## Introduction

Ce projet simule une machine de Rube Goldberg, une chaîne de réactions mécaniques comprenant des éléments interactifs en réseau tel que des enceinte, leds, ou autre . Les microcontrôleurs ESP8266 jouent un rôle central pour coordonner certaines étapes via un réseau Wi-Fi. Chaque action si nécessaire est déclenchée en chaîne, selon un ordre géré par le serveur.
Chaque étape du mécanisme déclenche la suivante, illustrant un processus complexe pour accomplir une tâche simple.

## Composants utilisés

- ESP8266 : Un pour le serveur, et plusieurs pour les potentiels clients.
- Éléments mécaniques : Ils traduisent les actions des ESP8266 en mouvements physiques.
- Chemins en pailles : Guident les billes entre les étapes.
- Bille en métal : Élément déclencheur principal.

## Schéma de câblage

#### ESP8266 Serveur :

#### ESP8266 Clients :

## Code Arduino

### Explication du code

#### Code serveur `ftd_server.ino` :

#### Résumé :

- Initialisation du réseau Wi-Fi : L’ESP8266 serveur crée un réseau Wi-Fi local.
- Gestion des clients : Il détecte les clients qui se connectent, les enregistre, et coordonne leurs actions via UDP.
- Transmission des messages : Envoie des signaux pour démarrer ou passer à l'étape suivante.

code :

```c
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <map>

const char* ssid = "ESPsoftAP_01";
const char* password = "ESP-WIFI-PW";
const uint16_t udpPort = 9999;
WiFiUDP udp;
const uint8_t resetBtnPin = 2;
bool reset = false;
unsigned char packetBuffer[255];
std::map<uint8_t, IPAddress> clients;
int marbleCircuitIndex = -1;

void setup() {
  Serial.begin(9600);
  WiFi.softAP(ssid, password);
  udp.begin(udpPort);
  pinMode(resetBtnPin, INPUT_PULLUP);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 255);
    IPAddress address = udp.remoteIP();
    uint8_t index;

    if (IsFirstConnection(address, &index)) {
      clients.emplace(index, address);
      marbleCircuitIndex = index;
      StartMarbleCircuit(index, address);
    } else if (len > 0 && index == marbleCircuitIndex) {
      std::map<uint8_t, IPAddress>::iterator it = clients.find(marbleCircuitIndex);
      if (it != clients.end()) {
        udp.beginPacket(address, udpPort);
        udp.write(0);
        udp.endPacket();
        ++it;
        if (it != clients.end()) StartMarbleCircuit(it->first, it->second);
      }
    }
  }

  if (!digitalRead(resetBtnPin)) {
    if (!reset) {
      clients.clear();
      marbleCircuitIndex = -1;
      reset = true;
    }
  } else reset = false;
}

bool IsFirstConnection(const IPAddress& address, uint8_t* outIndex) {
  for (auto& client : clients) {
    if (client.second == address) {
      *outIndex = client.first;
      return false;
    }
  }
  *outIndex = 0;
  return true;
}

void StartMarbleCircuit(uint8_t index, IPAddress address) {
  marbleCircuitIndex = index;
  udp.beginPacket(address, udpPort);
  udp.write(1);
  udp.endPacket();
}
```

#### Explication des fonctions principales :

`setup()`:

Configure l'ESP8266 en tant que point d'accès Wi-Fi.

Initialise le protocole UDP pour la communication.

`loop()`:

Vérifie les messages UDP entrants.

Enregistre de nouveaux clients s'ils ne sont pas déjà dans la liste.

Si un message vient du client actuel, passe au client suivant et envoie des instructions.

Surveille le bouton de réinitialisation pour effacer la liste des clients et réinitialiser le système.

`IsFirstConnection()`:

Vérifie si une adresse IP est déjà enregistrée comme client.

Retourne un index unique pour chaque nouveau client.

`StartMarbleCircuit()`:

Envoie une commande au client concerné pour démarrer une nouvelle étape du circuit.

#### Code client `ftd_client.ino` :

#### Résumé :

- Connexion au serveur : L’ESP8266 client se connecte au réseau Wi-Fi du serveur.
- Exécution des actions : Il attend un signal pour déclencher une action mécanique (moteur, led, enceinte, etc.)

code :

```c
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

const int bpin = 5;
WiFiUDP Client;
bool isFirstMessage = true;
uint8_t selfIndex = 1;

void setup() {
  Serial.begin(115200);
  pinMode(bpin, INPUT_PULLUP);
}

void loop() {
  if (WiFi.isConnected()) connectToWifi();

  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len - 1] = 0;
    Serial.print(packetBuffer);
  }

  int bstate = digitalRead(bpin);
  if (bstate == LOW && isFirstMessage) sendMessage((char*) "message");
  else if (bstate == LOW && !isFirstMessage) sendMessage((char*) &selfIndex);

  delay(100);
}

void connectToWifi() {
  const char* ssid = "ESPsoftAP_01";
  const char* password = "ESP-WIFI-PW";

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(1000);
}

void sendMessage(char *message) {
  int udpPort = 9999;
  Client.begin(udpPort);
  Client.beginPacket("192.168.4.1", udpPort);
  Client.write(message);
  Client.write("\r\n");
  Client.endPacket();
}
```

#### Explication des fonctions principales :

`setup()`:

Configure le client ESP8266 et prépare le port UDP pour la communication.

`loop()`:

Vérifie l'arrivée de messages UDP.

Lit l'état du bouton pour envoyer un message au serveur et participer à la chaîne d'actions.

`connectToWifi()`:

Connecte le client ESP8266 au réseau Wi-Fi du serveur.

`sendMessage()`:

Envoie un message UDP au serveur pour indiquer un état ou déclencher une action.

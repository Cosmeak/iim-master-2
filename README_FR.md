# rube_goldberg_machine

# Documentation technique

## Introduction

Ce projet simule une machine de Rube Goldberg, une chaîne de réactions mécaniques comprenant des éléments interactifs en réseau tel que des enceinte, leds, ou autre . Les microcontrôleurs ESP8266 jouent un rôle central pour coordonner certaines étapes via un réseau Wi-Fi. Chaque action si nécessaire est déclenchée en chaîne, selon un ordre géré par le serveur.
Chaque étape du mécanisme déclenche la suivante, illustrant un processus complexe pour accomplir une tâche simple.

## Composants utilisés

- ESP8266 : Un pour le serveur, et plusieurs pour les potentiels clients.
- Éléments mécaniques : Ils traduisent les actions des ESP8266 en mouvements physiques.
-  Capteur à ultrasons (HC-SR04)
- Servomoteur
- haut parleur 
- Chemins en pailles : Guident les billes entre les étapes.
- Bille en métal : Élément déclencheur principal.

## Schéma de câblage

une esp8266 se présente comme ceci au début:

![esp_shema](assets/esp-schema.png)

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
- Gestion des messages : Le client reçoit des messages du serveur pour lancer une action ou signaler une erreur.
- Exécution des actions : Le client contrôle un servo moteur (de la porte) pour gérer la bille et attend les événements liés au circuit.

code :

```c
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Servo.h>

const int bpin = 5; 
Servo door;

WiFiUDP Client;
bool isFirstMessage = true;
uint8_t selfIndex = 1; 
char packetBuffer[255];

bool ballCanGo = true;

void setup() {
  Serial.begin(115200);            
  pinMode(bpin, INPUT_PULLUP);
  door.attach(4);
}

void loop() {
  int packetSize = Client.parsePacket();
  if (packetSize) {
    Serial.println("A message is received.");
    int len = Client.read(packetBuffer, 255);
    if (len > 0) {
      if (packetBuffer[0] == 1 && !ballCanGo) ballCanGo = true;
      if (packetBuffer[0] == 0) { }
    };
    Serial.print(packetBuffer);
    Serial.println("\n");   
  }

  int bstate = digitalRead(bpin); 
  if (bstate == LOW && isFirstMessage) {
    sendMessage((char*) &selfIndex);
  } else if (bstate == LOW && !isFirstMessage && ballCanGo) {
    sendMessage((char*) 1);
  }

  if (ballCanGo) {
    door.write(8);
    delay(800);
    door.write(180);
    delay(800);
    door.detach();
    ballCanGo = false;
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
```

#### Explication des fonctions principales :

```setup()``` :
- Configure l'ESP8266 client.
- Initialise le servo moteur (door) attaché à la broche GPIO4 (D2).
- Prépare le port UDP pour la communication.

```loop()``` :
- Vérifie les messages reçus via UDP :
  - Si le message indique que le circuit peut être lancé (packetBuffer[0] == 1), active ballCanGo.
  - Gère les erreurs si un message de type erreur (packetBuffer[0] == 0) est reçu.

- Lit l'état du bouton connecté à GPIO5 (D1) :
  - Envoie un message d’identification si c’est le premier message (```isFirstMessage```).
  - Envoie un signal de lancement du circuit si tout est prêt (```ballCanGo```).

- Si ```ballCanGo``` est actif :
  - Contrôle le servo moteur pour ouvrir et fermer une porte, permettant à la bille de passer.
  - Désactive ballCanGo pour éviter des répétitions non voulues.
  
```connectToWifi()``` :
- Connecte le client ESP8266 au réseau Wi-Fi du serveur.

```sendMessage()``` :
- Envoie un message UDP au serveur pour indiquer un état ou déclencher une action.


## Mechanisme de fonctionnement du circuit :

- la porte bascule pour laisser passer la bille
- la bille se déplace dans le circuit en passant par les pailles
- la bille tombe dans un récipient , qui fait office de frein 
- la bille passe ensuite dans un gobelet qui bascule lorsque la bille passe dedans
- la bille tombe das une partie métallique en pente, fait un virage vers un chemin de paille 
- la bille arrive à une plaque de métallique, la bille étant en métal, elle est conductrice, elle ferme le circuit et déclancer le haut parleur configuré pour émettre un son de noël, chaque note correspond à une fréquence comme on l'a vu dans le code ci-dessus.

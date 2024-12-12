# rube_goldberg_machine

# Documentation technique

## Introduction

Ce projet simule une machine de Rube Goldberg, une chaîne de réactions mécaniques comprenant des éléments interactifs en réseau tel qu'une enceinte,bouton ou autre. Les microcontrôleurs ESP8266 jouent un rôle central pour coordonner certaines étapes via un réseau Wi-Fi. Chaque action si nécessaire est déclenchée en chaîne, selon un ordre géré par le serveur.
Chaque étape du mécanisme déclenche la suivante, illustrant un processus complexe pour accomplir une tâche simple.

## Composants utilisés

- ESP8266 : Un pour le serveur, et plusieurs pour les potentiels clients.
- Bille en métal : Élément déclencheur principal.
- Chemins en pailles : Guident les billes entre les étapes.
- Éléments mécaniques : Ils traduisent les actions des ESP8266 en mouvements physiques.
-  Capteur à ultrasons (HC-SR04)
- Servomoteur
- Plaque métallique : Utilisée pour détecter la bille et déclencher le buzzer.
- haut parleur (buzzer connecté)

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
- Exécution des actions :
  - Le client contrôle un servo moteur (porte) pour gérer la bille.
  - Utilise des capteurs ultrasoniques pour détecter la bille et déclencher d’autres actions, comme jouer de la musique

code :

```c
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Servo.h>

const int bpin = 5; 
const int musicSensors[3][2] = {
  { 2, 0 }, 
  { 12, 13 }, 
};

WiFiUDP Client;
bool isFirstMessage = true;  
uint8_t selfIndex = 1;       
char packetBuffer[255];

Servo door;
bool ballCanGo = true;  

void setup() {
  Serial.begin(115200);
  pinMode(bpin, INPUT_PULLUP);
  door.attach(4); 
  for (auto& sensorPin : musicSensors) {
    pinMode(sensorPin[0], OUTPUT);
    pinMode(sensorPin[1], INPUT);
  }
}

void loop() {
  if (!WiFi.isConnected()) {
    connectToWifi();
  }

  int packetSize = Client.parsePacket();
  if (packetSize) {
    Serial.println("A message is received.");
    int len = Client.read(packetBuffer, 255);
    if (len > 0) {
      if (packetBuffer[0] == 1 && !ballCanGo) ballCanGo = true;
      if (packetBuffer[0] == 0) {}  
    };
    Serial.print(packetBuffer);
    Serial.println("\n");
  }

  int bstate = digitalRead(bpin);
  if (bstate == LOW && !isFirstMessage && ballCanGo) sendMessage((char*)1);

  if (ballCanGo) {
    if (door.attached()) {
      door.write(8);
      delay(800);
      door.write(180);
      delay(800);
      door.detach();
    }

    int index = 0;
    for (auto& sensorPin : musicSensors) {
      digitalWrite(sensorPin[0], LOW);
      delay(10);
      digitalWrite(sensorPin[0], HIGH);
      delay(10);
      digitalWrite(sensorPin[0], LOW);

      long duration = pulseIn(sensorPin[1], HIGH);
      long distance = microsecondsToCentimeters(duration);
      if (distance < 10) {
        if (index == 0) playMusic();
        if (index == 1) {}  
      }
      index++;
    }
  }
  delay(500);
}

void connectToWifi() {
  const char* ssid = "ESPsoftAP_01";
  const char* password = "ESP-WIFI-PW";

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(++i);
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  if (isFirstMessage) sendMessage((char*)&selfIndex);
}

void sendMessage(char* message) {
  int udpPort = 9999;
  Client.begin(udpPort);
  Serial.print(F("UDP Client : "));
  Serial.println(WiFi.localIP());

  Client.beginPacket("192.168.4.1", udpPort);
  Serial.print("Sent : ");
  Serial.println(message);

  Client.write(message);
  Client.write("\r\n");
  Client.endPacket();
}

long microsecondsToCentimeters(long microseconds) {
  return (microseconds / 2) / 29.1;
}

void playMusic() {
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;

  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = pgm_read_word_near(melody + thisNote + 1);
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    tone(buzzer, pgm_read_word_near(melody + thisNote), noteDuration * 0.9);
    delay(noteDuration);
    noTone(buzzer);
  }
}
```

#### Explication des fonctions principales :

```setup()``` :
- Configure l'ESP8266 client.
- Initialise le servomoteur attaché à GPIO4 (D2) pour ouvrir et fermer la porte.
- Configure les capteurs ultrasoniques connectés (GPIO2, GPIO12, etc.) pour détecter la bille.
- Configure le bouton connecté à GPIO5 (D1) pour surveiller son état, afin de déterminer la progression dans le circuit.

```loop()``` :

***- Vérifie les messages UDP*** :
- Si un message valide est reçu du serveur (valeur de ```packetBuffer[0] == 1```), active ```ballCanGo```, permettant de déclencher les événements du circuit.
- Gère les messages d’erreur (placeholder pour ```packetBuffer[0] == 0```).

***- Lit l'état du bouton*** :
- Si le bouton est pressé (```bstate == LOW```), envoie un message au serveur pour signaler la progression du circuit.

***- Contrôle les événements du circuit*** :
- Si ```ballCanGo``` est actif :
  - Ouvre et ferme la porte avec le servomoteur.
  - Utilise les capteurs ultrasoniques pour détecter la bille :
    - Si la bille est détectée à une distance inférieure à 10 cm, déclenche des actions spécifiques comme jouer une mélodie via le buzzer.

```connectToWifi()``` :
- Connecte le client ESP8266 au réseau Wi-Fi configuré.
- Envoie un message d’identification au serveur après une connexion réussie.

```sendMessage()``` :
- Envoie des messages UDP au serveur pour signaler un état ou une progression dans le circuit.

```playMusic()``` :
- Joue une mélodie de Noël avec un buzzer connecté.
- Chaque note correspond à une fréquence précise, définie dans le tableau ```melody``` du code.

## Mechanisme de fonctionnement du circuit :

**Ouverture de la porte** :
La porte bascule grâce à un servomoteur, permettant à la bille de commencer son parcours.

**Déplacement dans le circuit** :
La bille se déplace à travers un chemin constitué de pailles, guidée par la pente du circuit.

**Freinage dans un récipient** :
La bille tombe dans un récipient qui agit comme un frein temporaire, ralentissant sa vitesse.

**Activation du gobelet basculant** :
La bille passe dans un gobelet conçu pour basculer lorsqu'elle y entre, libérant ainsi la bille vers la prochaine étape.

**Passage sur une pente métallique** :
La bille continue son chemin sur une pente métallique en réalisant un virage pour rejoindre un nouveau segment de pailles.

**Détection sur une plaque métallique** :
La bille arrive sur une plaque métallique, où elle poursuit son chemin jusqu'à activer un bouton connecté. Lorsque la bille appuie sur ce bouton, cela déclenche :

- Déclenche une fonction signalant que le circuit est fermé.
- Active un buzzer configuré pour jouer une mélodie de Noël. Chaque note de cette mélodie correspond à une fréquence, comme défini dans le code.

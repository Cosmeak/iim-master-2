# rube_goldberg_machine

# Documentation technique

## Introduction

Ce projet simule une machine de Rube Goldberg, une chaîne de réactions mécaniques comprenant des éléments interactifs en réseau tel  que des capteurs à ultrason,une led,une enceinte,un bouton. Les microcontrôleurs ESP8266 jouent un rôle central pour coordonner certaines étapes via un réseau Wi-Fi. Chaque action si nécessaire est déclenchée en chaîne, selon un ordre géré par le serveur.
Chaque étape du mécanisme déclenche la suivante, illustrant un processus complexe pour accomplir une tâche simple.

## Composants utilisés

- ESP8266 : Un pour le serveur, et plusieurs pour les potentiels clients.
- Bille en métal : Élément déclencheur principal.
- Chemins en pailles : Guident les billes entre les étapes.
- Éléments mécaniques : Ils traduisent les actions des ESP8266 en mouvements physiques.
-  Capteur à ultrasons (HC-SR04)
- une led 
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
- Gestion des clients :
  - Le serveur détecte les clients qui se connectent, les enregistre avec un identifiant unique et gère leur progression dans le circuit.
  - Contrôle l'accès des nouveaux clients si un circuit est déjà en cours.
- Transmission des messages :
  - Envoie un signal pour démarrer le circuit d’un client.
  - Attend un signal de fin de circuit pour passer au client suivant.
  - Réinitialise l'état lorsqu’un bouton est pressé.


code :

voir ```ftd_server.ino```

#### Explication des fonctions principales :

`setup()`:
- Configure l'ESP8266 en mode point d'accès Wi-Fi.
- Initialise le protocole UDP pour la communication avec les clients.
- Configure les broches pour le bouton de démarrage et les LED de statut.

`loop()`:

- Vérifie si un bouton de démarrage est pressé lorsque des clients sont connectés :
  - Démarre le circuit en envoyant une commande au premier client de la liste.
  - Affiche les clients connectés et leurs adresses IP dans le moniteur série.
- Gère les messages UDP entrants :
  - Enregistre un nouveau client si une connexion initiale est détectée.
  - Passe au client suivant si un client signale la fin de son circuit.
  - Affiche les messages reçus et leurs expéditeurs dans le moniteur série.
- Gère le clignotement des LED :
  - La LED de statut clignote pendant qu’un circuit est en cours.
  - La LED intégrée clignote lorsqu'il y a des clients connectés mais qu’aucun circuit n’est actif.
- Réinitialise le serveur lorsque le bouton de réinitialisation est pressé.

`IsFirstConnection()`:

- Vérifie si une adresse IP est déjà enregistrée comme client.

- Retourne un index unique pour chaque nouveau client.

`StartMarbleCircuit()`:
- Envoie une commande au client identifié pour démarrer une nouvelle étape du circuit.
- Met à jour l'index du client en cours dans la liste.

#### Code client `ftd_client.ino` :

#### Résumé :

- Connexion au serveur : L’ESP8266 client se connecte au réseau Wi-Fi du serveur.
- Gestion des messages : Le client reçoit des messages du serveur pour lancer une action ou signaler une erreur.
- Exécution des actions :
  - Le client contrôle un servo moteur (porte) pour gérer la bille.
  - Utilise des capteurs ultrasoniques pour détecter la bille et déclencher d’autres actions :
  - Jouer de la musique via un buzzer.
  - Activer un mécanisme "dansant" (servo moteurs supplémentaires).
  - Allumer une LED pour signaler le passage de la bille.

code :

voir ```ftd_client.ino```

#### Explication des fonctions principales :

```setup()``` :
- Configure l'ESP8266 client.
- Initialise le servomoteur attaché à GPIO4 (D2) pour ouvrir et fermer la porte.
Initialise deux autres servos moteurs :
  - GPIO14 (D5) : Contrôle la rotation d'un mécanisme simulant un hélicoradian (mouvement rotatif).
  - GPIO16 (D0) : Contrôle le mouvement vertical du mécanisme.
- Configure les capteurs ultrasoniques connectés (GPIO2, GPIO12, etc.) pour détecter la bille à différents endroits du parcours.
- Configure le bouton connecté à GPIO5 (D1) pour surveiller son état et signaler que le parcours est terminé.

```loop()```:
- Vérifie les messages UDP :

  - Si un message valide est reçu du serveur (valeur de packetBuffer[0] == 1), active ballCanGo, permettant de déclencher les événements du circuit.
  - Gère les messages d’erreur (placeholder pour packetBuffer[0] == 0).


- Lit l'état du bouton :
  - Si le bouton est pressé (```bstate == LOW```), envoie un message au serveur pour signaler que le parcours est terminé.
- Contrôle les événements du circuit :

- Si ```ballCanGo``` est actif :
  - Ouvre et ferme la porte avec le servomoteur pour laisser passer la bille.
  - Utilise les capteurs ultrasoniques pour détecter la bille à différentes étapes :
    - Capteur 1 : Détecte la bille lorsqu’elle est détectée à une distance inférieure à 10 cm et: 
      - Déclenche la musique via le buzzer.
      - Active le mouvement des deux nouveaux servos moteurs :
        - Servo heliup (mouvement vertical) : Passe à 180°.
        - Servo helispin (mouvement rotatif) : Passe à 8°.
    - Capteur 2 : Détecte la bille sur une pente métallique et allume une LED.

```connectToWifi()``` :
- Connecte le client ESP8266 au réseau Wi-Fi configuré.
- Envoie un message d’identification au serveur après une connexion réussie.

```sendMessage()``` :
- Envoie des messages UDP au serveur pour signaler un état ou une progression dans le circuit.


```playMusic()``` :
- Joue une mélodie de Noël avec un buzzer connecté.
- Chaque note correspond à une fréquence précise, définie dans le tableau ```melody``` du code.

## Mechanisme de fonctionnement du circuit :

***Ouverture de la porte*** : 
La porte bascule grâce à un servomoteur, permettant à la bille de commencer son parcours.

***Déplacement dans le circuit*** : La bille se déplace à travers un chemin constitué de pailles, guidée par la pente du circuit.

***Freinage dans un récipient*** :
- Détection via le capteur 1 :

  - Lorsque la bille est détectée à une distance inférieure à 10 cm :
    - Le buzzer joue une mélodie de Noël.
    - Le mécanisme de l’hélicoradian effectue un mouvement vertical (servo heliup) et une rotation (servo helispin).

- La bille tombe dans un récipient qui agit comme un frein temporaire, ralentissant sa vitesse.

***Activation du gobelet basculant*** : La bille passe dans un gobelet conçu pour basculer lorsqu'elle y entre, libérant ainsi la bille vers la prochaine étape.

***Passage sur une pente métallique*** : La bille continue son chemin sur une pente métallique en réalisant un virage pour rejoindre un nouveau segment de pailles.

  - Capteur 2 : Avant d'atteindre le récipient, la bille est détectée à une distance inférieure à 10 cm, déclenche l'allumage d'une LED.

***Détection sur une plaque métallique*** : La bille arrive sur une plaque métallique et appuie sur un bouton connecté. Cela :

- Envoie un message au serveur pour signaler que le parcours est terminé.
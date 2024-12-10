# rube_goldberg_machine


# Technical documentation : 

## English version 🇬🇧


## Introduction

## French version 🇫🇷

## Introduction

Ce projet simule une machine de Rube Goldberg, une chaîne de réactions mécaniques comprenant des éléments interactifs  en réseau. Les microcontrôleurs ESP8266 jouent un rôle central pour coordonner certaines étapes via un réseau Wi-Fi. Chaque action si nécessaire est déclenchée en chaîne, selon un ordre géré par le serveur.
Chaque étape du mécanisme déclenche la suivante, illustrant un processus complexe pour accomplir une tâche simple.

## Composants utilisés

- ESP8266 : Un pour le serveur, et plusieurs pour les potentielsclients.
- Éléments mécaniques : Ils traduisent les actions des ESP8266 en mouvements physiques.
- Chemins en pailles : Guident les billes entre les étapes.
- Bille en métal : Élément déclencheur principal.

## Schéma de câblage

#### ESP8266 Serveur :



#### ESP8266 Clients :



## Code Arduino

### Explication du code

#### Code serveur ```ftd_server.ino``` :

- Initialisation du réseau Wi-Fi : L’ESP8266 serveur crée un réseau Wi-Fi local.
- Gestion des clients : Il détecte les clients qui se connectent, les enregistre, et coordonne leurs actions via UDP.
- Transmission des messages : Envoie des signaux pour démarrer ou passer à l'étape suivante.

#### Code client ```ftd_client.ino``` :

- Connexion au serveur : L’ESP8266 client se connecte au réseau Wi-Fi du serveur.
- Exécution des actions : Il attend un signal pour déclencher une action mécanique .






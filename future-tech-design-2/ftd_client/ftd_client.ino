#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Servo.h>

// Board pins
const int bpin = 5;  // GPIO5 (D1)
const int musicSensors[3][2] = {
  // <trigger, echo>
  { 2, 0 },    // GPIO2 (D3) / GPIO0 (D4)
  { 12, 13 },  // GPIO12 (D6) / GPIO13 (D7)
  // { 16, 14 }   // GPIO16 (D0) / GPIO14 (D5)
}; 

// Communication
WiFiUDP Client;
bool isFirstMessage = true;  // Used for our identification protocol on the server side
uint8_t selfIndex = 1;       // Identifier on the server
char packetBuffer[255];

// Others
Servo door;
Servo heliup;
Servo helispin;
bool ballCanGo = false;  // Used to know if we can launch the circuit

void setup() {
  Serial.begin(115200);
  pinMode(bpin, INPUT_PULLUP);
  door.attach(4);  // GPIO4 (D2)
  helispin.attach(14);  // GPIO4 (D5)
  heliup.attach(16);  // GPIO4 (D0)
  heliup.write(0); // Come back to default position
  // Set input mode for all ultrasound sensors
  for (auto& sensorPin : musicSensors) {
    pinMode(sensorPin[0], OUTPUT);
    pinMode(sensorPin[1], INPUT);
  }
}

void loop() {
  // Connect to server hotspot if not connected to it
  if (!WiFi.isConnected()) {
    connectToWifi();
  }

  // Check if packet is received and parse it if exists
  int packetSize = Client.parsePacket();
  if (packetSize) {
    Serial.println("A message is received.");
    int len = Client.read(packetBuffer, 255);
    if (len > 0) {
      // Circuit can be launched
      if (packetBuffer[0] == 1 && !ballCanGo) ballCanGo = true;
      // An error occured
      if (packetBuffer[0] == 0) {}  // TODO: handle error
    };
    Serial.print(packetBuffer);
    Serial.println("\n");
  }

  // Read button state and send to the server that the circuit is finished
  int bstate = digitalRead(bpin);
  if (bstate == LOW && !isFirstMessage && ballCanGo) {
    Serial.println("Ending.");
    heliup.detach();
    helispin.detach();
    sendMessage((char*)1);
  };

  // Handle circuit events
  if (ballCanGo) {
    // Open the door and close it before detach
    if (door.attached()) {
      Serial.println("Can go!");
      Serial.println("Open the door.");
      door.write(180);
      delay(800);
      door.write(8);
      delay(800);
      door.detach();
    }

    // Wait for ultrasonic sensor to detect the ball and play a sound
    int index = 0;
    for (auto& sensorPin : musicSensors) {
      digitalWrite(sensorPin[0], LOW);
      delay(10);
      digitalWrite(sensorPin[0], HIGH);
      delay(10);
      digitalWrite(sensorPin[0], LOW);

      long duration = pulseIn(sensorPin[1], HIGH);
      long distance = microsecondsToCentimeters(duration);
      if (distance < 1) {
        // Play a sound and make dancing helicoridian
        if (index == 0) {
          heliup.write(180);
          helispin.write(8);
          playMusic();
        }
        // TODO: turn on xmas led
        else if (index == 1) {}  
      }

      index++;
    }
  } else {
    Serial.println("Waiting...");
  }
  delay(1000);
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

  if (isFirstMessage) {
    sendMessage((char*)&selfIndex);
    isFirstMessage = false;
  };
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

/* 
  We Wish You a Merry Christmas - Traditional Christmas song
  Connect a piezo buzzer or speaker to pin 11 or select a new pin.
  More songs available at https://github.com/robsoncouto/arduino-songs
  Robson Couto, 2019
*/
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978
#define REST 0


// change this to make the song slower or faster
int tempo = 140;

// change this to whichever pin you want to use
int buzzer = 15;
const int melody[] PROGMEM = {

  // We Wish You a Merry Christmas
  // Score available at https://musescore.com/user/6208766/scores/1497501

  NOTE_C5, 4,  //1
  NOTE_F5, 4, NOTE_F5, 8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5, 8,
  NOTE_D5, 4, NOTE_D5, 4, NOTE_D5, 4,
  NOTE_G5, 4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5, 8, NOTE_F5, 8,
  NOTE_E5, 4, NOTE_C5, 4, NOTE_C5, 4,
  NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,
  NOTE_F5, 4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5, 8,
  NOTE_D5, 4, NOTE_G5, 4, NOTE_E5, 4, NOTE_F5,2,

  // NOTE_C5,4, //8
  // NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
  // NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
  // NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  // NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
  // NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
  // NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
  // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  // NOTE_F5,2, NOTE_C5,4,

  // NOTE_F5,4, NOTE_F5,4, NOTE_F5,4,//17
  // NOTE_E5,2, NOTE_E5,4,
  // NOTE_F5,4, NOTE_E5,4, NOTE_D5,4,
  // NOTE_C5,2, NOTE_A5,4,
  // NOTE_AS5,4, NOTE_A5,4, NOTE_G5,4,
  // NOTE_C6,4, NOTE_C5,4, NOTE_C5,8, NOTE_C5,8,
  // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  // NOTE_F5,2, NOTE_C5,4,
  // NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
  // NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,

  // NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8, //27
  // NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
  // NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
  // NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
  // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  // NOTE_F5,2, NOTE_C5,4,
  // NOTE_F5,4, NOTE_F5,4, NOTE_F5,4,
  // NOTE_E5,2, NOTE_E5,4,
  // NOTE_F5,4, NOTE_E5,4, NOTE_D5,4,

  // NOTE_C5,2, NOTE_A5,4,//36
  // NOTE_AS5,4, NOTE_A5,4, NOTE_G5,4,
  // NOTE_C6,4, NOTE_C5,4, NOTE_C5,8, NOTE_C5,8,
  // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  // NOTE_F5,2, NOTE_C5,4,
  // NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
  // NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
  // NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  // NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,

  // NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,//45
  // NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
  // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  // NOTE_F5,2, NOTE_C5,4,
  // NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
  // NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
  // NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  // NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,

  // NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8, //53
  // NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
  // NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  // NOTE_F5,2, REST,4
};


// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

void playMusic() {
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = pgm_read_word_near(melody + thisNote + 1);
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;  // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, pgm_read_word_near(melody + thisNote), noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);
  }
}
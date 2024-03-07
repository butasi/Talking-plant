#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

const int AirValue = 420;    // Replace with actual AirValue
const int WaterValue = 190;  // Replace with actual WaterValue

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);  // Open serial port, set the baud rate to 115200 bps

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  // Use softwareSerial to communicate with MP3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Please recheck the connection!"));
    Serial.println(F("2. Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));

  // Set initial volume
  myDFPlayer.volume(30);
}

void loop() {
  static unsigned long timer = millis();
  static unsigned long replayTimer = millis();  // Timer for replaying the first MP3
  static bool playedAudio1 = false;             // Flag to track if audio 1 has been played
  static bool playedAudio2 = false;             // Flag to track if audio 2 has been played
  static bool playedAudio3 = false;             // Flag to track if audio 3 has been played
  static int prevSoilMoisturePercent = 0;       // Previous soil moisture percentage

  unsigned long currentMillis = millis();

  // Check soil moisture every loop iteration
  int soilMoistureValue = analogRead(A0);
  Serial.println(soilMoistureValue);

  int soilMoisturePercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  // Detect water addition
  int moistureChange = soilMoisturePercent - prevSoilMoisturePercent;
  if (moistureChange > 10) {  // Adjust threshold as needed
    // Water addition detected, play audio 4
    myDFPlayer.play(4);
  }

  prevSoilMoisturePercent = soilMoisturePercent;

  if (soilMoisturePercent >= 100 && !playedAudio2) {
    Serial.println("100 %");
    // If soil moisture reaches 100% and audio 2 has not been played yet, play audio 2
    myDFPlayer.play(2);
    playedAudio2 = true;
  } else if (soilMoisturePercent >= 100 && !playedAudio3) {
    Serial.println("100 %");
    // If soil moisture reaches 100% again after dropping below it, and audio 3 has not been played yet, play audio 3
    myDFPlayer.play(3);
    playedAudio3 = true;
  } else if (soilMoisturePercent < 100) {
    // If soil moisture drops below 100%, reset the flags to allow playing audio 2 or audio 3 again
    playedAudio2 = false;
    playedAudio3 = false;
  }

  if (soilMoisturePercent <= 0 && !playedAudio1) {
    Serial.println("0 %");
    // If soil moisture is 0% and audio 1 has not been played yet, play audio 1
    myDFPlayer.play(1);
    playedAudio1 = true;
  } else if (soilMoisturePercent > 0) {
    // If soil moisture increases above 0%, reset the flag to allow playing audio 1 again
    playedAudio1 = false;
  } else if (soilMoisturePercent > 0 && soilMoisturePercent < 100) {
  }
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisturePercent);
  Serial.println("%");

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());  // Print the detail message from DFPlayer to handle different errors and states.
  }

  delay(1000);  // Delay to stabilize the sensor readings
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

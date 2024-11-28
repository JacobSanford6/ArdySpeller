#include "Arduino_LED_Matrix.h"
#include <ArduinoBLE.h>
#include <vector>
#include "letters.h"

using namespace std;

ArduinoLEDMatrix matrix;

BLEService service("19b10000-e8f2-537e-4f6c-d104768a1214");
BLEStringCharacteristic wordCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 100);

byte frame[8][12]  = {
  { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void clearFrame(){
  for (int i = 0; i <= 8; i++){
    for (int ii=0; ii <= 12; ii++){
      frame[i][ii] = 0;
    }
  }
}

void renderWord(string word){
  clearFrame();
  matrix.renderBitmap(frame, 8, 12);
  delay(500);

  int last = -1;
  for (int i = 0; i < word.length(); i++){
    char c = std::tolower(word[i]);
    // if the same letter is used many times in a row, flash it
    if (c == last){
      clearFrame();
      matrix.renderBitmap(frame, 8, 12);
      delay(200);
    }

    // If c == 32, it is a space, so display nothing
    if (c == 32){
      clearFrame();
      matrix.renderBitmap(frame, 8, 12);
      delay(500);
    }else{
      // render any letter in the alphabet
      matrix.loadFrame(letters[c-97]);
    }
    last = c;
    delay(350);
  }
  delay(150);
}

void setup() {
  Serial.begin(9600);
  matrix.begin();
  BLE.begin();
  BLE.setAdvertisedService(service);
  service.addCharacteristic(wordCharacteristic);
  BLE.setLocalName("BT Word Service");
  BLE.setDeviceName("BT Word LED");
  BLE.addService(service);
  BLE.advertise();
}

void loop() {
  BLE.setDeviceName("BT Word LED");
  BLEDevice central = BLE.central();

  if (central){
    if (central.connected()){
      clearFrame();
      matrix.renderBitmap(frame, 8, 12);

      while (central.connected()){
        string wordStr = string(wordCharacteristic.value().c_str());
        renderWord(wordStr);
        delay(500);
      }
    }
  }

  renderWord("Disconnected");
  delay(300);
}

#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11);

int numberOfRegisters = 6;

const int dataPin = 7;  //data pin conneceted to pin 7
const int latchPin = 9; // latch pin connected to pin 8
const int clockPin = 8; // clock pin connected to pin 9

int data[6] = {0, 0, 0, 0, 0, 0}; //The results will be stored here


void setup() {
  // put your setup code here, to run once:
  pinMode (dataPin, OUTPUT);
  pinMode (latchPin, OUTPUT);
  pinMode (clockPin, OUTPUT);
  Serial.begin(9600);
  HC12.begin(9600);
}


String content = "";
void loop() {
  if (HC12.available() > 0)
  {
    String content = HC12.readString();
    Serial.println(content);
    data[0] = content.substring(1, 2).toInt();
    data[1] = content.substring(3, 4).toInt();
    data[2] = content.substring(5, 6).toInt();
    data[3] = content.substring(7, 8).toInt();
    data[4] = content.substring(9, 10).toInt();
    data[5] = content.substring(11, 12).toInt();
    UpdateDisplay();
    delay(10);
  }

}
void UpdateDisplay() {
  digitalWrite(latchPin, LOW);  // prepare shift register for data

  for (int i = numberOfRegisters - 1; i >= 0; i--) {
    if (i == 4) {
      shiftOut(dataPin, clockPin, MSBFIRST, myfnNumToBits(data[5]));
    }
    else if (i == 5) {
      shiftOut(dataPin, clockPin, MSBFIRST, myfnNumToBits(data[4]));
    }
    else {
      shiftOut(dataPin, clockPin, MSBFIRST, myfnNumToBits(data[i]));
    }
  }
  digitalWrite(latchPin, HIGH); // update display
  
}

byte myfnNumToBits(int someNumber) {
  switch (someNumber) {
    case 0:
      return 127;
      break;
    case 1:
      return 13;
      break;
    case 2:
      return 183 ;
      break;
    case 3:
      return 158;
      break;
    case 4:
      return 204;
      break;
    case 5:
      return 219;
      break;
    case 6:
      return 250;
      break;
    case 7:
      return 14;
      break;
    case 8:
      return 255;
      break;
    case 9:
      return 223;
      break;
    case 10:
      return 0;
      break;
  }
}

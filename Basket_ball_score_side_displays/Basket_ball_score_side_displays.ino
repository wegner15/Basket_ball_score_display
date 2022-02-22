#include <SoftwareSerial.h>

SoftwareSerial HC12(10, 11);

int numberOfRegisters=6;

const int dataPin = 7;  //data pin conneceted to pin 7
const int latchPin = 9; // latch pin connected to pin 8
const int clockPin = 8; // clock pin connected to pin 9

int data[6]={1,2,3,4,5,6}; //The results will be stored here
int Send[6]={1,2,3,4,5,6};

void setup() {
  // put your setup code here, to run once:
  pinMode (dataPin, OUTPUT); 
pinMode (latchPin, OUTPUT);
pinMode (clockPin, OUTPUT);
Serial.begin(9600);
HC12.begin(9600);  
Serial.println("Checking for more");
}
int number=0;
void loop() {
  // put your main code here, to run repeatedly:
String content = "";
//Serial.println("Checking for more");
if (HC12.available()) {
//  while (HC12.available()) {        // If HC-12 has data
    content= HC12.readString();      // Send the data to Serial monitor
//  }
    
 
  delay(2);
  Serial.println(content);
//<1,7,5,7,3,3,>
int start=0;
for(int i = 0; i < 6; i++){
  if (start==0){
    int index = content.indexOf(","); //We find the next comma
  data[i] = atol(content.substring(1,index).c_str()); //Extract the number
  content = content.substring(index+1); //Remove the number from the string
  Serial.println(data[i]);
  start=1;
  }
  else{
  int index = content.indexOf(","); //We find the next comma
  data[i] = atol(content.substring(0,index).c_str()); //Extract the number
  content = content.substring(index+1); //Remove the number from the string
  Serial.println(data[i]);}
}
UpdateDisplay();
delay(50);

}

}


void UpdateDisplay() {

  //    eightBits = eightBits ^ B11111111;  // then flip all bits using XOR

  digitalWrite(latchPin, LOW);  // prepare shift register for data

//    shiftOut(dataPin, clockPin, MSBFIRST, myfnNumToBits(5)); // send data 1
//    shiftOut(dataPin, clockPin, MSBFIRST, 212); // send data 2

    
  for (int i = numberOfRegisters-1; i >= 0; i--) {
   if (i==4){
    shiftOut(dataPin, clockPin, MSBFIRST, myfnNumToBits(data[5])); 
  }
  else if (i==5){
    shiftOut(dataPin, clockPin, MSBFIRST, myfnNumToBits(data[4])); 
//
  }
  else{
  shiftOut(dataPin, clockPin, MSBFIRST, myfnNumToBits(data[i])); }}
//delay(100);
  digitalWrite(latchPin, HIGH); // update display
  //delay(100);
}

byte myfnNumToBits(int someNumber) {
  switch (someNumber) {
    case 0:
      return 255;
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
      case 13:
      return 128;//time demacation
      break;
      case 14://right arrow
      return 143;
      break;
      case 15:
      return 241;//left Arrow
      break;
      
      
  }
}

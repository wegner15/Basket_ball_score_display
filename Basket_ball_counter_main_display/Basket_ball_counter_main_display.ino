#include <Wire.h>
#include <RtcDS3231.h>

#include <RTClib.h>


#include <SoftwareSerial.h>

int sendVal[6] = {0, 0, 0, 0, 0, 0};

SoftwareSerial HC12(D3 , D4); // HC-12 TX Pin, HC-12 RX Pin
const int numberOfRegisters = 20;
int  sendToShiftRegister[numberOfRegisters];

int timeRunning24=0;
int timerRunning=0;
int homeScores = 0;
int awayScores = 0;
int awayFauls = 0;
int homeFauls = 0;
int awayTimeout = 0;
String currentTime = "00:00";
int homeTimeout = 0;
String counter = "00:00";
int Start = 0;
int reseveCounterSeconds = 0;
int reseveCounterMinutes = 0;
int counterSeconds = 0;
int counterMinutes = 0;
int countUp = 0;
int counterState = 0;
int countType = 0;
int counter24_14Reserve = 0;
int  counter24Limit = 0;
int counterFor24_14 = 24;
int counter24Mode = 24;
int periods = 0;
int counterSet = 0;
int timeCounterMode = 0;
long lastTimeSinceStart = millis();
int resetval = 0;
//Shift register update variables
const int dataPin = 12;  // blue wire to 74HC595 pin 14
const int latchPin = 14; // green to 74HC595 pin 12
const int clockPin = 13; // yellow to 74HC595 pin 11

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "Bask";
const char* password = "12345678";
const char* PARAM_1 = "minutes";
const char* PARAM_2 = "seconds";
const char* PARAM_3 = "countUp";
const String inputParam1 = "minutes";
const String inputParam2 = "seconds";
const String inputParam3 = "countUp";


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>* {box-sizing: border-box;}
.column {float: left;width: 50%;padding: 10px;height: 50rem; }




</style></head><body>

<h2 style="color: #33cc33;">KSBC LEAGUE</h2>

<div class="row" style="width: 100%;">
  <div class="column" style="background-color:rgb(243, 239, 239); height: 100%; float: right;">
    <header class="teamName">AWAY</header>
        <div><a href="/home-2"><button id="buttonm2" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">-2</button></a><!----><a href="/homeplus2"><button id="buttona2" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">+2</button></a></div>
        <div><a href="/home-3"><button id="buttonm3" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">-3</button></a><!----><a href="/homeplus3"><button id="buttona3" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">+3</button></a></div>
        <div><a href="/home-1"><button id="buttonm1" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">-1</button></a><!----><a href="/homeplus1"><button id="buttona1" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">+1</button></a></div>
        <div><a href="/home-faul"><button id="buttonm1faul" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">Time Out</button></a><!----><a href="/homeplusfaul"><button id="buttona1faul" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">Foul</button></a></div>
  </div>
  
  <div class="column" style="background-color:rgb(233, 234, 240); height: 100%; width: 50%; float: left;">
      <header class="teamName" style="color:black;">HOME</header>
      <div><a href="/away-2"><button id="buttonm2"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">-2</button></a><!--
--><a href="/awayplus2"><button id="buttona2"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">+2</button></a>
    </div>
      <div><a href="/away-3"><button id="buttonm3"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">-3</button></a><!--
      --><a href="/awayplus3"><button id="buttona3"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">+3</button></a></div>
      <div><a href="/awayplus1"><button id="buttonm1"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">-1</button></a><!--
      --><a href="/away-1"><button id="buttona1"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">+1</button></a></div>
      <div><a href="/away-1faul"><button id="buttonm1faul"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">Foul</button></a><!--
      --><a href="/awayplusfaul"><button id="buttona1faul"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">Time Out</button></a></div>      
    </div></div>
  
    <div style="width:100%;height:25%;border:1px solid rgb(47, 15, 224);">
      <div class="period" >PERIODS<div>
        <a href="/period-1"><button id="buttonm2"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">-1 Period</button></a><!----><a href="/periodplus1"><button id="buttona2"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">+1Period</button></a>
      </div></div><div class="mode">MODE<div>
        <a href="/realTime"><button id="buttonm2" style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">Real Time</button></a><!----><a href="/countDown"><button id="buttona2"style="background-color: blue;width: 45%;height: 40px; color: azure;border-radius: 5px 5px;">Count Down</button></a>
      </div></div>
    </div>
      
    
    <div style="width:100%; height:20rem; padding-top: 1.5%; padding-left: 2.5%; "><div style="align-self: center; height: 30%;padding: 2%;">
        <a href="/start"><button id="startStopButton" style="height: 100%; font-size: larger; border-radius: 50%; width: 45%; background-color: blue; color: aliceblue;">START/STOP</button></a>
        <a href="/reset"><button id="resetButton" style="height: 100%; float: right; font-size: larger; border-radius: 50%;  width: 45%; background-color: blue; color: aliceblue;">RESET</button></a></div>
        <div style=" height: 20%; padding: 1.5%;"><a href="/left"><button  style="float: left; border-radius: 20%; width: 33.33%;height: 100%; background-color:blue; font-size: larger; color: wheat;"><</button></a>
          <a href="/change24-14"><button style="width: 33.33%; border-radius: 20%; height: 100%;font-size: larger;background-color: blue; color: wheat;">24/14</button></a>
          <a href="/right"><button style="float: right;border-radius: 20%; background-color:blue; width: 33.33%;height: 100%; font-size: larger; color: wheat;">></button></a></div>
        
          <form action="/get" class="box" id="my-form">
            <header>SET TIMER</header> 
            <input name="minutes" type="text" placeholder="Minutes"style="float: left; width: 33.33%;">
            <input name="seconds" type="text" placeholder="Seconds" style="width: 33.33%;">
              <input type='hidden' placeholder="countUp" value='0' name='countUp'> 
            <input type="submit" value="submit" style="float: right; width: 33.33%;">
            </form>
    
      </div>
      <p style="color:chartreuse;">&#169 Novatech Engineerics 2021
       <br> Phone: 0790668724
      </p>
    </body></html>




)rawliteral";

const char reset_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<style>
.button {
  border: none;
  color: white;
  padding: 15px 32px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
}

.button1 {background-color: #4CAF50; border-radius: 15%; height: 5rem; width: 15rem;} /* Green */
.button2  {background-color: #4CAF50; border-radius: 15%;height: 5rem;width: 15rem;}

</style>
</head>
<body>
  <header>Confirm Reset</header>
    <a href="/"><button class="button button2">HOME</button></a>
<a href="/resetreset"><button class="button button1">RESET</button></a>

</body>
</html>
)rawliteral";
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
RtcDS3231<TwoWire> Rtc(Wire);
RTC_DS3231 rtc;
int hh=0,mm=0;

void setup() {
  // initialize I/O pins
Serial.begin(9600);
HC12.begin(9600); 

  for (size_t i = 0; i < numberOfRegisters; i++) {
    sendToShiftRegister[i] = 10;
  }
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  Rtc.Begin();

//RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
//if (!Rtc.IsDateTimeValid()) 
//    {
//        Serial.println("RTC lost confidence in the DateTime!");
//        Rtc.SetDateTime(compiled);
//    }
//Rtc.SetDateTime(compiled);
    RtcDateTime now = Rtc.GetDateTime();
    
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
    
sendToShiftRegister[17]=13;
// from server 
WiFi.softAP(ssid, password);
  //Serial.println();
 // Serial.print("Server IP address: ");
  Serial.println(WiFi.softAPIP());
// Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
// Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1,inputMessage2,inputMessage3;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->getParam(PARAM_1)->value() != "") {
      inputMessage1 = request->getParam(PARAM_1)->value();
      counterMinutes=inputMessage1.toInt();
      ;
reseveCounterMinutes=inputMessage1.toInt();
      
      
    } else {
      inputMessage1 = "none";
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    if (request->getParam(PARAM_2)->value() != "") {
      inputMessage2 = request->getParam(PARAM_2)->value();
      counterSeconds=inputMessage2.toInt();
      reseveCounterSeconds=inputMessage2.toInt();
      counterSet=1;
      Start=0;
    } else {
      inputMessage2 = "none";
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    if (request->getParam(PARAM_3)->value() != "") {
      inputMessage3 = request->getParam(PARAM_3)->value();
      if (inputMessage3.toInt()==1){
        countType=1;
      }
      else if (inputMessage3.toInt()==0){
         countType=0;
      }
    } else {
      inputMessage3 = "none";
    }
    
    
    request->send_P(200, "text/html", index_html);
    
});
  server.on("/home-2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
    
    homeScores=homeScores-2;
  });
  server.on("/homeplus2", HTTP_GET, [](AsyncWebServerRequest *request){//
     request->send_P(200, "text/html", index_html);
    homeScores=homeScores+2;
    
  });
  server.on("/home-3", HTTP_GET, [](AsyncWebServerRequest *request){
    homeScores=homeScores-3;
     request->send_P(200, "text/html", index_html);
   
  });
  server.on("/homeplus3", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    homeScores=homeScores+3;
    
  });
  server.on("/home-1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
    
  });
  server.on("/homeplus1", HTTP_GET, [](AsyncWebServerRequest *request){//
     request->send_P(200, "text/html", index_html);
    homeScores=homeScores+1;
    
  });
  server.on("/home-faul", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    homeFauls++;
    if(homeFauls>6){
    homeFauls=0;}
    
  });
  server.on("/homeplusfaul", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    homeTimeout++;
    if(homeTimeout>6){
      homeTimeout=0;
    }
    
    
    
  });
  server.on("/away-2", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    awayScores=awayScores-2;
    
  });
  server.on("/awayplus2", HTTP_GET, [](AsyncWebServerRequest *request){//
     request->send_P(200, "text/html", index_html);
     awayScores=awayScores+2;
    
  });
  server.on("/away-3", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    awayScores=awayScores-3;
    
  });
  server.on("/awayplus3", HTTP_GET, [](AsyncWebServerRequest *request){
    awayScores=awayScores+3;
     request->send_P(200, "text/html", index_html);
    
  });
  server.on("/awayplus1", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    awayScores--;
    
  });
  server.on("/away-1", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    awayScores++;
    
  });
  server.on("/away-1faul", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    awayTimeout++;
    if(awayTimeout>6){
      awayTimeout=0;
    }
    
  });
  server.on("/awayplusfaul", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    
    awayFauls++;
    
    if (awayFauls>6){
      awayFauls=0;
    }
  });
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    if (Start==0){
    Start=1;
    }
    else{
       Start=0;
    }
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", reset_page);
   
  });
  server.on("/resetreset", HTTP_GET, [](AsyncWebServerRequest *request){
    //Reset=1;
     request->send_P(200, "text/html", index_html);
    resetval=1;
  });
  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
  updateLeftIndicator();
    
  });
  server.on("/period-1", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    periods--;
    
  });
  server.on("/periodplus1", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send_P(200, "text/html", index_html);
    periods++;
    if (periods>6){
      periods=1;
    }
    Start=0;
    counterSet=1;
    counterSeconds = reseveCounterSeconds;
counterMinutes = reseveCounterMinutes;
    counterFor24_14 = counter24_14Reserve;

    
  });
  server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    
 request->send_P(200, "text/html", index_html);
    updateRightIndicator();
    
  });
  
  server.on("/change24-14", HTTP_GET, [](AsyncWebServerRequest *request){
   
    if ( counter24Mode==24){
      counterFor24_14=14;
      counter24Mode=14;
      counter24_14Reserve=14;
      if (Start==1){
        counterFor24_14=15;
       }
      
    }
    
    else if (counter24Mode==14){
       counterFor24_14=24;
       counter24Mode=24;
       counter24_14Reserve=24;
       if (Start==1){
        counterFor24_14=25;
       }
       
       
    }
 request->send_P(200, "text/html", index_html);
    
  });
server.on("/realTime", HTTP_GET, [](AsyncWebServerRequest *request){
    timeCounterMode=0;
     request->send_P(200, "text/html", index_html);
    
  });
  server.on("/countDown", HTTP_GET, [](AsyncWebServerRequest *request){
    timeCounterMode=1;
     request->send_P(200, "text/html", index_html);
    
  });
  
  
  server.onNotFound(notFound);
  server.begin();
  
}
int number=0;

void loop() {

//for (int i = 0; i < numberOfRegisters; i++) {
//    sendToShiftRegister[i] = number;
//    
//  }
  
//  sendToShiftRegister[15]=1;

//  sendToShiftRegister[15] home scores 3rd digit
//  sendToShiftRegister[14] home scores  1st digit
//  sendToShiftRegister[13] time outs home
  //  sendToShiftRegister[12] home fauls 
//  sendToShiftRegister[11] time out away
//  sendToShiftRegister[10] periods
//  sendToShiftRegister[9] counter 14-24 seconds counter second digit
//  sendToShiftRegister[8] counter 14-24 seconds counter first digit
  //sendToShiftRegister[7] away scores 1st digit
//  sendToShiftRegister[6] away scores 2nd digit
//sendToShiftRegister[5] away scores 3rd digit
//sendToShiftRegister[4] fauls away
//updateCounterArray();

if ((lastTimeSinceStart - millis()) >= 1000 ) {
   if(timeCounterMode==1){
    updateCounterArray();
    update24_14CounterArray();
    
   }
   else if (timeCounterMode==0){
    updateTimeArray();
    
   }
    
    lastTimeSinceStart = millis();
  }
  
updateFaulsArray();
//updateTimeArray();
//updateCounterArray();
updateTimeouts();
updateScores();
updatePeriods();
HC12.print(stringConcatnator());
////update24_14CounterArray();
 UpdateDisplay();
Reset();
  
  

  delay(1000);
  
  number++;
  if (number>9){
    number=0;
    
  }
  
}
String stringConcatnator(){

  String sendtoRemotes="<";
  for(int i=0;i<6;i++){
    sendtoRemotes+=sendVal[i];
    if(i<6){
    sendtoRemotes+=",";}
  }
  sendtoRemotes=sendtoRemotes+String(">");
  Serial.println(sendtoRemotes);
  return sendtoRemotes;
}

void updateTimeouts(){
  sendToShiftRegister[18]=homeTimeout;// time outs home
  sendToShiftRegister[11]=awayTimeout;
//  Serial.print("away:");
//  Serial.println(awayTimeout);
//  Serial.print("home:");
//  Serial.println(homeTimeout);
}
void updateCounterArray() {
  if(counterSeconds>0&&Start==1){
    counterSeconds--;
    timerRunning=1;}
  if (counterSeconds==0&&counterMinutes>0&&counterSet==0){
    
    counterMinutes--;
    counterSeconds=59;
    timerRunning=1;
  }
  else if (Start==1){
     counterSet=0;
  }

  if (counterSeconds>0||counterMinutes>0&&Start==1){
    timerRunning=1;
  }
if (timerRunning==1&&counterMinutes==0&&counterSeconds==0){
    sendToShiftRegister[19]=0;
    UpdateDisplay();
    
    delay(1000);
    timerRunning=0;
  }
  else{
  sendToShiftRegister[19]=10;
  }

int counterSecondsFirstDigit=counterSeconds/10;
int counterSecondssecondDigit=counterSeconds%10;
int counterMinutesFirstDigit=counterMinutes/10;
int counterMinutessecondDigit=counterMinutes%10;
sendToShiftRegister[3] = counterMinutesFirstDigit;
  sendToShiftRegister[2] = counterMinutessecondDigit;
  sendToShiftRegister[1] = counterSecondsFirstDigit;
  sendToShiftRegister[0] = counterSecondssecondDigit;


  sendVal[0]=counterMinutesFirstDigit;
sendVal[1]=counterMinutessecondDigit;
sendVal[2]=counterSecondsFirstDigit;
sendVal[3]=counterSecondssecondDigit;

}
void update24_14CounterArray() {
  if ( counterFor24_14 > counter24Limit&&Start==1) {
    counterFor24_14--;
    
  }
  if (counterFor24_14>counterSeconds&&counterMinutes==0){
    counterFor24_14=counterSeconds;
  }
  int first24_14Counter = counterFor24_14 % 10;
  int second24_14Counter = counterFor24_14 / 10;
//  if (first24_14Counter==0){
//    first24_14Counter=10;
//  }
  sendToShiftRegister[9] = second24_14Counter; //24 counter digit 2
  sendToShiftRegister[8] = first24_14Counter; //24 counter digit 1
  sendVal[4]=first24_14Counter;
  sendVal[5]=second24_14Counter;
  if (counterFor24_14>0&&Start==1){
    timeRunning24=1;
  }
  if (timeRunning24==1&&counterFor24_14==0){
    sendToShiftRegister[19]=0;
    delay(100);
    timeRunning24=0;
  }
  else{
  sendToShiftRegister[19]=10;
  }
}
void updateRightIndicator() {
  if (Start==1){
        counterFor24_14=counter24_14Reserve+1;
       }
else{counterFor24_14=counter24_14Reserve;}
sendToShiftRegister[17]=14;
}
void Reset(){
if (resetval==1){
homeScores = 0;
awayScores = 0;
awayFauls = 0;
homeFauls = 0;
awayTimeout = 0;
homeTimeout = 0;
counterSeconds = 0;
counterMinutes = 0;
counterFor24_14 = 24;
periods = 0;
resetval=0;
}
}
void updateLeftIndicator() {
  if (Start==1){
        counterFor24_14=counter24_14Reserve+1;
       }
else{counterFor24_14=counter24_14Reserve;}
sendToShiftRegister[17]=15;
}
void set24counterVariables(int counterMode){
  if (countUp==1){
    counter24Limit=counterFor24_14;
  }
  else{
    counter24Limit=0;
  }
  counter24Mode=counterMode;
}
void updateFaulsArray() {
  Serial.println(homeFauls);
  Serial.println(awayFauls);
  
 sendToShiftRegister[12]=homeFauls; //home fauls 
 sendToShiftRegister[4]=awayFauls; //away fauls 
}
void updatePeriods(){
  if(periods<6){
  sendToShiftRegister[10]=periods;}
  else{
    periods=1;
    sendToShiftRegister[10]=periods;
  }
  
}
void updateTimeArray() {
  int timeArray[3]={0};
  
//  DateTime rtcTime = rtc.now();
//
//  int ss = rtcTime.second();
//  int mm = rtcTime.minute();
//  int hh = rtcTime.twelveHour();
//  int DD = rtcTime.dayOfTheWeek();
//  int dd = rtcTime.day();
//  int MM = rtcTime.month();
//  int yyyy = rtcTime.year();
  //Serial.print(ss );
  RtcDateTime now = Rtc.GetDateTime();
 timeArray[0]=now.Hour();
 timeArray[1]=now.Minute();
 timeArray[2]=now.Second();

//  Serial.println(timeArray[0]);
//  Serial.println(timeArray[1]);
//  Serial.println(timeArray[2]);
  
//    Serial.print(now.Hour(), DEC);
//    Serial.print(':');
//    Serial.print(now.Minute(), DEC);
//    Serial.print(':');
//    Serial.print(now.Second(), DEC);
// delay(100);
//int timetime=now.Hour();
  //int mytimeH = now.Hour();
 //int mytimeM =now.Minute();
  //int mytimeS =now.Second();
//  Serial.print(mytimeH);
  //mytimeH.remove(0,1); mytimeH.remove(mytimeH.length());
  //int timeArra=mytimeH.toInt();
 //timeArray[0] = mytimeH.toInt();
//  timeArray[1]=now.Second();
  int firstHourDigit = timeArray[0] / 10;
  int secondHourDigit = timeArray[0] % 10;
  int firstMinuteDigit = timeArray[1] / 10;
  int secondMinuteDigit = timeArray[1] % 10;
  int firstSecondDigit=timeArray[2]/10;
  int secondSecondDigit=timeArray[2]%10;

  
  sendToShiftRegister[3] = firstHourDigit;
  sendToShiftRegister[2] = secondHourDigit;
  sendToShiftRegister[1] = firstMinuteDigit;
  sendToShiftRegister[0] = secondMinuteDigit;
  sendToShiftRegister[8] = secondSecondDigit;
  sendToShiftRegister[9] = firstSecondDigit;

sendVal[0]=firstHourDigit;
 sendVal[1]=secondHourDigit;
 sendVal[2]=firstMinuteDigit;
 sendVal[3]=secondMinuteDigit;
 sendVal[4]=secondSecondDigit;
 sendVal[5]=firstSecondDigit;
  
  //UpdateDisplay();
//   RtcDateTime now = Rtc.GetDateTime();
//    Serial.print(" Time:");
//    
//    Serial.print(now.Hour(), DEC);
//    Serial.print(':');
//    Serial.print(now.Minute(), DEC);
//    Serial.print(':');
//    Serial.print(now.Second(), DEC);
//    
//    delay(1000); // one second

}
void UpdateDisplay() {

  //    eightBits = eightBits ^ B11111111;  // then flip all bits using XOR

  digitalWrite(latchPin, LOW);  // prepare shift register for data

//    shiftOut(dataPin, clockPin, MSBFIRST, 218); // send data 1
//    shiftOut(dataPin, clockPin, MSBFIRST, 212); // send data 2

    
  for (int i = numberOfRegisters-1; i >= 0; i--) {
    
    shiftOut(dataPin, clockPin, MSBFIRST, myfnNumToBits(sendToShiftRegister[i])); 

  }
//delay(100);
  digitalWrite(latchPin, HIGH); // update display
  //delay(100);
}
void updateScores(){
  int homeScoresFirstDigit=homeScores%10;
  int homeScoresSecondDigit=homeScores/10;
  int homeScoresThirdDigit=homeScores/100;
  int awayScoresFirstDigit=awayScores%10;
  int awayScoresSecondDigit=awayScores/10;
  int awayScoresThirdDigit=awayScores/100;
if(awayScoresSecondDigit>9){
  awayScoresSecondDigit=awayScoresSecondDigit%10;
}
if (homeScoresSecondDigit>9){
  homeScoresSecondDigit=homeScoresSecondDigit%10;
}
  sendToShiftRegister[15]=homeScoresThirdDigit; //home scores 3rd digit
 sendToShiftRegister[14] =homeScoresFirstDigit;//home scores  1st digit
sendToShiftRegister[16]=homeScoresSecondDigit; 
//sendVal[0]=
//sendVal[1]=
//sendVal[2]=

 sendToShiftRegister[7]=awayScoresFirstDigit; //away scores 1st digit
  sendToShiftRegister[6]=awayScoresSecondDigit; //away scores 2nd digit
sendToShiftRegister[5]=awayScoresThirdDigit; //away scores 3rd digit
//sendVal[3]=
//sendVal[4]=
//sendVal[5]=
}
byte myfnNumToBits(int someNumber) {
  switch (someNumber) {
    case 0:
      return 253;
      break;
    case 1:
      return 97;
      break;
    case 2:
      return 219;
      break;
    case 3:
      return 243;
      break;
    case 4:
      return 103;
      break;
    case 5:
      return 183;
      break;
    case 6:
      return 191;
      break;
    case 7:
      return 225;
      break;
    case 8:
      return 255;
      break;
    case 9:
      return 247;
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

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESPDateTime.h>


unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000; //1sec period


// Pinout Connections
//Light Relays  
int lrelay1_Pin = 13;
int lrelay2_Pin = 12;
const char* NIGHTCYCLE_ON = "12:30:00";
const char* NIGHTCYCLE_OFF = "18:30:00";
unsigned long cycleStart;
unsigned long cycleOnMillis;
unsigned long cycleOffMillis;
bool cycleUpFlag = false;
bool cycleState = false; // True is ON (Night), false is OFF (Day)
//Fan Relays
//int frelay1_Pin = **;

//SSID credentials
const char* ssid     = "Cuna de la ciencia";
const char* password = "enricofermi";
// Set web server port number to 80
WebServer server(80);

// Variable to store the HTTP request
String header;

// HTML & CSS contents which display on web server
String HTML = "<!DOCTYPE html>\
  <html>\
  <body>\
  <h1>Indoor Management</h1>\
  <i>Luces</i>\
  </body>\
  </html>";
 
void handle_root() {
// Handle root url (/)
  server.send(200, "text/html", HTML);
}

void setupDateTime() {
//DateTime Setup  
  // setup this after wifi connected
  // you can use custom timeZone,server and timeout
  // DateTime.setServer("asia.pool.ntp.org");
  // DateTime.begin(15 * 1000);
  // this method config ntp and wait for time sync
  // default timeout is 10 seconds
  DateTime.setTimeZone(-3);
  DateTime.begin();
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
  }
}


void setupWiFi(){
// Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

int checkTime(){ 
//Returns an integer for one of three possible states 
  String h2 = DateTime.format("%R");
  if(h2 == NIGHTCYCLE_ON){
    Serial.println("Night Cycle ON");
    return 0;
  }
    else if(h2 == NIGHTCYCLE_OFF){
      Serial.println("Night Cycle OFF");
      return 1;
    }
    else{
      Serial.println("Stand-By");
      return 2;
    }
}

int changeRelayState(int state){ 
//Returns an integer after switching relay states based on state recieved
  unsigned long lastMillis;
  switch(state){
      case 0:
        if(cycleState == false){
          //digitalWrite(lrelay1_Pin,LOW); //Lights Off
          Serial.println("Lights OFF");
          cycleState = true;
          cycleOnMillis = millis();
          return 0;
          }
          break;
      case 1:
        if(cycleState == true){
          //digitalWrite(lrelay1_Pin,HIGH); // Lights ON
          Serial.println("Lights ON");
          cycleState = false;
          cycleOffMillis = millis();
          return 1;
        }  
        break;
      case 2:
       if(cycleState == true){
         currentMillis = millis();
         lastMillis = cycleOnMillis - currentMillis;
         //Serial.print("Tiempo Encendido (Minutos) :");
         //Serial.println(lastMillis / 60000);
         }
         else{
           currentMillis = millis();
           lastMillis = cycleOffMillis - currentMillis;
           //Serial.print("Tiempo Apagado (Minutos) :");
           //Serial.println(lastMillis / 60000);
         }
         return 2;
         break;
  }   
}



void setup(){
  // put your setup code here, to run once:
  Serial.begin(9600);
  startMillis = millis();
  //Setting relays up
  pinMode(lrelay1_Pin, OUTPUT);
  pinMode(lrelay2_Pin, OUTPUT);

  setupWiFi();
  setupDateTime();

// Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/",handle_root);
  server.begin();

  Serial.println(DateTime.format(DateFormatter::TIME_ONLY));
  String h = DateTime.toString();
  String h2 = DateTime.format("%T");
  Serial.println(h);
  Serial.println(h2);
  
}
void loop(){
  // put your main code here, to run repeatedly:
  int relay_state;
  server.handleClient();
  relay_state = checkTime();
  changeRelayState(relay_state);  
  
}


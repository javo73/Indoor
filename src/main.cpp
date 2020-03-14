#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESPDateTime.h>



// Pinout Connections
//Light Relays  
int lrelay1_Pin = 13;
int lrelay2_Pin = 12;
const char* NIGHTCYCLE_ON = "12:00";
const char* NIGHTCYCLE_OFF = "18:00";
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
 
// Handle root url (/)
void handle_root() {
  server.send(200, "text/html", HTML);
}

void setupDateTime() {
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

void checkTime(){ //Gives a TRUE output if the time is between the dark hours 
  String h2 = DateTime.format("%R");

  if(h2 >= NIGHTCYCLE_ON)
    Serial.println("ALARM");
    else
    Serial.println(h2);
}
void setup(){
  // put your setup code here, to run once:
  Serial.begin(9600);
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
  String h2 = DateTime.format("%R");
  Serial.println(h);
  Serial.println(h2);
  
}
void loop(){
  // put your main code here, to run repeatedly:
  server.handleClient();
  
    
  
}


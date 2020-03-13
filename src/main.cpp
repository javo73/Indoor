#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

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
<h1>Hola Bubi</h1>\
<i>Te amo</i>\
</body>\
</html>";
 
// Handle root url (/)
void handle_root() {
  server.send(200, "text/html", HTML);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}
// Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/",handle_root);
  server.begin();
}
void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}


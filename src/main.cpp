#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPDateTime.h>
//Asyc Web Server
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//DHT22 SENSOR
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//DHT22 Temperature & Humidity Sensor
#define DHTPIN  14
#define DHTTYPE    DHT22

DHT_Unified dht(DHTPIN,DHTTYPE);

float temp = 00.00;
float hum = 00.00;

unsigned long startMillis;
unsigned long currentMillis;
unsigned long dhtpreviousMillis = 0;
const unsigned long period = 2000; //2sec period




//Light Variable Setup (includes relay pins, and global variables) 
int lrelay1_Pin = 25;
int lrelay2_Pin = 26;
int lrelay3_Pin = 34;

const char* NIGHTCYCLE_ON = "12:30:00";
const char* NIGHTCYCLE_OFF = "18:30:00";
unsigned long cycleStart;
unsigned long cycleOnMillis;
unsigned long cycleOffMillis;
bool cycleUpFlag = false;
bool cycleState = false; // True is ON (Night), false is OFF (Day)
//Fan Relays
int fanrelay2_Pin2 = 35;
int fanrelay3_Pin1 = 32;
int fanrelay3_Pin2 = 33;
//int frelay1_Pin = **;

//SSID credentials
const char* ssid     = "Cuna de la ciencia";
const char* password = "enricofermi";
// Set web server port number to 80
AsyncWebServer server(80);

/*String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float temp = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(temp)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(temp);
    return String(temp);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float hum = DHT.readHumidity();
  Serial.println(hum);
  if (isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    Serial.println(hum);
    return String(hum);
  }
}*/

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temp);
  }
  else if(var == "HUMIDITY"){
    return String(hum);
  }
  return String();
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

void changeRelayState(int state){ 
//Returns an integer after switching relay states based on state recieved
  unsigned long lastMillis;
  switch(state){
      case 0:
        if(cycleState == false){
          //digitalWrite(lrelay1_Pin,LOW); //Lights Off
          //digitalWrite(lrelay2_Pin,LOW); //Lights Off
          //digitalWrite(lrelay3_Pin,LOW); //Lights Off
          Serial.println("Lights OFF");
          cycleState = true;
          cycleOnMillis = millis();
          }
          break;
      case 1:
        if(cycleState == true){
          //digitalWrite(lrelay1_Pin,HIGH); // Lights ON
          //digitalWrite(lrelay2_Pin,HIGH); // Lights ON
          //digitalWrite(lrelay3_Pin,HIGH); // Lights ON
          Serial.println("Lights ON");
          cycleState = false;
          cycleOffMillis = millis();
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
         break;
  }   
}



void setup(){
  // put your setup code here, to run once:
  Serial.begin(9600);
  startMillis = millis();
  dht.begin();
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
  
  //Async Web Server 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temp).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(hum).c_str());
  });
  server.begin();

  Serial.println(DateTime.format(DateFormatter::TIME_ONLY));
  String h = DateTime.toString();
  String h2 = DateTime.format("%T");
  Serial.println(h);
  Serial.println(h2);
  //DHT22
  Serial.println(F("DHT22 Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  // Set delay between sensor readings based on sensor details.
}
void loop(){
  // put your main code here, to run repeatedly:
  unsigned long dhtMillis = millis();
  int relay_state;
  //relay_state = checkTime();
  //changeRelayState(relay_state);
  if (dhtMillis - dhtpreviousMillis >= period ) {
    dhtpreviousMillis = dhtMillis;
     sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    }
    else {
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    }
    else {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
    }
  }
  
}


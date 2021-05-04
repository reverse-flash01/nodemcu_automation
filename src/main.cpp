#define BLYNK_PRINT Serial     
#include<ESP8266WiFi.h>
#include<BlynkSimpleEsp8266.h>
#include<DHT.h>
#include<ESPAsyncTCP.h>
#include<ESPAsyncWebServer.h>
#include<AsyncElegantOTA.h>

//procedure prototypes
void sendSensor();
void connectionIndicator();

//definitions
#define DHTPIN 14 //D5
#define DHTTYPE DHT11 //DHT sensor type
#define REDLED 12 //D6

// network and project credentials
char auth[] = "ArApPh_RleM6cDytPu6_oAhm3ye6QKGX";//Blynk project authentication token
char ssid[] = "JALAL";//Wifi name
char pass[] = "YS3UL%U7";//Wifi password

//setting up 
DHT dht(DHTPIN,DHTTYPE);//DHT sensor object
BlynkTimer timer;//Timer object to run procedure at some interval
AsyncWebServer server(80);//OTA update server

//variables
unsigned long previousMillis = 0;
const long interval = 1000;      
int ledState = LOW; 

void sendSensor()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if(isnan(t)||isnan(h))
  {
    Serial.println("failed to read data!");
    return;
  }
  Blynk.virtualWrite(V5,h);
  Blynk.virtualWrite(V6,t);
}

BLYNK_CONNECTED(){
  Blynk.syncAll();
}

void connectionIndicator(){
  if(Blynk.connected() == true)
    digitalWrite(REDLED,HIGH);
  else
    digitalWrite(REDLED,LOW);
}

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(REDLED,OUTPUT);
  Serial.begin(9600);
  Blynk.begin(auth,ssid,pass);
  dht.begin();
  timer.setInterval(1000L,sendSensor);
  timer.setInterval(5000L,connectionIndicator);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
    request->send(200, "text/plain", "Hi! I am ESP8266.");
  });
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
}

void loop() { 
  Blynk.run();
  timer.run();
  AsyncElegantOTA.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
} 
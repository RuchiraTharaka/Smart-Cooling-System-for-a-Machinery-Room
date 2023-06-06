#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2  //D4

//Initializing the buzzer at D3
int buzzer = D3;

char* ssid = "Dialog 4G 517";
char* password = "576E5Fc3";
const char* mqtt_server = "192.168.41.113";

userName="serverCO326";
passWord="group5";

//Initializing fan
uint8_t fan = 3;  //Rx pin (GPIO3)(green wire)

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  String receivedMsg = "";
  for(int i=0;i<length;i++){
    receivedMsg = receivedMsg + (char)payload[i];    
  }
  Serial.println();
}

void setup_wifi(){
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId,userName,passWord)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(void)
{
  sensors.begin();  // Start up the library
  pinMode(buzzer,OUTPUT);  //Set D3 as output for buzzer
  pinMode(fan,OUTPUT);
  analogWrite(fan,0);
  Serial.begin(9600);
  Serial.setDebugOutput(true);

  setup_wifi();
  delay(1000);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

float temp;
long lastMsg;

void loop(void)
{ 
  if(!client.connected()){
    reconnect();    
  }
  client.loop();
  long now = millis();
  if(now-lastMsg>2000){
    lastMsg = now;
    // Send the command to get temperatures
    sensors.requestTemperatures(); 
  
    //print the temperature in Celsius
    Serial.print("Temperature: ");
    temp = sensors.getTempCByIndex(0);
    Serial.print(temp);
    Serial.print((char)176);//shows degrees character
    Serial.print("C  |  ");
    analogWrite(fan,int((temp-33.0)*256.0));
    Serial.print(int((temp-33.0)*256.0));

    snprintf(msg,200,"{\"temperature\": %f, \"heart rate\": %f, \"pulse rate\": %f, \"oxygen saturation\": %f}",value1,value2,value3,value4);
    client.publish("outtopic","Hello world");

    if(temp>35){
      tone(buzzer,1000,200); 
    }
  }
}

//client.subscribe("greenBottles/#");

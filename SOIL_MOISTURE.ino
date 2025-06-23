#include <WiFi.h>
#include <PubSubClient.h>

#define soil 34
#define motor 4

const char* ssid = "iot1";
const char* password =  "iot123456";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;

String valstring="";

bool mnl = false;
bool st = true; 

WiFiClient esp1234;
PubSubClient client(esp1234);

void setup() 
{
  pinMode(soil,INPUT);
  pinMode(motor,OUTPUT);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print(WiFi.status());
  Serial.println(WiFi.SSID());

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(MQTTcallback);
  while (!client.connected()) 
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("Client24567"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
  Serial.print("Before");
  client.subscribe("esp/mtr");
  Serial.print("After");
}

void MQTTcallback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message received in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String message;
  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (message == "On"){
    mnl = true;
    digitalWrite(motor,HIGH);
    Serial.println("PUMP ON");
    client.publish("esp/mst","ON");
  }
  if (message == "Off"){
     mnl = false;
     digitalWrite(motor, LOW);
     Serial.println("PUMP OFF");
     client.publish("esp/mst","OFF");
  }
}

void loop() 
{ 
  int val = digitalRead(soil);
  valstring = String(val);
  Serial.println(valstring);

  if(mnl == false){
    if(val==1){
      if(st==true){

        client.publish("esp/soil","Dry");
        digitalWrite(motor,HIGH);
        Serial.println("PUMP ON");
        client.publish("esp/mst","ON");
        st = false;

      }
      
    }
    else if(val==0){
      if(st==false){
        client.publish("esp/soil","Wet");
        digitalWrite(motor, LOW);
        Serial.println("PUMP OFF");
        client.publish("esp/mst","OFF");
        st = true;
      }
      
    }

  }
   
  client.loop();
  delay(200);
}
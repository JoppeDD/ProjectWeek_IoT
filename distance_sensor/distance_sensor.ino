/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Watson IoT connection details
#define MQTT_HOST "9qcd6d.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:9qcd6d:sensor:sonic-sensor-arduino"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "sensorsecuritytoken"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/update/fmt/json"

// Update these with values suitable for your network.

const char* ssid = "ucll-projectweek-IoT";
const char* password = "Foo4aiHa";
const char* mqtt_server = "9qcd6d.messaging.internetofthings.ibmcloud.com";
const int button = 0;
int buttonState = 0;
const int trigPin = 4;
const int echoPin = 5;
const int lockedPin = 13;
const int redLed = 15;
const int greenLed = 12;

WiFiClient espClient;
PubSubClient client(espClient);
boolean locked = false;
long lastMsg = 0;
char msg[50];
int value = 0;
long duration;
int distance;
int counter;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(lockedPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  digitalWrite(lockedPin, LOW);
  digitalWrite(greenLed, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
}

void setup_wifi() {

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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//
//  // Switch on the LED if an 1 was received as first character
//  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//    // but actually the LED is on; this is because
//    // it is acive low on the ESP-01)
//  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//  }
  if (locked){
    Serial.print("Message arrived");
    locked = false;
    digitalWrite(lockedPin, LOW);
    Serial.print("Pin value: ");
    Serial.println(digitalRead(lockedPin));
    Serial.println("Lock is unlocked");
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
    delay(7000);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("connected");
      client.subscribe(MQTT_TOPIC_DISPLAY);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    if (locked){
      Serial.println("Lock is locked");
      buttonState = digitalRead(button); 
      if (buttonState == LOW) {
        locked = false;
        digitalWrite(lockedPin, LOW);
        Serial.print("Pin value: ");
        Serial.println(digitalRead(lockedPin));
        digitalWrite(redLed, LOW);
        digitalWrite(greenLed, HIGH);
        delay(7000);
      }
    } else {
      // Clears the trigPin
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
    
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
    
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);
      
      // Calculating the distance
      distance= duration*0.034/2;
      if (distance > 184) {
        Serial.println("Out of range");
      } else {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println("cm");
      }
          
      if (distance < 10) {
        counter = 0;
        for (int i = 0; i < 5; i++) {
          digitalWrite(trigPin, LOW);
          delayMicroseconds(2);
    
          // Sets the trigPin on HIGH state for 10 micro seconds
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(10);
          digitalWrite(trigPin, LOW);
    
          // Reads the echoPin, returns the sound wave travel time in microseconds
          duration = pulseIn(echoPin, HIGH);
          
          // Calculating the distance
          distance= duration*0.034/2;
          if (distance < 10) {
              ++counter;
              Serial.println(counter);
          }
          if (distance > 184) {
            Serial.println("Out of range");
          } else {
            Serial.print("Distance: ");
            Serial.print(distance);
            Serial.println("cm");
          }
          delay(1000);
        }
        if (distance < 10 && counter == 5) {
          String payload = "{ \"locked\" : true}";
          locked = true;
          digitalWrite(lockedPin, LOW);
          digitalWrite(lockedPin, HIGH);
          digitalWrite(redLed, HIGH);
          digitalWrite(greenLed, LOW);
          Serial.print("Pin value: ");
          Serial.println(digitalRead(lockedPin));
          if (client.publish(MQTT_TOPIC, (char*) payload.c_str())) {
            Serial.println("Message sent");
          } else {
            Serial.println("Message failed");
          }
            delay(500);
         }
      }
    }
    delay(1000);
   }
   
//void unlock{
//  
//}
    
   
//  buttonState = digitalRead(button); 
//  if (buttonState == HIGH) {
//    delay(500);
//  }
//  else {
//    Serial.println("Button Pressed");
//    String payload = "{ \"d\" : {";
//    payload += "\"Button Pressed\":\""; payload += "True\",";
//    payload += "\"Local IP\":\""; payload += WiFi.localIP().toString(); payload += "\"";
//    payload += "}}";
//    Serial.println(payload);

//    if (client.publish(MQTT_TOPIC, (char*) payload.c_str())) {
//      Serial.println("Publish ok");
//    } else {
//      Serial.println("Publish failed");
//    }
//    delay(500);
//   }

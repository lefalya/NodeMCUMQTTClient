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

// Update these with values suitable for your network.

const char* ssid = "Lab Lanjut 121";
const char* password = "TanyaAsisten";
const char* mqtt_server = "192.168.121.104";

const int trigPin = 5;  //D1
const int echoPin = 16;  //D0
bool ledState = LOW;

long duration;
int distance;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  Serial.print("payload ");
  Serial.println(payload[0]);
  if (payload[0] == '2') {
    if (payload[2] == 'a') digitalWrite(D5, !ledState);   
    ledState = !ledState;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("test", "hello world");
      // ... and resubscribe
      client.subscribe("testing");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(D5, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  Serial.begin(115200);
  setup_wifi();
  Serial.println("connected");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  
  if (now - lastMsg > 500) {
    lastMsg = now;
    ++value;


    int sensorValue = analogRead(A0);   // read the input on analog pin 0

    float voltage = sensorValue * (3.3 / 1023.0);   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)

    Serial.println(voltage);   // print out the value you read

   
    char buf[8];
    sprintf(buf, "%f", voltage);

     char messageDelivered[10];
      strcpy(messageDelivered,"2");
      strcat(messageDelivered," ");
      strcat(messageDelivered,buf);
//    
    Serial.print("pre msg : ");
    Serial.println(messageDelivered);
    Serial.print("Publish message: ");
    Serial.println(messageDelivered);
    client.publish("test", messageDelivered);
   
  }

}

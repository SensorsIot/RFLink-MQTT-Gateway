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
#include <credentials.h>

// Update these with values suitable for your network.

const char* ssid = mySSID;
const char* password = myPASSWORD;
const char* mqtt_server = "192.168.0.203";
const char* MQTT_USER = "admin";
const char* MQTT_PASSWORD = "admin";
const char* MQTT_TOPIC_REC = "RFLINK_REC";
const char* MQTT_TOPIC_TRANS = "RFLINK_TRANS";

String hi = "";
bool dataAvailable = false;


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  randomSeed(micros());

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.write(payload[i]);
  }
  Serial.write('\n');
  //Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(MQTT_TOPIC_REC, "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_TOPIC_TRANS);
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(57600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  char serialChar;

  while (Serial.available() > 0) // Chek for availablity of data at Serial Port
  {
    serialChar = Serial.read(); // Reading Serial Data and saving in data variable
    String data2(serialChar);
    hi = hi + data2;
    delay(1);
    dataAvailable = true;
  }


  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // convert hi to char array
  if (dataAvailable == true) {
    int str_len = hi.length() + 1;
    char msg[str_len];
    hi.toCharArray(msg, str_len);
    //Serial.print("Publish message: ");
    //Serial.println(msg);
    client.publish(MQTT_TOPIC_REC, msg);
    hi = "";
    dataAvailable = false;
  }
}

/*
  Basic ESP8266 MQTT example

  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.

  It connects to an MQTT server then:

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
#include "credentials.h"

WiFiClient espClient;
PubSubClient client(espClient);

#define BAUD 57600
#define MAX_MSG_SIZE 60

char msg[MAX_MSG_SIZE + 1];
byte msg_pos;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.setSleepMode(WIFI_MODEM_SLEEP); // Default is WIFI_NONE_SLEEP
  WiFi.setOutputPower(10); // 0~20.5dBm, Default max

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet); // For Static IP
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
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
*/

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_ID, MQTT_USER, MQTT_PSWD)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // client.publish(MQTT_TOPIC_OUT, "hello world");
      // ... and resubscribe
      // client.subscribe(MQTT_TOPIC_IN);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      for (byte i = 0; i < 10; i++) delay(500); // delay(5000) may cause hang
    }
  }
}

void setup() {
  Serial.begin(BAUD, SERIAL_8N1, SERIAL_FULL);
  setup_wifi();
  client.setServer(MQTT_SERVER, 1883);
  // client.setCallback(callback);
}

void loop() {

  msg_pos = 0;

  while ((Serial.available() > 0) && (msg_pos < MAX_MSG_SIZE)) // Chek for availablity of data at Serial Port
  {
    msg[msg_pos++] = Serial.read(); // Reading Serial Data and saving in data variable
    delayMicroseconds(174U);        // 174µs/char @ 57600 Bauds
  }

  msg[msg_pos++] = 0; // add Null byte at the end

  // msg min size 40
  // msg always start with "20;"
  if ((msg_pos > 40) && (msg[0] == '2') && (msg[1] == '0') && (msg[2] == ';'))
  {
    if (!client.connected()) reconnect();
    //client.loop();
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(MQTT_TOPIC_OUT, msg);
  }
  else delay(7); // Wait for a complete minimal message
  
  delay(1); // To Enable some more Modem Sleep
}

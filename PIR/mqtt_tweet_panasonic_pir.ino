#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "networkname"
const char* password = "psk";
const char* mqtt_server = "mqtt_hostname.local"; // name of device that will be mqtt broker, mine is on my local network hence .local
#define USE_SERIAL Serial

int inputPin = A0;               // choose the input pin (from PIR sensor)
int pirState = false;            // we start, assuming no motion detected
int val = 0;                     // volt read
int volt_threshold = 500;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname("combopizza"); 
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

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
      client.publish("mqtttopicname", "hello world");
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
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  val = analogRead(inputPin); // 1024 if triggered, otherwise hovers around 15
  if (val > volt_threshold) {            // check if the input is above threshold
    if (!pirState) {
      // we have just turned on
      Serial.println("Motion detected!");
      while (!pirState) {
        Serial.println(WiFi.status());
        if ((WiFi.status() == WL_CONNECTED)) {
          String finaltrigger = "/trigger/[name of IFTTT event]/with/key/[webhook key]?value1=" + String(random(999999));
          USE_SERIAL.println(WiFi.localIP());
          HTTPClient http;
          USE_SERIAL.print("Transmitting...\n");
          http.begin("maker.ifttt.com", 80, finaltrigger); //HTTP
          // https://maker.ifttt.com/trigger/[name of IFTTT event]/with/key/[webhook key]?value1=1024&value2=120
          // JSON is weird. {"value1":"1028","value2":"512"} dictionary becomes ?value1=1024&value2=512
          int httpCode = http.GET(); // use return code if want to check response
          http.end();
          USE_SERIAL.println("Transmission completed.");
          while (!client.connected()) {
            reconnect();
            delay(5000);
          }
          long now = millis();
          if (now - lastMsg > 2000) {
            lastMsg = now;
            Serial.println("action");
            client.publish("mqtttopicname", "action");
          }
          pirState = true; // blocks re-entry after successful transmission, and while continously true for motion detected
        }
        delay(100);
      }
    }
  }
  else {
    if (pirState) {
      // we have just turned of
      Serial.println("Motion ended!");
      pirState = false;
      delay(3000);
    }
  }
}

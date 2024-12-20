#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>

// DHT11 setup
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LDR setup
#define LDRPIN A0

// Device Pins
#define FAN 3
#define HUMIDITY_REGULATOR 4
#define LIGHTING_SYSTEM 5

// Network Settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(184, 106, 153, 149); // ThingsSpeak IP
EthernetClient client;

// ThingsSpeak settings
const char* apiKey = "REYN5DQFK0WVE8N1";
const int channelId = 2792025;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);

  // Start Ethernet and DHT sensor
  Ethernet.begin(mac);
  dht.begin();

  // Configure device pins
  pinMode(FAN, OUTPUT);
  pinMode(HUMIDITY_REGULATOR, OUTPUT);
  pinMode(LIGHTING_SYSTEM, OUTPUT);

  Serial.println("System initialized...");
}

void loop() {
  // Read sensors
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  int ldrValue = analogRead(LDRPIN);

  // Control devices based on sensor data
  if (temp > 30) {
    digitalWrite(FAN, HIGH);
  } else {
    digitalWrite(FAN, LOW);
  }

  if (humidity < 40) {
    digitalWrite(HUMIDITY_REGULATOR, HIGH);
  } else {
    digitalWrite(HUMIDITY_REGULATOR, LOW);
  }

  if (ldrValue < 500) {
    digitalWrite(LIGHTING_SYSTEM, HIGH);
  } else {
    digitalWrite(LIGHTING_SYSTEM, LOW);
  }

  // Log data to ThingsSpeak
  if (client.connect(server, 80)) {
    String url = "/update?api_key=";
    url += apiKey;
    url += "&field1=" + String(temp);
    url += "&field2=" + String(humidity);
    url += "&field3=" + String(ldrValue);

    client.print("GET ");
    client.print(url);
    client.println(" HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();

    Serial.println("Data sent to ThingsSpeak");
    client.stop();
  } else {
    Serial.println("Connection to ThingsSpeak failed!");
  }

  delay(20000); // Update every 20 seconds
}

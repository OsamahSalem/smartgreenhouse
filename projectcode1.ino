#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>

// Define the ThingsSpeak Read API key and Channel ID
const char* apiKey = "REYN5DQFK0WVE8N1";
const char* channelID = "2792025";

// DHT sensor settings
#define DHTPIN 2       // DHT11 data pin connected to Arduino pin 2
#define DHTTYPE DHT11  // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

// LDR and control pins
#define LDRPIN A0      // LDR connected to analog pin A0
#define FANPIN 3       // Fan control pin connected to digital pin 3
#define HUMIDITYPIN 4  // Humidity regulator control pin connected to digital pin 4
#define LIGHTPIN 5     // Lighting system control pin connected to digital pin 5

// Ethernet settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address for the W5500 Ethernet module
IPAddress server(184, 106, 153, 149);               // ThingsSpeak server (api.thingspeak.com)

// Ethernet client
EthernetClient client;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize Ethernet connection
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while (true);
  }
  delay(1000); // Allow time for DHCP setup

  // Initialize DHT sensor
  dht.begin();

  // Initialize control pins
  pinMode(FANPIN, OUTPUT);
  pinMode(HUMIDITYPIN, OUTPUT);
  pinMode(LIGHTPIN, OUTPUT);
  pinMode(LDRPIN, INPUT);

  Serial.println("System initialized and ready.");
}

void loop() {
  // Read temperature and humidity from DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if DHT11 readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Control fan based on temperature (turn ON if >30°C)
  if (temperature > 30) {
    digitalWrite(FANPIN, HIGH);
  } else {
    digitalWrite(FANPIN, LOW);
  }

  // Control humidity regulator (turn ON if <40% humidity)
  if (humidity < 40) {
    digitalWrite(HUMIDITYPIN, HIGH);
  } else {
    digitalWrite(HUMIDITYPIN, LOW);
  }

  // Read light intensity from LDR
  int ldrValue = analogRead(LDRPIN);

  // Control lighting based on LDR value (turn ON if dark)
  if (ldrValue < 500) {
    digitalWrite(LIGHTPIN, HIGH);
  } else {
    digitalWrite(LIGHTPIN, LOW);
  }

  // Print data to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, LDR Value: ");
  Serial.println(ldrValue);

  // Send data to ThingsSpeak
  sendDataToThingSpeak(temperature, humidity, ldrValue);

  // Delay before next loop (ThingsSpeak requires at least 15 seconds between updates)
  delay(15000);
}

void sendDataToThingSpeak(float temperature, float humidity, int ldrValue) {
  if (client.connect(server, 80)) {
    Serial.println("Connected to ThingSpeak");

    // Build HTTP GET request to send data
    String url = "/update?api_key=" + String(apiKey) +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(ldrValue);

    client.print("GET ");
    client.print(url);
    client.println(" HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();

    Serial.println("Data sent to ThingSpeak.");
  } else {
    Serial.println("Connection to ThingSpeak failed.");
  }

  // Wait for the server response
  while (client.available()) {
    char c = client.read();
    Serial.print(c); // Print response for debugging
  }
  client.stop();
}

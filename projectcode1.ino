#include <DHT.h>

// Define pins
#define DHTPIN 2       // DHT11 data pin connected to Arduino pin 2
#define DHTTYPE DHT11  // DHT11 sensor type
#define LDRPIN A0      // LDR connected to analog pin A0
#define FANPIN 3       // Fan control pin connected to digital pin 3
#define HUMIDITYPIN 4  // Humidity regulator control pin connected to digital pin 4
#define LIGHTPIN 5     // Lighting system control pin connected to digital pin 5

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize pins as output or input
  pinMode(FANPIN, OUTPUT);
  pinMode(HUMIDITYPIN, OUTPUT);
  pinMode(LIGHTPIN, OUTPUT);
  pinMode(LDRPIN, INPUT);

  // Start the DHT sensor
  dht.begin();
}
void loop() {
  // Read temperature and humidity from DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print readings to the serial monitor for debugging
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Control fan based on temperature (e.g., turn on if >30°C)
  if (temperature > 30) {
    digitalWrite(FANPIN, HIGH); // Turn fan ON
    Serial.println("Fan ON");
  } else {
    digitalWrite(FANPIN, LOW); // Turn fan OFF
    Serial.println("Fan OFF");
  }

  // Control humidity regulator based on humidity (e.g., turn on if <40%)
  if (humidity < 40) {
    digitalWrite(HUMIDITYPIN, HIGH); // Turn humidity regulator ON
    Serial.println("Humidity Regulator ON");
  } else {
    digitalWrite(HUMIDITYPIN, LOW); // Turn humidity regulator OFF
    Serial.println("Humidity Regulator OFF");
  }

  // Read light intensity from LDR
  int ldrValue = analogRead(LDRPIN);
  
  // Print LDR value for debugging
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  // Control lighting system based on LDR value (e.g., turn on if dark)
  if (ldrValue < 500) { // Adjust threshold as needed
    digitalWrite(LIGHTPIN, HIGH); // Turn light ON
    Serial.println("Light ON");
  } else {
    digitalWrite(LIGHTPIN, LOW); // Turn light OFF
    Serial.println("Light OFF");
  }

  // Delay for stability (adjust as needed)
  delay(100);
}

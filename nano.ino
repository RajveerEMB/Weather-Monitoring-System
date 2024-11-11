#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 2            // Pin connected to the DHT11 sensor
#define DHTTYPE DHT22       // DHT 22 sensor
#define MQ2PIN A0           // Analog pin connected to the MQ2 sensor

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "Rajveer";                     // WiFi SSID
char pass[] = "123456789";                // WiFi password
const char* broker = "broker.hivemq.com"; // MQTT broker address
const char* topic = "Home/SensorData";    // MQTT topic for sending sensor data

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Connecting to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connecting to MQTT broker
  client.setServer(broker, 1883);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ArduinoNano")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  client.loop();

  // Read temperature and humidity from the DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Read the MQ2 sensor value
  int mq2Value = analogRead(MQ2PIN);
  float smokeLevel = map(mq2Value, 0, 1023, 0, 100); // Scale to percentage

  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Prepare the data string with temperature, humidity, and smoke level
  String dataMessage = "Temperature: " + String(temperature) + 
                       ", Humidity: " + String(humidity) + 
                       ", Smoke Level: " + String(smokeLevel);

  // Publish the sensor data to MQTT
  if (client.publish(topic, dataMessage.c_str())) {
    Serial.println("Sensor data sent successfully!");
    Serial.println(dataMessage);
  } else {
    Serial.println("Failed to send sensor data");
  }

  delay(5000);  // Delay before the next reading
}

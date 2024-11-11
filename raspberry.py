import os
import requests
import paho.mqtt.client as mqtt

# MQTT broker details
broker = "broker.hivemq.com"
port = 1883
topic = "Home/SensorData"  # Topic where Nano sends sensor data

# Firebase setup
database_secret = os.getenv('FIREBASE_SECRET')
database_url = "https://embedded-1system-default-rtdb.firebaseio.com"

# Check if Firebase secret is set
if not database_secret:
    print("Error: FIREBASE_SECRET environment variable is not set.")
    exit(1)

# Update Firebase with the received sensor data
def update_firebase_sensor_data(sensor_data):
    # sensor_data is expected to be a string like: "Temperature: 25.0, Humidity: 60.0, Smoke Level: 15.0"
    data = {"SensorData": sensor_data}
    response = requests.patch(f"{database_url}/SensorData.json?auth={database_secret}", json=data)
    if response.status_code == 200:
        print(f"Sensor data updated successfully: {sensor_data}")
    else:
        print(f"Failed to update sensor data: {response.text}")
# MQTT callback functions
def on_connect(client, userdata, flags, rc):
    print(f"Connected to MQTT Broker with result code {rc}")
    client.subscribe(topic)  # Subscribe to the topic where Nano publishes data

def on_message(client, userdata, msg):
    print(f"Received message on topic {msg.topic}: {msg.payload.decode()}")
    # Extract the sensor data from the message payload
    sensor_data = msg.payload.decode()  # Expecting a string like "Temperature: 25.0, Humidity: 60.0, Smoke Level: 15.0"
    update_firebase_sensor_data(sensor_data)  # Update Firebase with the received data

# Set up MQTT client
client = mqtt.Client()
client.on_connect = on_connect  # Callback when connected to MQTT
client.on_message = on_message  # Callback when message is received
client.connect(broker, port, 60)  # Connect to MQTT broker

client.loop_forever()  # Start the MQTT client loop to listen for messages

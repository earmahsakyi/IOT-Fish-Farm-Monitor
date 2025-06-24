#include <PubSubClient.h>
#include <WiFiS3.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiSSLClient.h>


 WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

// Pin Definitions
#define TURBIDITY_ANALOG_PIN A0
#define WATER_LEVEL_PIN A1
#define BUZZER_PIN 3
#define TEMP_SENSOR_PIN 4
#define PIR_PIN 5
#define RELAY_PIN 6


// Wi-Fi credentials
const char* ssid = "CASSIDY GH";
const char* password = "*******";

// MQTT credentials
const char* mqtt_server = "myfarmers.xyz";
const char* mqtt_username = "burna";
const char* mqtt_password = "*********";
const char* clientID = "Burnaboy24";
const char* pub_topic = "SENSORS";
const char* sub_topic = "DEVICE";

const char caCert[] PROGMEM = R"EOF(
Server certificate
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
-----END CERTIFICATE-----
)EOF";


// Calibration parameters for sensors
const float TURBIDITY_SLOPE = -2.0708; 
const float TURBIDITY_INTERCEPT = 502.6154; 

OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);

long last_loop_time;
boolean relay_status;
boolean buzzer_status;

// Function Prototypes
void put_on_relay();
void put_off_relay();
void beep_buzzer();
uint16_t read_motion_sensor();
float read_temp_sensor();
String read_water_level();
float read_turbidity();
void connectToWiFi();
void ReceivedMessage(char* topic, byte* payload, unsigned int payload_length);
bool Connect();
boolean mqtt_client_publish(const char* topic, const char* msg);

void setup() {
  Serial.begin(115200);

  
  connectToWiFi();// Connect to Wi-Fi

     
  wifiClient.setCACert(caCert); // Set the root CA certificate
 

  client.setServer(mqtt_server, 8883);
  client.setCallback(ReceivedMessage);

  if (Connect()) {
    Serial.println("Connected Successfully to MQTT Broker!");
  } else {
    Serial.println("Connection Failed!");
  }

  // Pin Modes
  pinMode(TURBIDITY_ANALOG_PIN, INPUT);
  pinMode(WATER_LEVEL_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  
  sensors.begin();// Initialize sensors

  // Initialize relay and buzzer states 
  put_off_relay();
  noTone(BUZZER_PIN);

  last_loop_time = millis();
  relay_status = false;
  buzzer_status = false;
}

void loop() {
  long now = millis();

  if ((now - last_loop_time) > 5000) {
    last_loop_time = now;
    if (!client.connected()) {
      if (Connect()) {
        Serial.println("Reconnected to MQTT Broker!");
      } else {
        Serial.println("Reconnection Failed!");
      }
    }
    client.loop();

    // Read sensors
    float turbidity = read_turbidity();
    String waterLevel = read_water_level();
    float temperature = read_temp_sensor();
    uint16_t motion = read_motion_sensor();
    char msg[256];

    char motion_str[20];
    if (motion) {
      strcpy(motion_str, "Motion Detected");
       mqtt_client_publish("MOTION_ALARM", "ON");
      beep_buzzer();
     
    } else {
      strcpy(motion_str, "No Motion Detected");
    }
   

    // Publish data to MQTT
    sprintf(msg, "Turbidity: %3.2f NTU, Water Level: %s, Temperature: %3.2f °C, Motion: %s", turbidity, waterLevel.c_str(), temperature, motion_str);
    mqtt_client_publish(pub_topic, msg);

    Serial.println(msg);
  }
}

void put_on_relay() {
  digitalWrite(RELAY_PIN, HIGH);
}

void put_off_relay() {
  digitalWrite(RELAY_PIN, LOW);
}

void beep_buzzer() {
analogWrite(BUZZER_PIN, 50); // Set the duty cycle to maximum
  delay(1000); // Beep for 1 second
  analogWrite(BUZZER_PIN, 0);
}

uint16_t read_motion_sensor() {
  uint16_t motionValue = digitalRead(PIR_PIN);
  return motionValue;
}

float read_temp_sensor() {
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  return temperature;
}

String read_water_level() {
  int waterLevelReading = analogRead(WATER_LEVEL_PIN);
 if (waterLevelReading <= 200) {
    return "Empty";
  } else if (waterLevelReading > 200 && waterLevelReading <= 450) {
    return "Low";
  } else if (waterLevelReading > 450 && waterLevelReading <= 600) {
    return "Medium";
  } else {
    return "Full";
  }
  
}

float read_turbidity() {
  int turbidityReading = analogRead(TURBIDITY_ANALOG_PIN);
  float turbidity = (turbidityReading * TURBIDITY_SLOPE) + TURBIDITY_INTERCEPT;
  return turbidity;
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    attempts++;
    if (attempts > 10) {
      Serial.println("Connection failed. Check credentials.");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to Wi-Fi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

void ReceivedMessage(char* topic, byte* payload, unsigned int payload_length) {
  char msg[256];
  if (payload_length > 0) {
    if (strncmp((char*)payload, "RELAY:OFF", payload_length) == 0) {
      put_off_relay();
    } else if (strncmp((char*)payload, "RELAY:ON", payload_length) == 0) {
      put_on_relay();
    } else if (strncmp((char*)payload, "BUZZER:ON", payload_length) == 0) {
      beep_buzzer();
    }
  }

  // Read and publish sensor data when a message is received
  float turbidity = read_turbidity();
  String waterLevel = read_water_level();
  float temperature = read_temp_sensor();
  uint16_t motion = read_motion_sensor();
  char motion_str[20];

  if (motion == 1) {
    strcpy(motion_str, "Motion Detected");
  } else {
    strcpy(motion_str, "No Motion Dectected");
  }

  sprintf(msg, "Turbidity: %3.2f NTU, Water Level: %s, Temperature: %3.2f °C, Motion: %s", turbidity, waterLevel.c_str(), temperature, motion_str);
  mqtt_client_publish(pub_topic, msg);
  Serial.println(msg);
}

bool Connect() {
  Serial.print("Connecting to MQTT Broker...");
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    client.subscribe(sub_topic);
    Serial.println("Connected.");
    return true;
  } else {
    Serial.print("Failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    return false;
  }
}

boolean mqtt_client_publish(const char* topic, const char* msg) {
  boolean result = client.publish(topic, msg);
  if (!result) {
    Serial.println("Connected, but could not publish");
  }
  return result;
}




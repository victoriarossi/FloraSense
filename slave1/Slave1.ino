#include <Wire.h> // For I2C communication
#include <OneWire.h> // For interfacing with 1-Wire devices
#include <DallasTemperature.h> // For temperature sensors
#include <stdint.h> 

#define ONE_WIRE_BUS 4 // Pin for 1-Wire temperature sensor
#define LED_PIN 8 // Pin connected to LED

OneWire oneWire(ONE_WIRE_BUS); // Initializes OneWire instance to communicate with devices
DallasTemperature sensors(&oneWire); // Initializes DallasTemperature object with OneWire instance

volatile float temperature = 0.0; // Store temperature value
volatile uint16_t sensorValue = 0; // Store moisture sensor value
int sensorPin = A0; // Analog pin for moisture sensor

const int speakerPin = 13; // Pin connected to speaker
const int toneFreq = 1000; // Frequency for tone 
const int toneDur = 500; // Duration of tone

void setup() {
  Serial.begin(9600);
  Wire.begin(8); 
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Wire.onRequest(requestEvent);
  sensors.begin();
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  // Request temperature readings anmd get first reading in Fahrenheit
  sensors.requestTemperatures();
  temperature = sensors.getTempFByIndex(0);
  sensorValue = analogRead(sensorPin);

  // Debugging statement
  Serial.print("Moisture Value: ");
  Serial.println(sensorValue);
  Serial.print("Raw Temp: ");
  Serial.println(temperature);

  // Checks if moisture is low
  if(sensorValue > 400) {
    digitalWrite(LED_PIN, HIGH);
    tone(speakerPin, toneFreq, toneDur);
  }
  // Moisture is good
  else {
    digitalWrite(LED_PIN, LOW);
    noTone(13);
  }
}

void requestEvent() {
  Serial.println("Entering requestEvent");
  // Store data to send I2C
  uint8_t dataBuffer[6];

  // Copies temperature and sensor value to buffer and writes buffer to I2C bus 
  memcpy(dataBuffer, &temperature, sizeof(temperature));
  memcpy(dataBuffer + sizeof(temperature), &sensorValue, sizeof(sensorValue));
  Wire.write(dataBuffer, sizeof(dataBuffer));
  Serial.println("Sent");
}
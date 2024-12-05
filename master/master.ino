#include <Wire.h>                  // Include Wire library for I2C communication
#include <LiquidCrystal.h>         // Include LiquidCrystal library for LCD control
#include <stdint.h>                // Include standard integer type definitions

// Initialize LCD with pin connections
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

// Define slave device addresses
const uint8_t SENSOR_SLAVE_ADDRESS = 8;
const uint8_t RELAY_SLAVE_ADDRESS = 9;
const uint8_t PHOTORESISTOR_ADRESS = 10;

// Define commands for relay control
volatile const char OPEN_VALVE = 'O';
volatile const char CLOSE_VALVE = 'C';

// Initialize sensor variables
float temperature = 0.0;           // Variable to store temperature reading
uint16_t soilMoisture = 0;         // Variable to store soil moisture reading
int photoresistor = 0;             // Variable to store photoresistor reading

void setup() {
  Wire.begin(7);                  // Initialize I2C with address 7
  lcd.begin(16, 2);               // Set up the LCD with 16 columns and 2 rows
  Serial.begin(9600);             // Initialize serial communication at 9600 baud
}

void loop() {
  // Define the number of bytes to request from the sensor slave
  const uint8_t numBytes = (uint8_t)(sizeof(float) + sizeof(uint16_t));

  // Request temperature and soil moisture data from the sensor slave
  Wire.requestFrom((uint8_t)SENSOR_SLAVE_ADDRESS, (uint8_t)numBytes);

  uint8_t dataBuffer[6];          // Buffer to store received data
  uint8_t index = 0;              // Index for filling the buffer

  // Read data into the buffer while available and within the expected size
  while (Wire.available() && index < numBytes) {
    dataBuffer[index++] = Wire.read();
  }

  // Extract temperature and soil moisture values from the data buffer
  memcpy(&temperature, dataBuffer, sizeof(temperature));
  memcpy(&soilMoisture, dataBuffer + sizeof(temperature), sizeof(soilMoisture));

  // Print temperature and soil moisture readings to the serial monitor
  Serial.print("Received Temp: ");
  Serial.println(temperature);

  Serial.print("Received Moisture: ");
  Serial.println(soilMoisture);

  // Display temperature and soil moisture readings on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 2);
  lcd.setCursor(0, 1);
  lcd.print("M:");
  lcd.print(soilMoisture);

  // Request light level data from the photoresistor slave
  Wire.requestFrom((uint8_t)PHOTORESISTOR_ADRESS, (uint8_t)sizeof(int));

  // Check if enough data is available to process
  if (Wire.available() >= sizeof(int)) {
    uint8_t dataBufferPhotoresistor[sizeof(int)];  // Buffer for photoresistor data
    for (uint8_t i = 0; i < sizeof(int); i++) {
      dataBufferPhotoresistor[i] = Wire.read();
    }

    // Extract photoresistor value from the data buffer
    memcpy(&photoresistor, dataBufferPhotoresistor, sizeof(photoresistor));

    // Print photoresistor value to the serial monitor
    Serial.print("Photoresistor: ");
    Serial.println(photoresistor);
  }

  // Begin communication with the relay slave
  Wire.beginTransmission(RELAY_SLAVE_ADDRESS);

  // Check if watering is needed and send the appropriate command
  if (checkWatering()) {
    Wire.write(OPEN_VALVE);       // Command to open the valve
    Serial.println("Command Sent: Open Valve");
  } else {
    Wire.write(CLOSE_VALVE);      // Command to close the valve
    Serial.println("Command Sent: Close Valve");
  }

  Wire.endTransmission();        // End communication with the relay slave

  delay(1000);                   // Wait for 1 second before the next iteration
}

// Function to determine if watering is needed
bool checkWatering() {
  int soil_threshold = 400;      // Default soil moisture threshold

  // Adjust threshold based on light and temperature conditions
  if (photoresistor > 700 && temperature > 80) {
    soil_threshold -= 20;        // Lower threshold for high light and heat
  } else if (photoresistor < 300 && temperature < 65) {
    soil_threshold += 20;        // Raise threshold for low light and cool conditions
  }

  // Return true if soil moisture exceeds the threshold, false otherwise
  return soilMoisture > soil_threshold;
}
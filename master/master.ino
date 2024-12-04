#include <Wire.h>
#include <LiquidCrystal.h>
#include <stdint.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const uint8_t SENSOR_SLAVE_ADDRESS = 8;
const uint8_t RELAY_SLAVE_ADDRESS = 9;
const uint8_t PHOTORESISTOR_ADRESS = 10;

volatile const char OPEN_VALVE = 'O';
volatile const char CLOSE_VALVE = 'C';


float temperature = 0.0;
uint16_t soilMoisture = 0;
int photoresistor = 0;

void setup() {
  Wire.begin(7);
  lcd.begin(16, 2);
  // Wire.onRequest(requestEvent);
  Serial.begin(9600);
}

void loop() {
  // Correctly define numBytes and cast to uint8_t
  const uint8_t numBytes = (uint8_t)(sizeof(float) + sizeof(uint16_t));
  Wire.requestFrom((uint8_t)8, (uint8_t)numBytes);

  uint8_t dataBuffer[6];
  uint8_t index = 0;

  while (Wire.available() && index < numBytes) {
    dataBuffer[index++] = Wire.read();
  }

  // Ensure the data is correctly extracted
  memcpy(&temperature, dataBuffer, sizeof(temperature));
  memcpy(&soilMoisture, dataBuffer + sizeof(temperature), sizeof(soilMoisture));

  Serial.print("Received Temp: ");
  Serial.println(temperature);

  Serial.print("Received Moisture: ");
  Serial.println(soilMoisture);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 2);
  lcd.setCursor(0, 1);
  lcd.print("M:");
  lcd.print(soilMoisture);


  Wire.requestFrom((uint8_t)10, (uint8_t)sizeof(int)); // Request 2 bytes (size of int)

  if (Wire.available() >= sizeof(int)) { // Ensure we have enough data
    uint8_t dataBufferPhotoresistor[sizeof(int)];
    for (uint8_t i = 0; i < sizeof(int); i++) {
      dataBufferPhotoresistor[i] = Wire.read();
    }
    memcpy(&photoresistor, dataBufferPhotoresistor, sizeof(photoresistor)); // Extract value

    // Correctly print the photoresistor value
    Serial.print("Photoresistor: ");
    Serial.println(photoresistor);
  }

  Wire.beginTransmission(RELAY_SLAVE_ADDRESS);
  if(checkWatering()){
    Wire.write(OPEN_VALVE);
    Serial.println("Command Sent: Open Valve");
  } else {
    Wire.write(CLOSE_VALVE);
    Serial.println("Command Sent: Close Valve");
  }
  Wire.endTransmission();
  

  delay(1000);
}

bool checkWatering(){
  int soil_threshold = 400;

  // Adjust threshold based on light and temperature conditions
  if (photoresistor > 700 && temperature > 80) {
      soil_threshold -= 20;  // Lower threshold for high light & heat
  } else if (photoresistor < 300 && temperature < 65) {
      soil_threshold += 20;  // Raise threshold for low light & cool
  }

  // Decision to water
  if (soilMoisture > soil_threshold) {
      return true;
  } else {
      return false;
  }
}
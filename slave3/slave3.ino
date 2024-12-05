#include <LiquidCrystal.h>
#include <Wire.h>

// Pin configuration for the photoresistor and LCD
int photoPin = A0; // Photoresistor is connected to analog pin A0
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7; // LCD pins

// Initialize the LCD (16x2) with the appropriate pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Variables for light sensor and time tracking
int lightRaw; // Stores the raw reading from the photoresistor
long previousMillis = 0; // Tracks time for display updates
const long interval = 1000; // Interval to update display (1 second)
const int MASTER_ADDRESS = 7; // Master device I2C address

void setup() {
  // Initialize the LCD
  lcd.begin(16, 2);

  // Start serial communication for debugging
  Serial.begin(9600);

  // Initialize I2C as a slave device
  Wire.begin(10); // Set slave address to 10

  // Define the behavior when the master requests data
  Wire.onRequest(requestEvent);
}

void loop() {
  // Read the value from the photoresistor
  lightRaw = analogRead(photoPin);

  // Debugging: Print the raw light value to the Serial monitor
  Serial.print("Light raw value: ");
  Serial.println(lightRaw);

  // Display the light level text on the LCD
  displayLightLevel(lightRaw);
  
}

// Function to display the light level on the first line of the LCD
void displayLightLevel(int lightRaw) {
  lcd.setCursor(0, 0); // Set cursor to the top line of the LCD

  // Determine the light level based on thresholds and display a message
  if (lightRaw <= 200) {
    lcd.print("Dark           "); // Pad with spaces to clear previous text
  } else if (lightRaw > 200 && lightRaw <= 400) {
    lcd.print("Partially Dark ");
  } else if (lightRaw > 400 && lightRaw <= 600) {
    lcd.print("Medium         ");
  } else if (lightRaw > 600 && lightRaw <= 720) {
    lcd.print("Fully Lit      ");
  } else {
    lcd.print("Brightly Lit   ");
  }
}

// I2C request event handler: sends the lightRaw value to the master device
void requestEvent() {
  Serial.println("Got request"); // Debugging: log that a request was received

  uint8_t dataBuffer[sizeof(lightRaw)]; // Create a buffer to hold the lightRaw value
  memcpy(dataBuffer, &lightRaw, sizeof(lightRaw)); // Copy lightRaw into the buffer

  Wire.write(dataBuffer, sizeof(dataBuffer)); // Send the buffer via I2C

  // Debugging: confirm the sent value
  Serial.print("Sent lightRaw value: ");
  Serial.println(lightRaw);
}
#include <Wire.h>

// Pin and variable definitions
const int RELAY_PIN = 5;  // Relay connected to digital pin D5
char receivedCommand = 'N'; // Holds the command received from the master device
const int interval = 5000;  // Interval for timing-related features (not used directly here)
unsigned long prevMilli = 0; // Previous time marker for timing-related features
const int Button = 7;  // Button connected to digital pin D7
int lastButtonState = HIGH; // Last known state of the button (HIGH = not pressed)
bool relayState = LOW; // Current state of the relay (LOW = off)
long lastDebounceTime = 0; // Last time the button state changed
const long debounceDelay = 50; // Debounce delay to avoid false button presses
bool close = true; // Tracks whether the relay should close (true = turn on)
bool readCommand = true; // Determines if the relay should act based on the received command

void setup() {
  pinMode(RELAY_PIN, OUTPUT); // Set relay pin as output
  digitalWrite(RELAY_PIN, LOW); // Ensure the relay is initially off
  Wire.begin(9); // Initialize the I2C slave with address 9
  Serial.begin(9600); // Start Serial communication for debugging
  pinMode(Button, INPUT_PULLUP); // Configure button pin with an internal pull-up resistor
}

void loop() {
  // Get the current time in milliseconds
  unsigned long currMilli = millis();
  
  // Read the current state of the button
  int buttonState = digitalRead(Button);

  // Request a command from the master device
  const uint8_t numBytes = (uint8_t)(sizeof(char)); // Number of bytes to request
  Wire.requestFrom((uint8_t)7, numBytes); // Request data from the master device (address 7)
  while (Wire.available()) {
    Serial.println("Wire available"); // Debugging: confirm data availability
    receivedCommand = Wire.read(); // Read the received command
  }

  // Debugging: Print the received command to the Serial monitor
  Serial.print("Received Command: ");
  Serial.println(receivedCommand);

  // Handle button press logic with debounce
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {  // Button was pressed
      Serial.print("READ COMMAND: ");
      Serial.println(readCommand);

      readCommand = false; // Temporarily disable command reading
      if (close) {
        digitalWrite(RELAY_PIN, HIGH); // Turn the relay on
        close = !close; // Toggle the close state
      } else {
        digitalWrite(RELAY_PIN, LOW); // Turn the relay off
        close = !close; // Toggle the close state
        readCommand = true; // Re-enable command reading
      }
    }
    lastButtonState = buttonState; // Update the last known button state
  }

  // Handle commands from the master device if readCommand is true
  if (readCommand) {
    if (receivedCommand == 'O') { // 'O' command: Open the valve
      digitalWrite(RELAY_PIN, HIGH); // Turn the relay on
      Serial.println("OPEN VALVE"); // Debugging: Log action
    } else { // Any other command: Close the valve
      digitalWrite(RELAY_PIN, LOW); // Turn the relay off
      Serial.println("CLOSE VALVE"); // Debugging: Log action
    }
  }
}
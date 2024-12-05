#include <Wire.h>

void setup() {
  Serial.begin(9600);  // Start serial communication
  Wire.begin();        // Start I2C communication

  Serial.println("\nI2C Scanner - Starting...");
  delay(1000); // Wait a moment to ensure everything is ready

  Serial.println("Scanning I2C bus...");

  // Scan through all possible addresses (0x08 to 0x77)
  for (byte address = 8; address < 120; address++) {
    Serial.print("Checking address 0x");
    if (address < 16) {
      Serial.print("0");  // Format the address with a leading zero for addresses below 0x10
    }
    Serial.println(address, HEX);

    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");  // Format the address with a leading zero for addresses below 0x10
      }
      Serial.println(address, HEX);
    } else {
      Serial.print("No device found at address 0x");
      if (address < 16) {
        Serial.print("0");  // Format the address with a leading zero for addresses below 0x10
      }
      Serial.println(address, HEX);
    }

    delay(50);  // Small delay to avoid overloading the bus
  }

  Serial.println("\nI2C scan complete.");
}

void loop() {
  // Nothing to do in the loop, scanning happens in setup
}
